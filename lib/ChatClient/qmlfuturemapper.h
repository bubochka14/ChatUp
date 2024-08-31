#pragma once
#include <qobject.h>
#include <qqmlengine.h>
class QmlFutureMapper : public QObject
{
    Q_OBJECT;
    QML_NAMED_ELEMENT(QmlFuture);
    QML_SINGLETON;
};