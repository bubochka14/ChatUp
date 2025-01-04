#pragma once
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <qtranslator.h>
#include "applicationsettings.h"
#include "wsnetworkmanager.h"
#include "qmlwindowfactory.h"
#include "networkmanager.h"
#include "qffuture.h"
#include <QWKQuick/qwkquickglobal.h>
#include "rtc/websocket.hpp"
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class App : public QObject
{
	Q_OBJECT;
	NetworkManager* _manager;
	AbstractWindowFactory* _windowFactory;
	ControllerManager* _controllerManager;

public:
	explicit App(const QString& host,int port,QObject* parent = nullptr);
	explicit App(NetworkManager* netFact,AbstractWindowFactory* windowFactory, QObject* parent = nullptr);
	int run();
public slots:
	void setAppLanguage(const QString& lan = QString());
	void logout(const QString& reason="");
private:
	StartupWindow* startup;
	AbstractChatWindow* chat;
	ServerHandler* sh;
};
