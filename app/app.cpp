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
{
	_controllerManager = std::make_shared<CallerControllerManager>(_network);
	setAppLanguage();
}

void App::handleRegistration(const QString& login, const QString& pass)
{
	startup->clear();
	startup->setState(StartupWindow::Loading);
	startup->setStatus("Connecting");
	_controllerManager->userController()->create(pass, login)
		.then([this, login, pass]() {
		_network->setCredentials({ pass.toStdString(),login.toStdString() });
		_network->initialize()
			.then(this, [this]() {
			chat->initialize().then(this, [this]()
				{
					startup->hide();
					chat->show();
					startup->clear();
				});
				});

			});
}
void App::handleLogin(const QString& login, const QString& pass)
{
	startup->clear();
	startup->setState(StartupWindow::Loading);
	startup->setStatus("Connecting");
	_network->setCredentials({ login.toStdString(),pass.toStdString() });
	_network->initialize().then(this, [this]() {
		chat = _windowFactory->createChatWindow(_controllerManager);
		chat->initialize().then(this, [this](){
			startup->hide();
			chat->show();
			startup->clear();
		});
		}).onFailed(this, [this](const QString& error){
			startup->clear();
			startup->setErrorString(error);
		}
		).onFailed(this, [this]{
			startup->clear();
			startup->setErrorString("Unknown error");
		});
}

int App::run()
{
	qCDebug(LC_ChatClient) << "Running app";
	Core::Init();
	startup = _windowFactory->createStartupWindow();
	chat = _windowFactory->createChatWindow(_controllerManager);
	if (!startup || !chat)
		return -1;
	QObject::connect(startup, &StartupWindow::registerPassed, this, &App::handleRegistration);
	QObject::connect(startup, &StartupWindow::loginPassed, this, &App::handleLogin);
	//connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });
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
/*	roomController->logout();
	messageController->logout();
	userController->logout()*/;

	//chat = _windowFactory->createChatWindow(_controllerManager);
	//connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });

	startup->clear();
	startup->setErrorString(reason);
	startup->show();
}