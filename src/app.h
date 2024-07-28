#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <qeventloop.h>
#include "userverifydialog.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <qfuture.h>
#include <qtranslator.h>
#include "chatroommodel.h"
#include "chatwindow.h"
#include "applicationsettings.h"
#include <QQmlContext>
#include <QGuiApplication>
Q_DECLARE_LOGGING_CATEGORY(LC_ChatClient)
class ChatClient : public QObject
{
	Q_OBJECT;
	QQmlEngine* _qmlEngine;
	QSharedPointer<ChatWindow> _window;
	QSharedPointer <UserVerifyDialog> _dialog;
	QSharedPointer<WSClient> _WSClient;
	QTranslator* _currentTranslator;
	QHash<QString, QTranslator*> _translators;
	ChatRoomModel _model;
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