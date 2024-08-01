#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <qeventloop.h>
#include "userverifierdialog.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfuture.h>
#include <qtranslator.h>
#include "chatroommodel.h"
#include "chatwindow.h"
#include "authenticationmaster.h"
#include "wsclient.h"
#include "applicationsettings.h"
#include <QQmlContext>
#include <QGuiApplication>
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class ChatClient : public QObject
{
	Q_OBJECT;
	QQmlEngine* _qmlEngine;
	ChatWindow* _window;
	UserVerifierDialog* _dialog;
	WSClient _wsClient;
	QTranslator* _currentTranslator;
	QHash<QString, QTranslator*> _translators;
	ChatRoomModel _model;
	AuthenticationMaster* _authMaster;
	//QString   _userToken;
	//QUrl      _hostUrl;
public:
	explicit ChatClient(QObject* parent = nullptr);
	void run(const QUrl&);
	//void setHostUrl(const QUrl& other);
	//QUrl hostUrl() const;
public slots:
	void setAppLanguage(const QString& lan = QString());
signals:
	void hostUrlChanged();
protected:
	virtual bool setupWSClient(const QUrl&);
	virtual QString authenticateUser();

};