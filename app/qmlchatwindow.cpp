#include "qmlchatwindow.h"
QmlChatWindow::QmlChatWindow(QQmlEngine* eng,ControllerManager* manager,
	QObject* parent)
	:AbstractChatWindow(parent)
	,_hasError(false)
{
	QQmlComponent comp(eng);
	comp.loadFromModule("app", "ChatWindow");
	QObject* windowObj = comp.createWithInitialProperties({ 
		{"manager",QVariant::fromValue(manager)},
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
		_error = "Cannot cast root object of ChatWindow to QQuickWindow";
		_hasError = true;
	}
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