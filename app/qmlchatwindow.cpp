#include "qmlchatwindow.h"
Q_LOGGING_CATEGORY(LC_QML_CHAT_WINDOW, "QmlChatWindow");
QmlChatWindow::QmlChatWindow(QQmlEngine* eng, std::shared_ptr<ControllerManager> manager,
	QObject* parent)
	:AbstractChatWindow(parent)
	,_hasError(false)
	,_window(nullptr)
	,_manager(manager)
	,_initPromise(nullptr)
	,_comp(eng)
{
	_creationChecker.setInterval(1000);
	_inc.setInitialProperties({
			{"visible",false} 
		});
	connect(&_creationChecker, &QTimer::timeout, this, &QmlChatWindow::finalizeCreation);
	connect(&_comp, &QQmlComponent::statusChanged, this, [this](QQmlComponent::Status st)
		{
			if (st == QQmlComponent::Ready)
			{
				qCDebug(LC_QML_CHAT_WINDOW) << "Component loaded";
				//when component loaded and current user received
				_creationChecker.start();
				_comp.create(_inc);
				
			}
			else if (st == QQmlComponent::Error)
			{
				_creationChecker.stop();
				qCCritical(LC_QML_CHAT_WINDOW) << "QQmlComponent creation error:" << _comp.errorString();
				_initPromise->setException(std::make_exception_ptr(StandardError(0, "QQmlComponent creation error:" + _comp.errorString())));
				_initPromise.reset();
			}
		});
}
void QmlChatWindow::finalizeCreation() {
	if (!_inc.isReady()) {
		return;
	}
	QObject* windowObj = _inc.object();
	if (!windowObj) {
		_error = _comp.errorString();
		_hasError = true;
		return;
	}
	_window = qobject_cast<QQuickWindow*>(windowObj);
	if (!_window) {
		_error = "Cannot cast root object of ChatWindow to QQuickWindow";
		_hasError = true;
	}
	_creationChecker.stop();
	connect(_window, SIGNAL(logout()), this, SIGNAL(logout()));
	_initPromise->finish();
	_initPromise.reset();
}
QFuture<void> QmlChatWindow::initialize()
{
	//if initialization already started
	if (_initPromise)
		return _initPromise->future();
	_initPromise = std::make_unique<QPromise<void>>();
	_initPromise->setProgressRange(0, 1);
	_initPromise->start();
	//receiving current user
	_manager->userController()->get().then(this,[this](User::Handle* handle) {
		//setup wrappers
		if (!CurrentUserWrapper::singletonInstance)
			CurrentUserWrapper::singletonInstance= handle;
		else
			CurrentUserWrapper::singletonInstance->copy(handle);

		UserControllerWrapper::singletonInstance			= _manager->userController();
		CallControllerWrapper::singletonInstance			= _manager->callController();
		GroupControllerWrapper::singletonInstance			= _manager->groupController();
		MessageControllerWrapper::singletonInstance			= _manager->messageController();

		if(!CameraPipelineWrapper::singletonInstance)
			CameraPipelineWrapper::singletonInstance		= new Media::Video::CameraPipeline();

		if(!MicrophonePipelineWrapper::singletonInstance)
			MicrophonePipelineWrapper::singletonInstance	= new Media::Audio::MicrophonePipeline;
		qCDebug(LC_QML_CHAT_WINDOW) << "Current user:" << CurrentUserWrapper::singletonInstance->id() << "received";
			//loading component from app module
			_comp.loadFromModule("app", "ChatWindow", QQmlComponent::Asynchronous);
		}).onFailed([this]() {
			qCCritical(LC_QML_CHAT_WINDOW) << "Cannot receive current user info";
			_initPromise->setException(std::make_exception_ptr(StandardError(0, "Cannot receive current user info")));
			_initPromise.reset();
		});
		return _initPromise->future();
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
QmlChatWindow::~QmlChatWindow()
{
	if (_window)
	{
		_window->deleteLater();
		_window = nullptr;
	}
}