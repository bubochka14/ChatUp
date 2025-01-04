#pragma once
#include <qobject.h>
#include <qfuture.h>
class  AbstractController : public QObject
{
	Q_OBJECT;
public:
	virtual QFuture<void> initialize() { return QtFuture::makeReadyVoidFuture(); };
	virtual void reset(){}
protected:
	AbstractController(QObject* parent = nullptr) : QObject(parent) {};

};