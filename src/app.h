#pragma once
#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <qtranslator.h>
#include "servermethodcaller.h"
#include "RoomModel.h"
#include "wsclient.h"
#include "applicationsettings.h"
#include "qmlwindowfactory.h"
#include "clientmethoddispatcher.h"
#include "networkfactory.h"
#include "qffuture.h"
#include <QWKQuick/qwkquickglobal.h>

Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class App : public QObject
{
	Q_OBJECT;
	AuthenticationMaster* _authMaster;
	NetworkFactory* _netFactory;
	AbstractWindowFactory* _windowFactory;
	ChatControllerFactory* _controllerFactory;

public:
	explicit App(const QString& host,int port,QObject* parent = nullptr);
	explicit App(NetworkFactory* netFact,AbstractWindowFactory* windowFactory, QObject* parent = nullptr);
	int run();
public slots:
	void setAppLanguage(const QString& lan = QString());

};