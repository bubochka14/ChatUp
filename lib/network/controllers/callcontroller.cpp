#include "callcontroller.h"
Q_LOGGING_CATEGORY(LC_CALL_CONTROLLER, "CallerCallController");
using namespace Call;
struct DumpStreamSource : public Media::Video::StreamSource
{
	std::shared_ptr<Media::FramePipe> frameOutput() override
	{
		return pipe;
	}
	QFuture<Media::Video::SourceConfig> open() override
	{
		return QtFuture::makeReadyValueFuture(Media::Video::SourceConfig());
	}
	void close() override
	{

	}
	bool isOpen() override
	{
		return true;
	}
	std::shared_ptr<Media::FramePipe> pipe;
};
Controller::Controller(std::shared_ptr<NetworkCoordinator> m, QObject* parent)
	:AbstractController(parent)
	,_manager(m)
{
	rtc::Configuration config;
	config.iceServers.emplace_back("stun:stun.l.google.com:19302");
	//config.disableAutoNegotiation = true;
	_rtc = std::make_shared<rtc::Service>(m, std::move(config));

	Api::Join::handle(m, [this](Participate::Data&& part) {
		QtFuture::makeReadyFuture().then(this, [this, part = std::move(part)]() {
			auto h = handler(part.roomID);
			if (h->participants()->idToIndex(part.userID).isValid())
				return;
			h->participants()->insertData(0, std::move(part));
			});
		});
	Api::Disconnect::handle(m, [this](Participate::Data&& part) {
		QtFuture::makeReadyFuture().then(this, [this, part = std::move(part)]() {
			if (_handlers.contains(part.roomID))
			{
				Handler* handler = _handlers[part.roomID];
				QModelIndex index = handler->participants()->idToIndex(part.userID);
				if (index.isValid())
				{
					handler->participants()->removeRow(index.row());
				}
				else
					qCWarning(LC_CALL_CONTROLLER) << "Unknown userID in disconnectCall: "<< part.userID;

			}
			else
				qCWarning(LC_CALL_CONTROLLER) << "Unknown roomID in disconnectCall: "<< part.roomID;
			});
		});
}
Handler::Handler(Controller* controller, int roomID)
	:_controller(controller)
	, _roomID(roomID)
	, _isMuted(false)
	, _prt(new Participate::Model(this))
	, _state(Disconnected)
{
}
void Handler::setState(State other)
{
	if (other == _state)
		return;
	_state = other;
	emit stateChanged();
}
Handler::State Handler::state() const
{
	return _state;
}
void Handler::closeVideo()
{
	return _controller->closeVideo(this);
}
QFuture<void> Handler::closeAudio()
{
	return QtFuture::makeReadyVoidFuture();

}
QFuture<void> Handler::openVideo(Media::Video::StreamSource* source)
{
	return _controller->openVideo(this, source);
}
//QFuture<void> Handler::openAudio(Media::StreamSource* source)
//{
//	return _controller->openAudio(this, source);
//}
QFuture<void> Controller::openVideo(Handler* h, Media::Video::StreamSource* st)
{
	return st->open().then([this, st, h](Media::Video::SourceConfig&& config)
		{
			int currentUser = _manager->currentUser();
			if (!_userContexts.contains(currentUser))
				_userContexts[currentUser] = StreamContext();

			_userContexts[_manager->currentUser()].videoSource = st;
			Participate::Model* model = h->participants();
			for (size_t i = 0; i < model->rowCount(); i++)
			{
				int paricipantID = model->data(model->index(i),
					Participate::Model::IDRole()).toInt();
				if (paricipantID == currentUser)
					continue;
				auto pcHandle = _rtc->getPeerConnectionHandle(paricipantID);
				if (!pcHandle)
					qCCritical(LC_CALL_CONTROLLER) << "Call participant and rtc peer not sync";
				else
 					pcHandle->openLocalVideo(st->frameOutput(),config);

			};
			model->setData(model->idToIndex(currentUser), true, Participate::Model::HasVideo);

		});

}
void Controller::connectVideoSink(Handler* h, int userID, QVideoSink* s)
{
	if (!_userContexts.contains(userID))
	{
		qCWarning(LC_CALL_CONTROLLER) << "Cannot connect to video sink, user" << userID << "haven`t media context";
		return;
	}
	if (!_userContexts[userID].videoSource)
	{
		qCWarning(LC_CALL_CONTROLLER) << "Cannot connect to video sink, user" << userID << "haven`t open video stream";
		return;
	}
	_userContexts[userID].videoSinkConnector.reset(new Media::Video::SinkConnector(
		_userContexts[userID].videoSource->frameOutput(),s));
}

