#include "callscontroller.h"
using namespace Calls;
Controller::Controller(QObject* parent)
	:QObject(parent)
{}
Handler::Handler(Controller* controller, int roomID)
	:_controller(controller)
	,_roomID(roomID)
	,_isMuted(false)
	,_hasCall(false)
{

}
QFuture<void> Controller::disconnect(Handler* other)
{
	other->setHasCall(false);
	return QtFuture::makeReadyVoidFuture();
}
void Handler::setHasCall(bool other)
{
	if (_hasCall == other)
		return;
	_hasCall = other;
	emit hasCallChanged();
}
QFuture<void> Handler::disconnect()
{
	return _controller->disconnect(this);
}

ParticipateModel* Handler::participants() 
{
	if(!_prt)
		_prt = new ParticipateModel(this);
	return _prt;
}
QFuture<bool> Controller::join(Handler* h)
{
	h->setHasCall(true);
	h->participants();
	return QtFuture::makeReadyValueFuture<bool>(true);
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
	auto handler = new Handler(this,roomID);
	_handlers[roomID] = handler;
	return handler;
}
QFuture<bool> Handler::join()
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
bool Handler::hasCall() const
{
	return _hasCall;
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