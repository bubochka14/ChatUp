#pragma once
#include "network_include.h"
#include <qobject.h>
#include "usercontroller.h"
#include "callcontroller.h"
#include "messagecontroller.h"
#include <qqmlengine.h>
#include "groupcontroller.h"
class CC_NETWORK_EXPORT ControllerManager : public QObject
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
	Q_PROPERTY(Group::Controller* groupController READ groupController);
	Q_PROPERTY(Message::Controller* messageController READ messageController);
	Q_PROPERTY(User::Controller* userController READ userController);
	Q_PROPERTY(Call::Controller* callController READ callController);
public:
	virtual Group::Controller* groupController() = 0;
	virtual Message::Controller* messageController() = 0;
	virtual User::Controller* userController() = 0;
	virtual Call::Controller* callController() = 0;
	virtual QFuture<void> initializeAll() { return QtFuture::makeReadyVoidFuture(); };
protected:
	explicit ControllerManager(QObject* parent = nullptr);
};
class CC_NETWORK_EXPORT CallerControllerManager : public ControllerManager
{
public:
	explicit CallerControllerManager(std::shared_ptr<NetworkCoordinator> manager,
		QObject* parent = nullptr
	);
	Group::Controller* groupController() override;
	Message::Controller* messageController() override;
	User::Controller* userController() override;
	Call::Controller* callController() override;
	QFuture<void> initializeAll() override;

private:
	Group::Controller* group;
	Message::Controller* message;
	User::Controller* user;
	Call::Controller* call;
	std::optional<QString> _lastError;
};