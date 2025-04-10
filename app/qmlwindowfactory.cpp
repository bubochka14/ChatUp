#include "qmlwindowfactory.h"
Q_LOGGING_CATEGORY(LC_QML_WINDOW_FACTORY, "QmlWindowFactory");
QmlWindowFactory::QmlWindowFactory(QObject* parent)
	:AbstractWindowFactory(parent)
	, _engine(new QQmlApplicationEngine)
{
	qputenv("QML_XHR_ALLOW_FILE_READ", QString("1").toUtf8());

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        //QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        //Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
     //Alpha channel request for window agent features
    QQuickWindow::setDefaultAlphaBuffer(true);
	// Window agent registration
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
	const bool curveRenderingAvailable = true;
#else
	const bool curveRenderingAvailable = false;
#endif

	_engine->rootContext()->setContextProperty(QStringLiteral("$curveRenderingAvailable"), QVariant(curveRenderingAvailable));
    qmlRegisterType<QWK::QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent");
    qmlRegisterModule("QWindowKit", 1, 0);
	// Registering types available via Future in QML
	//QuickFuture::registerType<UserInfo*>();
	QuickFuture::registerType<Group::Model*>();
	QuickFuture::registerType<int>();
	QuickFuture::registerType<void>();
	QuickFuture::registerType<Message::Model*>();
	QuickFuture::registerType<User::Model*>();
	QuickFuture::registerType<User::Handle*>();
	QuickFuture::registerType<Participate::Model*>();

	qmlRegisterSingletonType<QMLObjectConverter>("ObjectConverter", 1, 0, "ObjectConverter",
		[](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
		Q_UNUSED(engine);
		Q_UNUSED(scriptEngine);
		return new QMLObjectConverter();
	});

}
StartupWindow* QmlWindowFactory::createStartupWindow()
{
	auto window = new QmlStartupWindow(_engine);
	if (window->hasCreationError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create startup window: " << window->errorString();
		return nullptr;
	}
	return window;
}
AbstractChatWindow* QmlWindowFactory::createChatWindow(std::shared_ptr<ControllerManager> manager)
{
	auto window = new QmlChatWindow(_engine, manager);
	if (window->hasError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create chat window: " << window->errorString();
		return nullptr;
	}
	return window;
}
