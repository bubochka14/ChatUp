#include "abstractuserverifier.h"
AbstractUserVerifier::VerifyState AbstractUserVerifier::state() const
{
	return _state;
}
void AbstractUserVerifier::setState(VerifyState other)
{
	if (_state == other)
		return;
	_state = other;
	emit stateChanged();
}
AbstractUserVerifier::AbstractUserVerifier(QObject* parent)
	:QObject(parent){}
