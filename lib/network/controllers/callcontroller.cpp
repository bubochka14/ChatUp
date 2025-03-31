#include "callcontroller.h"
Q_LOGGING_CATEGORY(LC_CALL_CONTROLLER, "CallerCallController");
using  namespace Call;
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
void Controller::reset()
{
	if(_activeCallRoomID.has_value())
		disconnect(_handles[_activeCallRoomID.value()]);
	std::lock_guard g(_handlesMutex);
	_activeCallRoomID = std::nullopt;
	_handles.clear();
}
Controller::Controller(std::shared_ptr<NetworkCoordinator> m, QObject* parent)
	:AbstractController(parent)
	,_manager(m)
{
	rtc::Configuration config;
	config.iceServers.emplace_back("stun:stun.l.google.com:19302");
	//config.disableAutoNegotiation = true;
	growHandlePool(12);
	_rtc = std::make_shared<rtc::Service>(_manager, std::move(config));
	Media::Audio::Output* out = new Media::Audio::Output();
	Api::Join::handle(m, [this](Participate::Data&& part) {
		QtConcurrent::run([this,part = std::move(part)]() {
			auto h = handle(part.roomID);
			{
				std::lock_guard g(_handlesMutex);
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

		Call::Handle* callHandle = handle(upd.roomID);
		std::lock_guard g(_handlesMutex);

		Participate::Model* pt = callHandle->participants();
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
			_rtc->closeUserConnection(part.userID);
			{
				std::lock_guard g(_localVideoStream.mutex);
				_localVideoStream.connectors.erase(part.userID);
			}
			std::lock_guard g(_handlesMutex);
			_activeCallRoomID = std::nullopt;
			if (_handles.contains(part.roomID))
			{
				auto Handle = _handles[part.roomID];
				QModelIndex index = Handle->participants()->idToIndex(part.userID);
				if (index.isValid())
				{
					Handle->participants()->removeRow(index.row());
				}
				else
					qCWarning(LC_CALL_CONTROLLER) << "Unknown userID in disconnectCall: "<< part.userID;

			}
			else
				qCWarning(LC_CALL_CONTROLLER) << "Unknown roomID in disconnectCall: "<< part.roomID;
			});
		});
}
void Handle::setRoomID(int other)
{
	if (_roomID == other)
		return;
	_roomID = other;
}
Handle::Handle(Controller* controller, int roomID)
	:_controller(controller)
	,_roomID(roomID)
	,_isMuted(false)
	,_prt(new Participate::Model(this))
	,_state(Disconnected)
{
}
void Handle::setState(State other)
{
	if (other == _state)
		return;
	_state = other;
	emit stateChanged();
}
Handle::State Handle::state() const
{
	return _state;
}
void Handle::closeVideo()
{
	return _controller->closeVideo(this);
}
void Handle::closeAudio()
{
	return _controller->closeAudio(this);

}
QFuture<void> Handle::openVideo(Media::Video::StreamSource* source)
{
	return _controller->openVideo(this, source);
}
//QFuture<void> Handle::openAudio(Media::StreamSource* source)
//{
//	return _controller->openAudio(this, source);
//}
void Handle::connectAudioOutput(int userID, Media::Audio::Output*out)
{
	_controller->connectAudioOutput(this, userID, out);
}
void Controller::connectAudioOutput(Handle* h, int userID, Media::Audio::Output* out)
{
	using namespace Media::Audio;
	out->start(out->availableDevices().first(), _rtc->getRemoteAudio(userID));
}
void Controller::connectVideoSink(Handle* h, int userID, QVideoSink* s)
{
	using namespace Media::Video;
	std::shared_ptr<SinkConnector> connector;
	{
		std::lock_guard g(_localVideoStream.mutex);
		if(!_localVideoStream.connectors.contains(userID))
			_localVideoStream.connectors[userID] = std::make_shared<SinkConnector>();;
		_localVideoStream.connectors[userID]->connect(s);
		if(userID != _manager->currentUser())
			_localVideoStream.connectors[userID]->connect(_rtc->getRemoteVideo(userID));
		else if(_localVideoStream.src)
			_localVideoStream.connectors[userID]->connect(_localVideoStream.src->frameOutput());
	}
}

void Handle::connectVideoSink(int userID, QVideoSink* sink)
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

		_localVideoStream.connectors.clear();
	}
}
QFuture<void> Controller::disconnect(Handle* h)
{
	Call::Api::Disconnect req;
	{
		std::lock_guard g(_handlesMutex);
		if (h->state() != Handle::InsideTheCall)
			return QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
		req.roomID = h->roomID();
	}
	clearMedia();
	_rtc->closeAllConnections();
	return req.exec(_manager).then([this, h]() {
		{
			std::lock_guard g(_handlesMutex);
			h->setState(Handle::Disconnected);
			auto model = h->participants();
			model->removeRow(model->idToIndex(_manager->currentUser()).row());
		}
	});
}

QFuture<void> Handle::disconnect()
{
	return _controller->disconnect(this);
}

