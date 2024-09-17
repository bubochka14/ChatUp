// QtWSClient.cpp: определяет точку входа для приложения.
//

#include <QGuiApplication>
#include <QCommandlineparser>
#include <QProcessEnvironment>
#include "qtranslator.h"
#include <qdiriterator.h>
#include "app.h"
#include <QUrl>
#ifdef Q_OS_WIN
    // Indicates to hybrid graphics systems to prefer the discrete part by default.
extern "C" {
    Q_DECL_EXPORT unsigned long NvOptimusEnablement = 0x00000001;
    Q_DECL_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
int main(int argc, char ** argv)
{
	using namespace Qt::Literals::StringLiterals;

	QGuiApplication gApp(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);
    QCommandLineParser parser;
    parser.addVersionOption();
    //A boolean options with multiple names, e.g. (-p, --port);
    QCommandLineOption portOption(QStringList() << "p" << "port",
        QCoreApplication::translate("main", "Property to specify host port, default value is 7071"),QString(),"8000");
    parser.addOption(portOption);

    QCommandLineOption addrOption(QStringList() << "h" << "host",
        QCoreApplication::translate("main", "Property to specify host address, default value is 127.0.0.1"),
        QString(), "127.0.0.1");
    parser.addOption(addrOption);
    parser.process(gApp);

    QString host = parser.value(addrOption);
    int port = parser.value(portOption).toInt();
    qDebug() << "host: " << host << ", port: " << port;
    QUrl url;
    url.setPort(port);
    url.setScheme(u"ws"_s);
    url.setHost(host);
    App client(host,port);
    if (client.run())
        return gApp.exec();
}