void Handler::connectVideoSink(int userID, QVideoSink* sink)
{
	_controller->connectVideoSink(this, userID, sink);
}
//QFuture<void> Controller::openAudio(Handler* h, Media::StreamSource* st)
//{
//	return QtFuture::makeReadyVoidFuture();
//}
QFuture<void> Controller::disconnect(Handler* h)
{
	if(h->state() != Handler::InsideTheCall)
		return QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
	Call::Api::Disconnect req;
	req.roomID = h->roomID();
	return req.exec(_manager).then([this, h]() {
		h->setState(Handler::Disconnected);
		auto model = h->participants();
		model->removeRow(model->idToIndex(_manager->currentUser()).row());
		_userContexts.clear();
	});
}

QFuture<void> Handler::disconnect()
{
	return _controller->disconnect(this);
}

Participate::Model* Handler::participants()
{
	if (!_prt)
		_prt = new Participate::Model(this);
	return _prt;
}
QFuture<void> Controller::join(Handler* h)
{
	if (h->state() == Handler::InsideTheCall)
		QtFuture::makeExceptionalFuture(std::make_exception_ptr("Already inside the call"));
	Call::Api::Join req;
	req.roomID = h->roomID();
	return req.exec(_manager).then([this,h]() {
		h->setState(Handler::InsideTheCall);
		Participate::Data current;
		Participate::Model*model = h->participants();
		int userID;
		for (size_t i = 0; i < model->rowCount(); i++)
		{
			userID = model->data(model->index(i),Participate::Model::IDRole()).toInt();
			_rtc->establishPeerConnection(userID).then([this,model, userID]() {
				auto pcHandle = _rtc->getPeerConnectionHandle(userID);
				pcHandle->onRemoteVideoOpen([model,userID,this](std::shared_ptr<Media::FramePipe> fr) {
					DumpStreamSource* str = new DumpStreamSource;
					str->pipe = fr;
					_userContexts[userID].videoSource = str;
					model->setData(model->idToIndex(userID), Participate::Model::HasVideo, true);
					});
				});
		}
		current.hasAudio = h->hasAudio();
		current.hasVideo = h->hasVideo();
		current.userID = _manager->currentUser();
		h->participants()->insertData(0, std::move(current));
		});
}
void Controller::release(Handler* h)
{
	if (_handlers.contains(h->roomID()))
	{
		_handlers.remove(h->roomID());
	}
	h->deleteLater();

}
Handler* Controller::handler(int roomID)
{
	if (_handlers.contains(roomID))
	{
		return _handlers[roomID];
	}
	auto handler = new Handler(this, roomID);
	_handlers[roomID] = handler;
	return handler;
}
QFuture<void> Handler::join()
{
	return _controller->join(this);
}
void Handler::release()
{

}
int Handler::roomID() const
{
	return _roomID;
}
void Handler::setHasAudio(bool st)
{
	_controller->setAudio(st, this);

}
void Handler::setHasVideo(bool st)
{
	_controller->setVideo(st, this);
}
void Controller::setAudio(bool st, Handler* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return;
	if (model->data(indexOfCurrent, Participate::Model::HasAudio) == st)
		return;
	model->setData(indexOfCurrent, st,Participate::Model::HasAudio);
	emit h->hasAudioChanged();
}
void Controller::setVideo(bool st, Handler* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return;
	if (model->data(indexOfCurrent, Participate::Model::HasVideo) == st)
		return;
	model->setData(indexOfCurrent, st, Participate::Model::HasVideo);
	emit h->hasVideoChanged();
}
bool Controller::hasAudio(Handler* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return false;
	return model->data(indexOfCurrent, Participate::Model::HasAudio).toBool();
}
bool Controller::hasVideo(Handler* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return false;
	return model->data(indexOfCurrent, Participate::Model::HasVideo).toBool();
}
bool Handler::hasVideo()
{
	return _controller->hasVideo(this);
}
bool Handler::hasAudio()
{
	return _controller->hasAudio(this);

}
void Controller::closeVideo(Handler* h)
{
	if (!_userContexts.contains(_manager->currentUser()))
	{
		qCWarning(LC_CALL_CONTROLLER) << "Attempt to close a video stream while it is not open";
		return;
	}
	_userContexts[_manager->currentUser()].videoSource->close();
	auto model = h->participants();
	model->setData(model->idToIndex(_manager->currentUser()), false, Participate::Model::HasVideo);
}