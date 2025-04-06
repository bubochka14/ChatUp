#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")
App::App(std::string host, int port)
	:App(std::make_shared<NetworkCoordinator>(host,port),
		std::make_shared<QmlWindowFactory>())

{
}
App::App(std::shared_ptr<NetworkCoordinator> netFact,
	std::shared_ptr<AbstractWindowFactory> windowFactory)
	:_windowFactory(windowFactory)
	,_network(netFact)
	,_emp(new QtEventLoopEmplacer(this))
	,chat(nullptr)
{
	_controllerManager = std::make_shared<CallerControllerManager>(_network);
	setAppLanguage();
}

void App::handleRegistration(const QString& login, const QString& pass)
{
	startup->clear();
	startup->setState(StartupWindow::Loading);
	startup->setStatus("Connecting");
	_controllerManager->userController()->create(pass, login).then([this, login, pass]() {
		_network->setCredentials({login.toStdString(),pass.toStdString() });
		_network->initialize().then(this, [this](){
			if (!chat)
				chat = createChatWindow();
			if (!chat)
				throw std::string("Cannot create chat window");
			chat->initialize().then(this, [this](){
				startup->hide();
				chat->show();
				startup->clear();
			});
		});

	}).onFailed(this,[this](std::string err) {
		startup->clear();
		startup->setErrorString(QString::fromStdString(std::move(err)));
	});
}
void App::handleLogin(const QString& login, const QString& pass)
{
	startup->clear();
	startup->setState(StartupWindow::Loading);
	startup->setStatus("Connecting");
	_network->setCredentials({ login.toStdString(),pass.toStdString() });
	_network->initialize().then(this, [this]() {
		if(!chat)
			chat = createChatWindow();
		if (!chat)
			throw std::string("Cannot create chat window");
		chat->initialize().then(this, [this](){
			startup->hide();
			chat->show();
			startup->clear();
		});
		}).onFailed(this, [this](std::string error){
			startup->clear();
			startup->setErrorString(QString::fromStdString(std::move(error)));
		}
		).onFailed(this, [this]{
			startup->clear();
			startup->setErrorString("Unknown error");
		});
}
AbstractChatWindow* App::createChatWindow()
{
	if (!_windowFactory || !_controllerManager)
		return nullptr;
	auto out = _windowFactory->createChatWindow(_controllerManager);
	connect(out, &AbstractChatWindow::logout, this, [this]() {logout(""); });
	return out;

}
int App::run()
{
	qCDebug(LC_ChatClient) << "Running app";
	Core::Init();
	startup = _windowFactory->createStartupWindow();
	if (!startup)
		return -1;
	QObject::connect(startup, &StartupWindow::registerPassed, this, &App::handleRegistration);
	QObject::connect(startup, &StartupWindow::loginPassed, this, &App::handleLogin);
	_network->onDisconnected([this]() {
		if(chat)
		{
			_emp->emplaceTask([this]() {
				logout("Server disconnected");
				});
		}
		});
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
	if(chat)
	{
		chat->hide();
		chat->deleteLater();
		chat = nullptr;
	}
	_network->disconnect();
	_controllerManager->resetAll();

/*	roomController->logout();
	messageController->logout();
	userController->logout()*/;

	//chat = _windowFactory->createChatWindow(_controllerManager);
	//connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });

	startup->clear();
	startup->setErrorString(reason);
	startup->show();
}