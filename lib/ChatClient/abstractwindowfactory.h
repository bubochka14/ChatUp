#pragma once 
#include <qobject.h>
#include "startupwindow.h"
#include "applicationsettings.h"
#include "abstractchatwindow.h"
#include "chatcontroller.h"
#include "chatclient_include.h"

class CHAT_CLIENT_EXPORT AbstractWindowFactory : public QObject
{
	Q_OBJECT;
public:
	virtual ~AbstractWindowFactory() = default;
	virtual StartupWindow* createStartupWindow() =0;
	virtual AbstractChatWindow* createChatWindow(AbstractChatController* controller) =0;
protected:
	explicit AbstractWindowFactory(QObject* parent = nullptr);

};