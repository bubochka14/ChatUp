#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")
App::App(const QString& host, int port, QObject* parent)
	:App(new WSNetworkManager(host, port),new QmlWindowFactory(), parent)

{
}
App::App(NetworkManager* netFact, AbstractWindowFactory* windowFactory, QObject* parent)
	:QObject(parent)
	,_manager(netFact)
	,_windowFactory(windowFactory)
{
	_controllerManager = new CallerControllerManager(_manager, this);
	_manager->setParent(this);
	_windowFactory->setParent(this);
	setAppLanguage();
}

int App::run()
{
	startup = _windowFactory->createStartupWindow();
	chat = _windowFactory->createChatWindow(_controllerManager);
	if (!startup || !chat)
		return 0;
	startup->setParent(this);
	_manager->initialize();
	QThread* coreThread = new QThread;;
	QObject* context = new QObject;
	context->moveToThread(coreThread);
	coreThread->start();
	connect(startup, &StartupWindow::registerPassed, this, [this, context]
	(const QString& login, const  QString& pass) {
			startup->clear();
			startup->setState(StartupWindow::Loading);
			_manager->setCredentials({ login,pass });
			_controllerManager->initializeAll()
				.then(context, [this]() {
				startup->clear();
				chat->show();
				startup->hide();
					})
				.onFailed([this](const QString& error)
					{
						startup->clear();
						startup->setErrorString(error);
					}
				).onFailed([this]
					{
						startup->clear();
						startup->setErrorString("Unknown error");
					});
		});
	connect(startup, &StartupWindow::loginPassed, this, [this,context](const QString& login, const  QString& pass) {
		startup->clear();
		startup->setState(StartupWindow::Loading);
		_manager->setCredentials({ login,pass });
		_controllerManager->initializeAll()
			.then(context, [this]() {
			startup->clear();
			chat->show();
			startup->hide();
				})
			.onFailed([this](const QString& error)
				{
					startup->clear();
					startup->setErrorString(error);
				}
			).onFailed([this]
				{
					startup->clear();
					startup->setErrorString("Unknown error");
				});
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
/*	roomController->logout();
	messageController->logout();
	userController->logout()*/;

	chat = _windowFactory->createChatWindow(_controllerManager);
	connect(chat, &AbstractChatWindow::logout, this, [this]() {logout(); });

	startup->clear();
	startup->setErrorString(reason);
	startup->show();
}