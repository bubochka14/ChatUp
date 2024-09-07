#include "qmlchatwindow.h"
QmlChatWindow::QmlChatWindow(QQmlEngine* eng, AbstractChatController* controller, const QUrl& url,
	ApplicationSettings* settings, QObject* parent)
	:AbstractChatWindow(settings,parent)
	,_hasError(false)
	,_controller(controller)
{
	QQmlComponent comp(eng,url);
	_window = qobject_cast<QQuickWindow*>(comp.createWithInitialProperties({ {"controller",QVariant::fromValue(_controller)} , {"visible",false} }));
	if (!_window)
	{
		_error =  comp.errorString();
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
	_window->show();
}
void QmlChatWindow::hide()
{
	_window->hide();
}