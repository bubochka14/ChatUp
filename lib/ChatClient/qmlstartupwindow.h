#pragma once
#include "startupwindow.h"
#include <qqmlengine.h>
#include <qquickwindow.h>
#include <qqmlcomponent.h>

class QmlStartupWindow : public StartupWindow
{
	Q_OBJECT;
public:
	QmlStartupWindow(QQmlEngine* eng, const QUrl& url, QObject* parent = nullptr);
	bool hasCreationError() const;
	QString creationError() const;
public slots:
	void show() override;
	void hide() override;
private slots:
	void proxyRegisterPassed(const QString& user, const QString& password);
	void proxyLoginPassed(const QString& user, const QString& password);

private:
	bool _hasError;
	QString _error;
	QQuickWindow* _window;
};