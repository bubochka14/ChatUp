#pragma once
#include <qobject.h>
#include <qfuture.h>
#include "network_include.h"
class CC_NETWORK_EXPORT AbstractController : public QObject
{
	Q_OBJECT;
public:
	virtual QFuture<void> initialize() { return QtFuture::makeReadyVoidFuture(); };
	virtual void reset(){}
protected:
	AbstractController(QObject* parent = nullptr) : QObject(parent) {};

};