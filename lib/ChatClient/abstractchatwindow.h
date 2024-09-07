#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <qqmlapplicationengine.h>
#include <QLoggingCategory>
#include <qquickwindow.h>
#include <QQuickitem>
#include <QQuickview.h>
#include <qqmlcomponent.h>
#include <qdir.h>
#include <qqmlcontext.h>
#include "chatcontroller.h"
#include "applicationsettings.h"
#include "chatclient_include.h"
class CHAT_CLIENT_EXPORT AbstractChatWindow : public QObject
{
	Q_OBJECT;
	Q_PROPERTY(AbstractChatController* controller READ controller WRITE setController NOTIFY controllerChanged);
public:
	ApplicationSettings* settings() const;
	void setController(AbstractChatController* controller);
	AbstractChatController* controller() const;
public slots:
	virtual void show() = 0;
	virtual void hide() = 0;
signals:
	void controllerChanged();
	void chatMessageSent(int id, const QString& mess);
protected:
	explicit AbstractChatWindow(ApplicationSettings* settings, QObject* parent = nullptr);
private:
	AbstractChatController* _controller;
	ApplicationSettings* _settings;
};