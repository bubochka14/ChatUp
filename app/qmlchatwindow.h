#pragma once
#include "abstractchatwindow.h"
#include <QQuickWindow>
#include <qurl.h>
#include <qqmlapplicationengine.h>
#include <qqmlcomponent.h>
#include "controllers/controllermanager.h"
class QmlChatWindow : public AbstractChatWindow
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
public:
	explicit QmlChatWindow(
		QQmlEngine* eng,
		ControllerManager* manager,
		QObject* parent = nullptr
	);
	bool hasError() const;
	QString errorString() const; 
public slots:
	void show() override;
	void hide() override;
private slots:
	//void handleLogout();
private:
	bool _hasError;
	QString _error;
	QQuickWindow* _window;
};
