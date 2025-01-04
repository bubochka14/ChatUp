#include "callcontroller.h"
using namespace Call;
Controller::Controller(NetworkManager* m, QObject* parent)
	:AbstractController(parent)
	,_manager(m)
{}
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
QFuture<void> Controller::disconnect(Handler* h)
{
	Call::Api::Disconnect req;
	req.roomID = h->roomID();
	return req.exec(_manager).then([this, h]() {
		h->setState(Handler::Disconnected);
		auto model = h->participants();
		model->removeRow(model->idToIndex(_manager->userID()).row());
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
	Call::Api::Join req;
	req.roomID = h->roomID();
	return req.exec(_manager).then([this,h]() {
		h->setState(Handler::InsideTheCall);
		Participate::Data current;
		current.hasAudio = true;
		current.userID = _manager->userID();
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
void Handler::setIsMuted(bool other)
{
	if (_isMuted == other)
		return;
	_isMuted = other;
	emit isMutedChanged();
}

bool Handler::isMuted() const
{
	return _isMuted;
}