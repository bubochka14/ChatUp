#pragma once
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <qtranslator.h>
#include "applicationsettings.h"
#include "wsnetworkfactory.h"
#include "qmlwindowfactory.h"
#include "qffuture.h"
#include <QWKQuick/qwkquickglobal.h>
#include "rtc/websocket.hpp"
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class App : public QObject
{
public:
	explicit App(std::string host,int port);
	explicit App(std::shared_ptr<NetworkCoordinator> netFact,
		std::shared_ptr<AbstractWindowFactory> windowFactory);
	int run();
public slots:
	void setAppLanguage(const QString& lan = QString());
	void logout(const QString& reason="");
protected slots:
	void handleRegistration(const QString& login, const QString& pass);
	void handleLogin(const QString& login, const QString& pass);
private:
	AbstractChatWindow* createChatWindow();
	StartupWindow* startup;
	AbstractChatWindow* chat;
	std::shared_ptr<NetworkCoordinator> _network;
	std::shared_ptr<AbstractWindowFactory>_windowFactory;
	std::shared_ptr<ControllerManager> _controllerManager;
};
