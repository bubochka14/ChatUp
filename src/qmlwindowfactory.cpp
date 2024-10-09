#include "qmlwindowfactory.h"
Q_LOGGING_CATEGORY(LC_QML_WINDOW_FACTORY, "QmlWindowFactory");
QmlWindowFactory::QmlWindowFactory(QObject* parent)
	:AbstractWindowFactory(parent)
	,_engine(new QQmlEngine(this))
{

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
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
	QuickFuture::registerType<UserInfo*>();
	QuickFuture::registerType<RoomModel*>();
	QuickFuture::registerType<MessageModel*>();
	QuickFuture::registerType<UsersModel*>();

	qmlRegisterSingletonType<QMLObjectConverter>("ObjectConverter", 1, 0, "ObjectConverter", [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
		Q_UNUSED(engine);
		Q_UNUSED(scriptEngine);
		return new QMLObjectConverter();
	});

}
StartupWindow* QmlWindowFactory::createStartupWindow()
{
	auto window = new QmlStartupWindow(_engine, QUrl("qrc:/resources/StartupWindow.qml"));
	if (window->hasCreationError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create startup window: " << window->errorString();
		return nullptr;
	}
	return window;
}
AbstractChatWindow* QmlWindowFactory::createChatWindow(RoomController* room, UserController* user, MessageController* mess)
{
	auto window = new QmlChatWindow(_engine,room,mess,user, QUrl("qrc:/resources/ChatWindow.qml"));
	if (window->hasError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create chat window: " << window->errorString();
		return nullptr;
	}
	return window;
}
