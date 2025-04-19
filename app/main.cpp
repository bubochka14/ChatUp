#include <QGuiApplication>
#include <QCommandLineParser>
#include "qtranslator.h"
#include <qdiriterator.h>
#include "app.h"
#include <QUrl>

int main(int argc, char ** argv)
{
	using namespace Qt::Literals::StringLiterals;
	QGuiApplication gApp(argc, argv);
    QGuiApplication::setApplicationVersion(APP_VERSION);
    QCommandLineParser parser;
    parser.addVersionOption();
    //A boolean options with multiple names, e.g. (-p, --port);
    QCommandLineOption portOption(QStringList() << "p" << "port",
        QCoreApplication::translate("main", "Property to specify host port, default value is 9000"),"portOption", "8000");
    parser.addOption(portOption);
    QCommandLineOption addrOption(QStringList() << "h" << "host",
        QCoreApplication::translate("main", "Property to specify host address, default value is 5.35.124.43"),
        "hostOption", "5.35.124.43");
    parser.addOption(addrOption);
    parser.process(gApp);

    QString host = parser.value(addrOption);
    int port = parser.value(portOption).toInt();
    App client(host.toStdString(), port);
    if (client.run())
        return gApp.exec();
    return -1;
}
