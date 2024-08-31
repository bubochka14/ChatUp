#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")
ChatClient::ChatClient(const QString& host, int port, QObject* parent)
	:QObject(parent)
	,_appFactory(new WSApplicationFactory(host,port,this))
{
	_settings = _appFactory->createApplicationSettings();
	_windowFactory = _appFactory->createWindowFactory(_settings);
	_chatController = _appFactory->createChatController();
	_authMaster = _appFactory->createAuthenticationMaster();
	setAppLanguage();
}
ChatClient::ChatClient(ApplicationFactory* factory, QObject* parent)
	:QObject(parent)
	,_appFactory(factory)
	, _currentTranslator(nullptr)
{
	_settings = _appFactory->createApplicationSettings();
	_windowFactory = _appFactory->createWindowFactory(_settings);
	_chatController = _appFactory->createChatController();
	_authMaster = _appFactory->createAuthenticationMaster();
	setAppLanguage();
}

int ChatClient::run()
{
	StartupWindow* startup = _windowFactory->createStartupWindow();
	AbstractChatWindow* chat = _windowFactory->createChatWindow(_chatController);

	if (!startup || !chat)
		return 0;
	startup->setParent(this);
	connect(_authMaster, &AuthenticationMaster::authentificated, this, [=](UserInfo* userInfo) {
			_chatController->initializeUser(userInfo);
			startup->setStatus("Initialization...");
			startup->setLoadingProgress(0.5);
		});
	connect(_authMaster, &AuthenticationMaster::errorReceived, this, [=](const QString& error) {
		startup->clear();
		startup->setErrorString(error);
		});
	connect(_chatController, &AbstractChatController::initialized, this, [=]() {
			startup->clear();
			chat->show();
			startup->hide();
		});
	connect(startup, &StartupWindow::registerPassed, this, [=](const QString& login, const  QString& pass) {
			startup->clear();
			startup->setState(StartupWindow::Loading);
			startup->setStatus("Connecting...");
			startup->setLoadingProgress(0.1);
			_authMaster->registerUser(login, pass);
		});
	connect(startup, &StartupWindow::loginPassed, this, [=](const QString& login, const  QString& pass) {
			startup->clear();
			startup->setState(StartupWindow::Loading);
			startup->setStatus("Connecting...");
			startup->setLoadingProgress(0.1);
			_authMaster->loginUser(login, pass);
		});
	startup->show();
	return 1;
}
void ChatClient::setAppLanguage(const QString& lan)
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
