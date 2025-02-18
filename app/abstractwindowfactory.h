#pragma once 
#include <qobject.h>
#include "startupwindow.h"
#include "abstractchatwindow.h"
#include "controllers/controllermanager.h"
class AbstractWindowFactory : public QObject
{
	Q_OBJECT;
public:
	virtual ~AbstractWindowFactory() = default;
	virtual StartupWindow* createStartupWindow() =0;
	virtual AbstractChatWindow* createChatWindow(std::shared_ptr<ControllerManager> manager) = 0;
protected:
	explicit AbstractWindowFactory(QObject* parent = nullptr);

};
