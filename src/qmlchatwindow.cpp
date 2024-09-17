#include "qmlchatwindow.h"
QmlChatWindow::QmlChatWindow(QQmlEngine* eng, AbstractChatController* controller,
	const QUrl& url, QObject* parent)
	:AbstractChatWindow(parent)
	,_hasError(false)
	,_controller(controller)
{
	QQmlComponent comp(eng,url, QQmlComponent::PreferSynchronous);
	QObject* windowObj = comp.createWithInitialProperties({ {"controller",QVariant::fromValue(_controller)}, {"visible",false} });
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
	connect(this, &AbstractChatWindow::controllerChanged, this, [=]()
		{
			_window->setProperty("controller", QVariant::fromValue(this->controller()));
		});
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
	if(_window)
	_window->show();
}
void QmlChatWindow::hide()
{
	if (_window)
	_window->hide();
}