Participate::Model* Handle::participants()
{
	if (!_prt)
		_prt = new Participate::Model(this);
	return _prt;
}
void Controller::closeAudio(Handle* h)
{
	QtConcurrent::run([this, h]() {
		{
			std::lock_guard g(_localAudioStream.mutex);
			if (_localAudioStream.src)
			{
				_localAudioStream.src->close();
				_localAudioStream.src = nullptr;
			}
		}
		{
			std::lock_guard g(_handlesMutex);
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
		});
}
QFuture<void> Controller::join(Handle* h)
{
	return QtConcurrent::run([this, h]() {
		Call::Api::Join req;
		{
			std::lock_guard g(_handlesMutex);
			if (h->state() == Handle::InsideTheCall)
				QtFuture::makeExceptionalFuture(std::make_exception_ptr("Already inside the call"));
			req.roomID = h->roomID();
		}
		req.exec(_manager).waitForFinished();
		Participate::Data current;
		int userID;
		std::lock_guard g(_handlesMutex);
		h->setState(Handle::InsideTheCall);
		_activeCallRoomID = h->roomID();
		Participate::Model* model = h->participants();
		for (size_t i = 0; i < model->rowCount(); i++)
		{
			userID = model->data(model->index(i), Participate::Model::IDRole()).toInt();
			if (userID == _manager->currentUser())
				continue;
		}
		current.hasAudio = false;
		current.hasVideo = false;
		current.userID = _manager->currentUser();
		h->participants()->insertData(0, std::move(current));
	});
}
void Controller::release(Handle* h)
{
	std::lock_guard g(_handlesMutex);

	if (_handles.contains(h->roomID()))
	{
		_freeHandlePool.emplace(_handles[h->roomID()]);
		_handles.erase(h->roomID());
	}
	h->deleteLater();

}
void Controller::growHandlePool(size_t size)
{
	//run on controller`s thread
	QtFuture::makeReadyFuture().then(this, [this, size]() {
		for (size_t i = 0; i < size; i++)
		{
			_freeHandlePool.emplace(new Handle(this));
		}
	});
}
Handle* Controller::handle(int roomID)
{
	std::lock_guard g(_handlesMutex);

	if (_handles.contains(roomID))
		return _handles[roomID];
	if(_freeHandlePool.empty())
		growHandlePool(max(_handles.size(),10));
	auto newHandle = _freeHandlePool.top();
	_freeHandlePool.pop();
	newHandle->setRoomID(roomID);
	_handles[roomID] = newHandle;
	Api::Get req;
	req.roomID = roomID;
	req.exec(_manager).then(this, [this,roomID, newHandle](std::vector<Participate::Data> res) {
		std::lock_guard g(_handlesMutex);

		newHandle->participants()->insertRange(0,std::make_move_iterator(res.begin()), std::make_move_iterator(res.end()));
	});
	return newHandle;
}
QFuture<void> Handle::join()
{
	return _controller->join(this);
}
void Handle::release()
{

}
int Handle::roomID() const
{
	return _roomID;
}
void Handle::setHasAudio(bool st)
{
	_controller->setAudio(st, this);

}
void Handle::setHasVideo(bool st)
{
	_controller->setVideo(st, this);
}
void Controller::setAudio(bool st, Handle* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return;
	if (model->data(indexOfCurrent, Participate::Model::HasAudio) == st)
		return;
	model->setData(indexOfCurrent, st,Participate::Model::HasAudio);
}
void Controller::setVideo(bool st, Handle* h)
{
	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return;
	if (model->data(indexOfCurrent, Participate::Model::HasVideo) == st)
		return;
	model->setData(indexOfCurrent, st, Participate::Model::HasVideo);
}
bool Controller::hasAudio(Handle* h)
{
	std::lock_guard g(_handlesMutex);

	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return false;
	return model->data(indexOfCurrent, Participate::Model::HasAudio).toBool();
}
bool Controller::hasVideo(Handle* h)
{
	std::lock_guard g(_handlesMutex);

	Participate::Model* model = h->participants();
	QModelIndex indexOfCurrent = model->idToIndex(_manager->currentUser());
	if (!indexOfCurrent.isValid())
		return false;
	return model->data(indexOfCurrent, Participate::Model::HasVideo).toBool();
}
bool Handle::hasVideo()
{
	return _controller->hasVideo(this);
}
bool Handle::hasAudio()
{
	return _controller->hasAudio(this);

}
QFuture<void> Handle::openAudio(Media::Audio::StreamSource* source)
{
	return _controller->openAudio(this, source);
}
QFuture<void> Controller::openVideo(Handle* h, Media::Video::StreamSource* st)
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
			std::lock_guard g(_handlesMutex);
			setVideo(true, h);
		}
		emit h->hasVideoChanged();
		Api::UpdateCallMedia req;
		req.video = true;
		req.exec(_manager).then([this,h, currentUser,config = std::move(config)]() {
			std::lock_guard gs(_localVideoStream.mutex);
			if (!_localVideoStream.src)
				return;
			std::lock_guard gh(_handlesMutex);
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
QFuture<void> Controller::openAudio(Handle* h, Media::Audio::StreamSource* st)
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
			std::lock_guard g(_handlesMutex);
			setAudio(true, h);
		}
		emit h->hasAudioChanged();
		Api::UpdateCallMedia req;
		req.audio = true;
		req.exec(_manager).then([this, h, currentUser, config = std::move(config)]() {
			std::lock_guard gs(_localAudioStream.mutex);
			if (!_localAudioStream.src)
				return;
			std::lock_guard gh(_handlesMutex);
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
void Controller::closeVideo(Handle* h)
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
		std::lock_guard g(_handlesMutex);
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