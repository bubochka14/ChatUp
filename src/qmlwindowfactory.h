#pragma once
#include "abstractwindowfactory.h"
#include "qmlstartupwindow.h"
#include "qmlchatwindow.h"
#include <qloggingcategory.h>
#include "chatclient_include.h"
#include "QWKQuick/quickwindowagent.h"
#include <quickfuture.h>
#include "qmlobjectconverter.h"
Q_DECLARE_LOGGING_CATEGORY(LC_QML_WINDOW_FACTORY);
class QmlWindowFactory : public AbstractWindowFactory
{
	Q_OBJECT;
public:
	explicit QmlWindowFactory(QObject* parent = nullptr);
	StartupWindow* createStartupWindow() override;
	AbstractChatWindow* createChatWindow(RoomController*, UserController*, MessageController*) override;
private:
	QQmlEngine* _engine;
};
