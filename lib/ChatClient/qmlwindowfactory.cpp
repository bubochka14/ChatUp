#include "qmlwindowfactory.h"
Q_LOGGING_CATEGORY(LC_QML_WINDOW_FACTORY, "QmlWindowFactory");
QmlWindowFactory::QmlWindowFactory(ApplicationSettings* settings, QQmlEngine*e, QObject* parent)
	:_engine(e)
	,_settings(settings)
{}
StartupWindow* QmlWindowFactory::createStartupWindow()
{
	auto window = new QmlStartupWindow(_engine, QUrl("qrc:qt/qml/ChatClient/StartupWindow.qml"));
	if (window->hasCreationError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create startup window: " << window->errorString();
		return nullptr;
	}
	return window;
}
AbstractChatWindow* QmlWindowFactory::createChatWindow(AbstractChatController* controller)
{
	auto window = new QmlChatWindow(_engine,controller, QUrl("qrc:qt/qml/ChatClient/ChatWindow.qml"),_settings);
	if (window->hasError())
	{
		qCCritical(LC_QML_WINDOW_FACTORY) << "Cannot create chat window: " << window->errorString();
		return nullptr;
	}
	return window;
}