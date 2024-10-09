#pragma once
#include <QObject>
#include <qqmlengine.h>
class QMLObjectConverter : public QObject
{
    Q_OBJECT
public:
    explicit QMLObjectConverter(QObject* parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE QVariantHash toHash(const QJSValue& val)
    {
        QVariant&& var = val.toVariant();
        if (!var.canConvert<QVariantHash>())
            qWarning() << "QMLObjectConverter: cannot convert js value to hash";
        return var.toHash();
    }
};