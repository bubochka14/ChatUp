// QtChatClient.cpp: определяет точку входа для приложения.
//

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "chatclient.h"
using namespace std;

int main(int argc, char ** argv)
{
	using namespace Qt::Literals::StringLiterals;

	QGuiApplication gapp(argc, argv);
	QQmlApplicationEngine engine(":/MainWindow.qml");
	QUrl url;
	url.setScheme(u"ws"_s);
	url.setHost("localhost");
	url.setPort(7071);
	ChatClient client(url, true);
	QObject::connect(engine.rootObjects().first(), SIGNAL(messageSent(QString)),
		&client, SLOT(sendMessage(QString)));
	return gapp.exec();
}
