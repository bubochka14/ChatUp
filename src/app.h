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
#include "applicationfactory.h"
#include "chatcontroller.h"
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class ChatClient : public QObject
{
	Q_OBJECT;
	AuthenticationMaster* _authMaster;
	ApplicationFactory* _appFactory;
	AbstractWindowFactory* _windowFactory;
	AbstractChatController* _chatController;
	ApplicationSettings* _settings;
	ClientMethodDispatcher* _dispatcher;
public:
	explicit ChatClient(const QString& host,int port,QObject* parent = nullptr);
	explicit ChatClient(ApplicationFactory* factory, QObject* parent = nullptr);
	int run();
public slots:
	void setAppLanguage(const QString& lan = QString());

};