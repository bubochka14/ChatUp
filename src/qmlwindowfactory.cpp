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
     // Make sure alpha channel is requested, our special effects on Windows depends on it.
    QQuickWindow::setDefaultAlphaBuffer(true);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        const bool curveRenderingAvailable = true;
#else
        const bool curveRenderingAvailable = false;
#endif
    _engine->rootContext()->setContextProperty(QStringLiteral("$curveRenderingAvailable"), QVariant(curveRenderingAvailable));
    qmlRegisterType<QWK::QuickWindowAgent>("QWindowKit", 1, 0, "WindowAgent");
    qmlRegisterModule("QWindowKit", 1, 0);
	QuickFuture::registerType<UserInfo*>();
	QuickFuture::registerType<RoomModel*>();
	QuickFuture::registerType<MessageModel*>();


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
AbstractChatWindow* QmlWindowFactory::createChatWindow(AbstractChatController* controller)
{
	auto window = new QmlChatWindow(_engine,controller, QUrl("qrc:/resources/ChatWindow.qml"));
	if (window->hasError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create chat window: " << window->errorString();
		return nullptr;
	}
	return window;
}
