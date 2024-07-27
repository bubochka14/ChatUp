#pragma once
#include <QObject>
#include <qqmlapplicationengine.h>
#include <QLoggingCategory>
#include <qquickwindow.h>
#include <QQuickitem>
#include "components_global.h"
#include <QQuickview.h>
#include <qqmlcomponent.h>
#include <qqmlincubator.h>
Q_DECLARE_LOGGING_CATEGORY(LC_VERIFY_DIALOG);
//Interlayer between qml AuthorizationDialog and AuthenticationMaster class		
class COMP_EXPORT UserVerifyDialog : public QQuickWindow
{
	Q_OBJECT;
	QSharedPointer<QObject> _content;
public:
	explicit UserVerifyDialog(QQmlEngine* engine = nullptr/*optional*/, QWindow * parent = nullptr);
public slots:
	void setLoadingScreen(bool st);
	void setErrorString(const QString& error);
signals:
	void registerPassed(const QString& user, const QString& password);
	void loginPassed(const QString& user, const QString& password);
};