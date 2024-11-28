#pragma once
#include <QObject>
class AbstractChatWindow : public QObject
{
	Q_OBJECT;

public slots:
	virtual void show() = 0;
	virtual void hide() = 0;
signals:
	void logout();
	void closed();
protected:
	explicit AbstractChatWindow(QObject* parent = nullptr);
};
