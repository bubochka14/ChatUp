// QtWSClient.cpp: определяет точку входа для приложения.
//

#include <QGuiApplication>
#include <QCommandlineparser>
#include <QProcessEnvironment>
#include "chatclient.h"
#include "qtranslator.h"
int main(int argc, char ** argv)
{
	using namespace Qt::Literals::StringLiterals;

	QGuiApplication gApp(argc, argv);
    QGuiApplication::setApplicationName(APP_NAME);
    QGuiApplication::setApplicationVersion(APP_VERSION);
    QCommandLineParser parser;
    parser.addVersionOption();
    
    // A boolean options with multiple names, e.g. (-p, --port);
    QCommandLineOption portOption(QStringList() << "p" << "port",
        QCoreApplication::translate("main", "Property to specify host port, default value is 7071"),QString(),"7071");
    parser.addOption(portOption);

    QCommandLineOption addrOption(QStringList() << "h" << "host",
        QCoreApplication::translate("main", "Property to specify host address, default value is 185.244.51.214"),
        QString(), "185.244.51.214");
    parser.addOption(addrOption);

    parser.process(gApp);


    QString host = parser.value(addrOption);
    int port = parser.value(portOption).toInt();
    qDebug() << "host: " << host << ", port: " << port;
    QUrl url;
    url.setScheme(u"ws"_s);
    url.setHost("185.244.51.214");
    url.setPort(port);
	ChatClient client;
	client.run(url);
	return gApp.exec();
}
