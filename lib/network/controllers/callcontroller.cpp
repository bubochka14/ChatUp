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
	_rtc = std::make_shared<rtc::Service>(_manager, std::move(config));
	Media::Audio::Output* out = new Media::Audio::Output();
	Api::Join::handle(m, [this](Participate::Data&& part) {
		QtFuture::makeReadyFuture().then(this, [this, part = std::move(part)]() {
			auto h = handler(part.roomID);
			{
				std::lock_guard g(_handlersMutex);
				auto model = h->participants();
				if (model->idToIndex(part.userID).isValid())
					return;
				model->insertData(0, std::move(part));
				int currentUser = _manager->currentUser();
				if (model->data(model->idToIndex(currentUser), Participate::Model::HasVideo).toBool())
				{
					std::lock_guard g(_localVideoStream.mutex);
					_rtc->openLocalVideo(part.userID, _localVideoStream.src->frameOutput(), _localVideoStream.config);
				}
				if (model->data(model->idToIndex(currentUser), Participate::Model::HasAudio).toBool())
				{
					std::lock_guard g(_localAudioStream.mutex);
					_rtc->openLocalAudio(part.userID, _localAudioStream.src->frameOutput(), _localAudioStream.config);
				}
			}
		});
	});
	Api::UpdateCallMedia::handle(m, [this](Api::UpdateCallMedia::MediaUpdate upd){

		Call::Handler* callHandler = handler(upd.roomID);
		std::lock_guard g(_handlersMutex);

		Participate::Model* pt = callHandler->participants();
		auto index = pt->idToIndex(upd.userID);
		if (!index.isValid())
		{
			qCWarning(LC_CALL_CONTROLLER) << "Unknown userID received";
			return;
		}
		if (pt->data(index, Participate::Model::HasVideo).toBool() == true && upd.video == false)
			_rtc->flushRemoteVideo(upd.userID);
		if (pt->data(index, Participate::Model::HasAudio).toBool() == true && upd.audio == false)
			_rtc->flushRemoteAudio(upd.userID);
		pt->setData(index, upd.video, Participate::Model::HasVideo);
		pt->setData(index, upd.audio, Participate::Model::HasAudio);

	});
	Api::Disconnect::handle(m, [this](Participate::Data&& part) {
		QtFuture::makeReadyFuture().then(this, [this, part = std::move(part)]() {
			std::lock_guard g(_handlersMutex);
			_connectors.erase(part.userID);
			if (_handlers.contains(part.roomID))
			{
				auto handler = _handlers[part.roomID];
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
	,_roomID(roomID)
	,_isMuted(false)
	,_prt(new Participate::Model(this))
	,_state(Disconnected)
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
void Handler::closeAudio()
{
	return _controller->closeAudio(this);

}
QFuture<void> Handler::openVideo(Media::Video::StreamSource* source)
{
	return _controller->openVideo(this, source);
}
//QFuture<void> Handler::openAudio(Media::StreamSource* source)
//{
//	return _controller->openAudio(this, source);
//}
void Handler::connectAudioOutput(int userID, Media::Audio::Output*out)
{
	_controller->connectAudioOutput(this, userID, out);
}
void Controller::connectAudioOutput(Handler* h, int userID, Media::Audio::Output* out)
{
	using namespace Media::Audio;
	out->start(out->availableDevices().first(), _rtc->getRemoteAudio(userID));
}
void Controller::connectVideoSink(Handler* h, int userID, QVideoSink* s)
{
	using namespace Media::Video;
	std::shared_ptr<SinkConnector> connector;
	{
		std::lock_guard g(_localVideoStream.mutex);
		if(!_connectors.contains(userID))
			_connectors[userID] = std::make_shared<SinkConnector>();;
		_connectors[userID]->connect(s);
		if(userID != _manager->currentUser())
			_connectors[userID]->connect(_rtc->getRemoteVideo(userID));
		else
			_connectors[userID]->connect(_localVideoStream.src->frameOutput());
	}
}

void Handler::connectVideoSink(int userID, QVideoSink* sink)
{
	_controller->connectVideoSink(this, userID, sink);
}
void Controller::clearMedia()
{
	{
		std::lock_guard g(_localAudioStream.mutex);
		_localAudioStream.config = Media::Audio::SourceConfig();
		if (_localAudioStream.src)
			_localAudioStream.src->close();
		_localAudioStream.src = nullptr;
	}
	{
		std::lock_guard g(_localVideoStream.mutex);
		if (_localVideoStream.src)
			_localVideoStream.src->close();
		_localVideoStream.src = nullptr;
		_localVideoStream.config = Media::Video::SourceConfig();

	}
	_connectors.clear();
}
QFuture<void> Controller::disconnect(Handler* h)
{
	Call::Api::Disconnect req;
	{
		std::lock_guard g(_handlersMutex);
		if (h->state() != Handler::InsideTheCall)
			return QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
		req.roomID = h->roomID();
	}
	clearMedia();
	_rtc->closeAllConnections();
	return req.exec(_manager).then([this, h]() {
		{
			std::lock_guard g(_handlersMutex);
			h->setState(Handler::Disconnected);
			auto model = h->participants();
			model->removeRow(model->idToIndex(_manager->currentUser()).row());
		}
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
void Controller::closeAudio(Handler* h)
{
	{
		std::lock_guard g(_localAudioStream.mutex);
		if (_localAudioStream.src)
		{
			_localAudioStream.src->close();
			_localAudioStream.src = nullptr;
		}
	}
	{
		std::lock_guard g(_handlersMutex);
		setAudio(false, h);
		auto model = h->participants();
		int currentUser = _manager->currentUser();
		for (size_t i = 0; i < model->rowCount(); i++)
		{
			int paricipantID = model->data(model->index(i),
				Participate::Model::IDRole()).toInt();
			if (paricipantID == currentUser)
				continue;
			_rtc->closeLocalAudio(paricipantID);

		};
	}
	emit h->hasAudioChanged();

	Api::UpdateCallMedia req;
	req.audio = false;
	req.exec(_manager);
}
QFuture<void> Controller::join(Handler* h)
{
	Call::Api::Join req;
	{
		std::lock_guard g(_handlersMutex);
		if (h->state() == Handler::InsideTheCall)
			QtFuture::makeExceptionalFuture(std::make_exception_ptr("Already inside the call"));
		req.roomID = h->roomID();
	}
	return req.exec(_manager).then([this,h]() {
		Participate::Data current;
		int userID;

		std::lock_guard g(_handlersMutex);
		h->setState(Handler::InsideTheCall);
		_activeCallRoomID = h->roomID();
		Participate::Model*model = h->participants();
		for (size_t i = 0; i < model->rowCount(); i++)
		{
			userID = model->data(model->index(i),Participate::Model::IDRole()).toInt();
			if (userID == _manager->currentUser())
				continue;
		}
		current.hasAudio = false;
		current.hasVideo = false;
		current.userID   = _manager->currentUser();
		h->participants()->insertData(0, std::move(current));
		});
}
void Controller::release(Handler* h)
{
	std::lock_guard g(_handlersMutex);

	if (_handlers.contains(h->roomID()))
	{
		_handlers.erase(h->roomID());
	}
	h->deleteLater();

}
Handler* Controller::handler(int roomID)
{
	std::lock_guard g(_handlersMutex);

	if (_handlers.contains(roomID))
		return _handlers[roomID];
	auto handler = new Handler(this, roomID);
	_handlers[roomID] = handler;
	Api::Get req;
	req.roomID = roomID;
	req.exec(_manager).then(this, [this,roomID, handler](std::vector<Participate::Data> res) {
		handler->participants()->insertRange(0,std::make_move_iterator(res.begin()), std::make_move_iterator(res.end()));
	});
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
}
bool Controller::hasAudio(Handler* h)
{
	std::lock_guard g(_handlersMutex);

	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return false;
	return model->data(indexOfCurrent, Participate::Model::HasAudio).toBool();
}
bool Controller::hasVideo(Handler* h)
{
	std::lock_guard g(_handlersMutex);

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
QFuture<void> Handler::openAudio(Media::Audio::StreamSource* source)
{
	return _controller->openAudio(this, source);
}
QFuture<void> Controller::openVideo(Handler* h, Media::Video::StreamSource* st)
{
	return st->open().then([this, st, h](Media::Video::SourceConfig&& config){
		int currentUser = _manager->currentUser();
		{
			std::lock_guard g(_localVideoStream.mutex);
			if (_localVideoStream.src && _localVideoStream.src !=st)
				_localVideoStream.src->close();
			_localVideoStream.src = st;
			_localVideoStream.config = config;
		}		
		{
			std::lock_guard g(_handlersMutex);
			setVideo(true, h);
		}
		emit h->hasVideoChanged();
		Api::UpdateCallMedia req;
		req.video = true;
		req.exec(_manager).then([this,h, currentUser,config = std::move(config)]() {
			std::lock_guard gs(_localVideoStream.mutex);
			if (!_localVideoStream.src)
				return;
			std::lock_guard gh(_handlersMutex);
			auto model = h->participants();
			for (size_t i = 0; i < model->rowCount(); i++)
			{
				int paricipantID = model->data(model->index(i),
					Participate::Model::IDRole()).toInt();
				if (paricipantID != currentUser)
					_rtc->openLocalVideo(paricipantID, _localVideoStream.src->frameOutput(),std::move(config));

			};
		});

	});

}
QFuture<void> Controller::openAudio(Handler* h, Media::Audio::StreamSource* st)
{
	return st->open().then([this, st, h](Media::Audio::SourceConfig&& config) {
		int currentUser = _manager->currentUser();
		{
			std::lock_guard g(_localAudioStream.mutex);
			if (_localAudioStream.src&& _localAudioStream.src!= st)
				_localAudioStream.src->close();
			_localAudioStream.src = st;
			_localAudioStream.config = config;
		}
		{
			std::lock_guard g(_handlersMutex);
			setAudio(true, h);
		}
		emit h->hasAudioChanged();
		Api::UpdateCallMedia req;
		req.audio = true;
		req.exec(_manager).then([this, h, currentUser, config = std::move(config)]() {
			std::lock_guard gs(_localAudioStream.mutex);
			if (!_localAudioStream.src)
				return;
			std::lock_guard gh(_handlersMutex);
			auto model = h->participants();
			for (size_t i = 0; i < model->rowCount(); i++)
			{
				int paricipantID = model->data(model->index(i),
					Participate::Model::IDRole()).toInt();
				if (paricipantID != currentUser)
					_rtc->openLocalAudio(paricipantID, _localAudioStream.src->frameOutput(),std::move(config));
			};
		});
	});
}
void Controller::closeVideo(Handler* h)
{
	{
		std::lock_guard g(_localVideoStream.mutex);
		if (_localVideoStream.src)
		{
			_localVideoStream.src->close();
			_localVideoStream.src = nullptr;
		}
	}
	{
		std::lock_guard g(_handlersMutex);
		setVideo(false, h);
		auto model = h->participants();
		int currentUser = _manager->currentUser();
		for (size_t i = 0; i < model->rowCount(); i++)
		{
			int paricipantID = model->data(model->index(i),
				Participate::Model::IDRole()).toInt();
			if (paricipantID == currentUser)
				continue;
			_rtc->closeLocalVideo(paricipantID);

		};
	}
	Api::UpdateCallMedia req;
	req.video = false;
	req.exec(_manager);
}