#include "chatwindow.h"
ChatWindow::ChatWindow(QQmlEngine* engine, QWindow* parent)
	:QQuickWindow(parent)
{
	QQmlEngine* mEngine = engine;
	/*if (!mEngine)
		mEngine = new QQmlEngine(this)*/;

	QQmlComponent component(mEngine, (":qt/qml/components/MainWindow.qml"));
	//QQmlComponent component(mEngine, (":/components/MainWindow.qml"));
	_content.reset(component.create());
	if (_content.isNull())
	{
		qCritical() << "Unable to load MainWindow: " << component.errorString();

		qApp->quit();
	}
	qobject_cast<QQuickItem*>(_content.get())->setParentItem(contentItem());
	connect(_content.get(), SIGNAL(messageSent(QString, int)), this, SLOT(_proxySlot(QString, int)));
	connect(_content.get(), SIGNAL(languageSet(QString)), this, SIGNAL(languageChanged(QString)));

	setWidth(960);
	setHeight(560);
}

void  ChatWindow::_proxySlot(const QString& message, int roomID)
{
	emit chatMessage(message, roomID);
}