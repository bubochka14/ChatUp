#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")
App::App(const QString& host, int port, QObject* parent)
	:App(new WSNetworkFactory(host, port),new QmlWindowFactory(), parent)

{
}
App::App(NetworkFactory* netFact, AbstractWindowFactory* windowFactory, QObject* parent)
	:QObject(parent)
	,_netFactory(netFact)
	,_windowFactory(windowFactory)
{
	_controllerFactory = _netFactory->createControllerFactory();

	_netFactory->setParent(this);
	_controllerFactory->setParent(this);
	_windowFactory->setParent(this);
	setAppLanguage();
}

int App::run()
{
	roomController = _controllerFactory->createRoomController();
    messageController = _controllerFactory->createMessageController();
	userController = _controllerFactory->createUserController();


	startup = _windowFactory->createStartupWindow();
	chat = _windowFactory->createChatWindow(roomController,userController,messageController);
	_authMaster = _netFactory->createAuthenticationMaster();;
	sh = _netFactory->createHandler();
	if (!startup || !chat)
		return 0;
	startup->setParent(this);
	connect(sh, &ServerHandler::isConnectedChanged, this, [&]() {
		if (!sh->isConnected())
		{
			logout("Server disconnected");
		}
		});
	connect(_authMaster, &AuthenticationMaster::authentificated, this, [&](UserInfo* userInfo) {
		startup->setStatus("Initialization...");
		startup->setLoadingProgress(0.5);
		messageController->initialize(userInfo);
		roomController->initialize(userInfo);
		userController->initialize(userInfo);
		startup->clear();
		chat->show();
		startup->hide();
		});
	connect(_authMaster, &AuthenticationMaster::errorReceived, this, [&](const QString& error) {
		startup->clear();
		startup->setErrorString(error);
		});
	connect(startup, &StartupWindow::registerPassed, this, [&](const QString& login, const  QString& pass) {
			startup->clear();
			startup->setState(StartupWindow::Loading);
			startup->setStatus("Connecting...");
			startup->setLoadingProgress(0.1);
			_authMaster->registerUser(login, pass);
		});
	connect(startup, &StartupWindow::loginPassed, this, [&](const QString& login, const  QString& pass) {
			startup->clear();
			startup->setState(StartupWindow::Loading);
			startup->setStatus("Connecting...");
			startup->setLoadingProgress(0.1);
			_authMaster->loginUser(login, pass);
		});
	connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });
	startup->show();
	return 1;
}
void App::setAppLanguage(const QString& lan)
{
	//QString tmp = lan;
	//ApplicationSettings settings(APP_NAME,ORG_NAME);

	//if (lan.isEmpty())
	//	tmp = settings.language();

	//QTranslator* tr = _translators.value(tmp);
	//if (!tr)
	//{
	//	tr = new QTranslator(this);
	//	if (!tr->load(QLocale(tmp), QLatin1String("comp"), QLatin1String("_"), QString(QM_DIR)))
	//	{
	//		qCCritical(LC_ChatClient) << "Unable to find translation files";
	//		tr->deleteLater();
	//		return;
	//	}
	//	_translators.insert(tmp, tr); 
	//}
	//if (tr == _currentTranslator)
	//{
	//	qCDebug(LC_ChatClient) << "Received same translator";
	//	return;
	//}
	//if(QCoreApplication::installTranslator(tr))
	//{
	//	if(_currentTranslator)
	//		QCoreApplication::removeTranslator(_currentTranslator);
	//	_currentTranslator = tr;
	//	_qmlEngine->retranslate();
	//	_qmlEngine->setUiLanguage(tmp);
	//	settings.setLanguage(tmp);
	//	qCDebug(LC_ChatClient) << "Language was set to " << tmp;
	//}
	//else
	//{
	//	qCCritical(LC_ChatClient) << "Unable to set language: " << tmp;

	//}

	
}
void App::logout(const QString& reason)
{
	chat->hide();
	delete chat;
	roomController->logout();
	messageController->logout();
	userController->logout();

	chat = _windowFactory->createChatWindow(roomController, userController, messageController);
	connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });

	startup->clear();
	startup->setErrorString(reason);
	startup->show();
}