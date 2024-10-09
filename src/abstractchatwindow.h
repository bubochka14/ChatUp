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

class AbstractChatWindow : public QObject
{
	Q_OBJECT;

public slots:
	virtual void show() = 0;
	virtual void hide() = 0;
protected:
	explicit AbstractChatWindow(QObject* parent = nullptr);
};