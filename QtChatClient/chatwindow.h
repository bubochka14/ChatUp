#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <qqmlapplicationengine.h>
#include <QLoggingCategory>
#include <qquickwindow.h>
#include <QQuickitem>
#include <QQuickview.h>
#include <qqmlcomponent.h>
#include <qqmlincubator.h>
#include <qqmlcontext.h>
class ChatWindow : public QQuickWindow
{
	Q_OBJECT;
	QSharedPointer<QObject> _content;
	QQmlContext*            _context;
public:
	explicit ChatWindow(QAbstractListModel* roomModel,QQmlEngine* engine = nullptr, QWindow* parent = nullptr);
public slots:
	//void setRoomModel(QAbstractListModel * model);

signals:
	void chatMessage(const QString& message, ulong roomID);
private slots:
	void _proxySlot(const QString& message, int roomID);
};