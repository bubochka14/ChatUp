#pragma once
#include "abstractwindowfactory.h"
#include "qmlstartupwindow.h"
#include "qmlchatwindow.h"
#include <qloggingcategory.h>
#include "chatclient_include.h"

Q_DECLARE_LOGGING_CATEGORY(LC_QML_WINDOW_FACTORY);
class CHAT_CLIENT_EXPORT QmlWindowFactory : public AbstractWindowFactory
{
	Q_OBJECT;
public:
	explicit QmlWindowFactory(ApplicationSettings* settings, QQmlEngine*, QObject* parent = nullptr);
	StartupWindow* createStartupWindow() override;
	AbstractChatWindow* createChatWindow(AbstractChatController* controller) override;
private:
	QQmlEngine* _engine;
	ApplicationSettings* _settings;
};