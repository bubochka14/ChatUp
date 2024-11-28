#include "startupwindow.h"
StartupWindow::State StartupWindow::state() const
{
	return _state;
}
void StartupWindow::setState(State other)
{
	if (_state == other)
		return;
	_state = other;
	emit stateChanged();
}
StartupWindow::StartupWindow(QObject* parent)
	:QObject(parent)
	,_state(Inital)
	,_progress(0)
{}
float StartupWindow::loadingProgress() const
{
	return _progress;
}
void StartupWindow::clear()
{
	setLoadingProgress(0);
	setState(Inital);
	setStatus("");
	setErrorString("");
};


void StartupWindow::setLoadingProgress(float other)
{
	if (other == _progress)
		return;
	_progress = other;
	emit loadingProgressChanged();
}
QString StartupWindow::status()const
{
	return _status;
}
void StartupWindow::setStatus(const QString& other)
{
	if (other == _status)
		return;
	_status = other;
	emit statusChanged();
}
QString StartupWindow::errorString() const
{
	return _error;
}
void StartupWindow::setErrorString(const QString& other)
{
	if (other == _error)
		return;
	_error = other;
	emit errorStringChanged();
}