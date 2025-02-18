#pragma once
#include <QObject>
#include <qfuture.h>
#include <controllers/controllermanager.h>
class AbstractChatWindow : public QObject
{
	Q_OBJECT;
public:
	virtual QFuture<void> initialize();
public slots:
	virtual void show() = 0;
	virtual void hide() = 0;
signals:
	void logout();
	void closed();
protected:
	explicit AbstractChatWindow(QObject* parent = nullptr);
};
