#include "qmlchatwindow.h"
QmlChatWindow::QmlChatWindow(QQmlEngine* eng, RoomController* room,MessageController* message,UserController* user, 
	const QUrl& url, QObject* parent)
	:AbstractChatWindow(parent)
	,_hasError(false)
{
	QQmlComponent comp(eng,url, QQmlComponent::PreferSynchronous);
	QObject* windowObj = comp.createWithInitialProperties({ 
		{"roomController",QVariant::fromValue(room)}, 
		{"userController",QVariant::fromValue(user)}, 
		{"messageController",QVariant::fromValue(message)}, 
		{"visible",false} }
		);
	if (!windowObj)
	{
		_error =  comp.errorString();
			_hasError = true;
		return;
	}
	_window = qobject_cast<QQuickWindow*>(windowObj);
	if (!_window)
	{
		_error = "Cannot cast root object of "+ url.toString() + " to QQuickWindow";
		_hasError = true;
	}
	connect(_window, SIGNAL(logout()), this, SLOT(handleLogout()));
}
void QmlChatWindow::handleLogout()
{
	emit logout();
}

bool QmlChatWindow::hasError() const
{
	return _hasError;
}
QString QmlChatWindow::errorString() const
{
	return _error;
}
void QmlChatWindow::show()
{
	if(!hasError())
		_window->show();
}
void QmlChatWindow::hide()
{
	if (!hasError())
		_window->hide();
}