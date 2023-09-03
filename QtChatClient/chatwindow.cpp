#include "chatwindow.h"
ChatWindow::ChatWindow(QAbstractListModel* roomModel, QQmlEngine* engine, QWindow* parent)
	:QQuickWindow(parent)
{
	QQmlEngine* mEngine = engine;
	if (!mEngine)
		mEngine = new QQmlEngine(this);
	mEngine->rootContext()->setContextProperty("roomModel", roomModel);
	QQmlComponent component(mEngine, (QUrl("qrc:/qml/MainWindow.qml")));
	_content.reset(component.create());
	if (_content.isNull())
		qCritical() << "Unable to load MainWindow: " << component.errorString();
	qobject_cast<QQuickItem*>(_content.get())->setParentItem(contentItem());
	connect(_content.get(), SIGNAL(messageSent(QString, int)), this, SLOT(_proxySlot(QString, int)));

	setWidth(960);
	setHeight(560);
}

void  ChatWindow::_proxySlot(const QString& message, int roomID)
{
	emit chatMessage(message, roomID);
}