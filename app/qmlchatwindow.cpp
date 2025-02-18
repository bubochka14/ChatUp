#include "qmlchatwindow.h"
QmlChatWindow::QmlChatWindow(QQmlEngine* eng, std::shared_ptr<ControllerManager> manager,
	QObject* parent)
	:AbstractChatWindow(parent)
	,_hasError(false)
	,_engine(eng)
	,_manager(manager)
{
}
QFuture<void> QmlChatWindow::initialize()
{
	auto promise = std::make_unique<QPromise<void>>();
	auto future = promise->future();
	UserControllerWrapper::singletonInstance    = _manager->userController();
	CallControllerWrapper::singletonInstance    = _manager->callController();
	GroupControllerWrapper::singletonInstance   = _manager->groupController();
	MessageControllerWrapper::singletonInstance = _manager->messageController();
	CameraPipelineWrapper::singletonInstance    = new CameraPipeline();
	_manager->userController()->get().then([this]
		(User::Handle* handle) 
		{
			CurrentUserWrapper::singletonInstance = handle;
		}).then(this,[this, promise = std::move(promise)]() {
			QQmlComponent comp(_engine);
			comp.loadFromModule("app", "ChatWindow");
			QObject* windowObj = comp.createWithInitialProperties({
				{"manager",QVariant::fromValue(_manager.get())},
				{"visible",false} }
				);
			if (!windowObj)
			{
				_error = comp.errorString();
				_hasError = true;
				return;
			}
			_window = qobject_cast<QQuickWindow*>(windowObj);
			if (!_window)
			{
				_error = "Cannot cast root object of ChatWindow to QQuickWindow";
				_hasError = true;
			}
			promise->finish();
			});
		return future;
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