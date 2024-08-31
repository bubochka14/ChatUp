#include "applicationFactory.h"
ApplicationFactory::ApplicationFactory(QObject* parent)
	:QObject(parent)
{
}

WSApplicationFactory::WSApplicationFactory(const QString& host,int port,QObject* parent)
	:ApplicationFactory(parent)
	,_ws(new WSClient(QWebSocketProtocol::VersionLatest))
	,_engine(new QQmlApplicationEngine(parent))
	
{
	_caller = new WSServerMethodCaller(_ws,host, port,parent);
}
AuthenticationMaster* WSApplicationFactory::createAuthenticationMaster()
{
	return new CallerAuthentificationMaster(_caller);
}
ApplicationSettings* WSApplicationFactory::createApplicationSettings()
{
	return new ApplicationSettings("ChatClient", "Org");
}
AbstractWindowFactory* WSApplicationFactory::createWindowFactory(ApplicationSettings* settings)
{
	return new QmlWindowFactory(settings,_engine,parent());
}
AbstractChatController* WSApplicationFactory::createChatController()
{
	return new CallerChatController(_caller,parent());
}