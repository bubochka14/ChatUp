#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <qeventloop.h>
#include "userverifydialog.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfuture.h>

#include "chatroommodel.h"
#include "chatwindow.h"
#include <QQmlContext>
#define AUTH_CONNECTION_SEC 5
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class ChatClient : public QObject
{
	Q_OBJECT;
	QQmlApplicationEngine _qmlEngine;
	QSharedPointer<ChatWindow> _window;
	QSharedPointer<WSClient> _WSClient;
	ChatRoomModel _model;
	//QString   _userToken;
	//QUrl      _hostUrl;
public:
	explicit ChatClient(QObject* parent = nullptr);
	void run(const QUrl&);
	//void setHostUrl(const QUrl& other);
	//QUrl hostUrl() const;
signals:
	void hostUrlChanged();
protected:
	virtual bool setupWSClient(const QUrl&);
	virtual QString authenticateUser();

};