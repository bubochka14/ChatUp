#pragma once
#include "abstractchatwindow.h"
#include <QQuickWindow>
#include <qurl.h>
#include <qqmlengine.h>
#include "chatcontroller.h"
class QmlChatWindow : public AbstractChatWindow
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
public:
	explicit QmlChatWindow(
		QQmlEngine* eng,
		RoomController*,
		MessageController*,
		UserController*,
		const QUrl&, 
		QObject* parent = nullptr
	);
	bool hasError() const;
	QString errorString() const; 
public slots:
	void show() override;
	void hide() override;
private slots:
	void handleLogout();
private:
	bool _hasError;
	QString _error;
	QQuickWindow* _window;
};
