#pragma once
#include "abstractchatwindow.h"
#include <QQuickWindow>
#include <qurl.h>
#include <qqmlengine.h>

class QmlChatWindow : public AbstractChatWindow
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
public:
	explicit QmlChatWindow(QQmlEngine* eng,AbstractChatController* controller,const QUrl&, ApplicationSettings* settings, QObject* parent = nullptr);
	bool hasError() const;
	QString errorString() const; 
public slots:
	void show() override;
	void hide() override;
private:
	bool _hasError;
	QString _error;
	AbstractChatController* _controller;
	QQuickWindow* _window;
};