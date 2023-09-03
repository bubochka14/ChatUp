#pragma once 
#include <QObject>
#include <qqmlapplicationengine.h>
#include <qquickwindow.h>
#include <QQuickitem>
#include <QQuickview.h>
#include <qqmlcomponent.h>
#include <qqmlincubator.h>
class QuickWindow : public QQuickWindow
{
	Q_OBJECT;
protected:
	QSharedPointer<QObject> _content;

};