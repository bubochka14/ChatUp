#include "chatwindow.h"
ChatWindow::ChatWindow(QQmlEngine* engine, QWindow* parent)
	:QQuickWindow(parent)
	,_content(nullptr)
{
	QQmlEngine* mEngine = engine;
	/*if (!mEngine)
		mEngine = new QQmlEngine(this)*/;

	QQmlComponent component(mEngine);
	component.loadUrl(QUrl("qrc:/qt/qml/ChatClient/MainWindow.qml"));
	//QQmlComponent component(mEngine, (":/components/MainWindow.qml"));
	_content.reset(component.create());
	if (!_content)
	{
		qCritical() << "Unable to load MainWindow: " << component.errorString();

		qApp->quit();
	}
	else
	{
		connect(_content.get(), SIGNAL(messageSent(QString, int)), this, SLOT(_proxySlot(QString, int)));
		connect(_content.get(), SIGNAL(languageSet(QString)), this, SIGNAL(languageChanged(QString)));
	}

}

void  ChatWindow::_proxySlot(const QString& message, int roomID)
{
	emit chatMessage(message, roomID);
}