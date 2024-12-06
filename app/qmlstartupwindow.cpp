#include "qmlstartupwindow.h"
QmlStartupWindow::QmlStartupWindow(QQmlEngine* eng, QObject* parent)
	:StartupWindow(parent)
	, _hasError(false)
{
	QQmlComponent comp(eng);
	comp.loadFromModule("app", "StartupWindow");
	auto obj = comp.create();
	_window = qobject_cast<QQuickWindow*>(obj);
	if (_window)
	{
		connect(_window, SIGNAL(loginUser(const QString&, const QString&)), this, SLOT(proxyLoginPassed(const QString&, const QString&)));
		connect(_window, SIGNAL(registerUser(const QString&, const QString&)), this, SLOT(proxyRegisterPassed(const QString&, const QString&)));
		connect(this, &StartupWindow::stateChanged, this, [=]() {
			if (state() == Loading) {
				_window->setProperty("state", "loading");
			}
			else
				_window->setProperty("state", "inital");

			});
		connect(this, &StartupWindow::loadingProgressChanged, this, [=]() {
				_window->setProperty("loadingProgress", loadingProgress());
			});
		connect(this, &StartupWindow::statusChanged, this, [=]() {
			_window->setProperty("loadingStatus", status());
			});
		connect(this, &StartupWindow::errorStringChanged, this, [=]() {
			_window->setProperty("errorString", errorString());
			});
	}else
	{
		_error = comp.errorString();
		_hasError = true;
	}
}
bool QmlStartupWindow::hasCreationError() const
{
	return _hasError;
}
QString QmlStartupWindow::creationError() const
{
	return _error;
}
void QmlStartupWindow::show()
{
	if(_window)
	_window->show();
}
void QmlStartupWindow::hide()
{
	if (_window)
	_window->hide();
}
void QmlStartupWindow::proxyRegisterPassed(const QString& user, const QString& password)
{
	emit StartupWindow::registerPassed(user, password);
}
void QmlStartupWindow::proxyLoginPassed(const QString& user, const QString& password)
{
	emit StartupWindow::loginPassed(user, password);

}