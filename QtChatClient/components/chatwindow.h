#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <qqmlapplicationengine.h>
#include <QLoggingCategory>
#include <qquickwindow.h>
#include <QQuickitem>
#include "components_global.h"
#include <qqmlcomponent.h>
#include <qqmlincubator.h>
#include <qdir.h>
#include <qqmlcontext.h>
class COMP_EXPORT ChatWindow : public QQuickWindow
{
	Q_OBJECT;
	QSharedPointer<QObject> _content;
	QQmlContext*            _context;
public:
	explicit ChatWindow(QQmlEngine* engine, QWindow* parent = nullptr);
public slots:
	//void setRoomModel(QAbstractListModel * model);

signals:
	void chatMessage(const QString& message, int roomID);
	void languageChanged(const QString& message);
private slots:
	void _proxySlot(const QString& message, int roomID);
};