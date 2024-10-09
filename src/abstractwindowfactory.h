#pragma once 
#include <qobject.h>
#include "startupwindow.h"
#include "applicationsettings.h"
#include "abstractchatwindow.h"
#include "chatcontroller.h"
#include "chatclient_include.h"

class AbstractWindowFactory : public QObject
{
	Q_OBJECT;
public:
	virtual ~AbstractWindowFactory() = default;
	virtual StartupWindow* createStartupWindow() =0;
	virtual AbstractChatWindow* createChatWindow(RoomController*, UserController*,MessageController*) =0;
protected:
	explicit AbstractWindowFactory(QObject* parent = nullptr);

};