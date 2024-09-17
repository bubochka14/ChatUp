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
	_netFactory->setParent(this);
	_windowFactory->setParent(this);
	//QWK::registerTypes(_engine);
	setAppLanguage();
}

int App::run()
{
	StartupWindow* startup = _windowFactory->createStartupWindow();
	AbstractChatController* _chatController = _netFactory->createChatController();
	AbstractChatWindow* chat = _windowFactory->createChatWindow(_chatController);
	ClientMethodDispatcher* _dispatcher = _netFactory->createDispatcher();;
	AuthenticationMaster* _authMaster = _netFactory->createAuthenticationMaster();;

	if (!startup || !chat)
		return 0;
	startup->setParent(this);
	connect(_dispatcher, &ClientMethodDispatcher::disconnected, this, [=](const QVariantHash& data)
		{
			qCDebug(LC_ChatClient) << "Disconnected";
		});
	connect(_dispatcher, &ClientMethodDispatcher::updatedMessage, this, [=](const QVariantHash& data)
		{
			bool st;
			int room = MessageModel::MessageData::checkRoomId(data, st);
			if (!st)
				return;
			int id = MessageModel::MessageData::checkId(data, st);
			if (st)
				_chatController->getRoomHistory(room).then([=](MessageModel* model)
					{
						model->setData(model->idToIndex(id),data, MessageModel::HashRole);
					});
		});
	connect(_dispatcher, &ClientMethodDispatcher::updatedUser, this, [=](const QVariantHash& data)
		{
		});
	connect(_dispatcher, &ClientMethodDispatcher::deletedRoom, this, [=](const QVariantHash& data)
		{
			bool st;
			int id = RoomModel::RoomData::checkId(data,st);
			auto rooms = _chatController->userRooms();
			if(st)
			rooms->removeRow(rooms->idToIndex(id).row());
		});
	connect(_dispatcher, &ClientMethodDispatcher::updatedRoom, this, [=](const QVariantHash& data)
		{
			bool st;
			int id = RoomModel::RoomData::checkId(data, st);
			auto rooms = _chatController->userRooms();
			if (st)
				rooms->setData(rooms->idToIndex(id),data, RoomModel::HashRole);
		});
	connect(_dispatcher, &ClientMethodDispatcher::deletedMessage, this, [=](const QVariantHash& data)
		{
			bool st;
			int room = MessageModel::MessageData::checkRoomId(data, st);
			if (!st)
				return;
			int id = MessageModel::MessageData::checkId(data, st);
			if (st)
				_chatController->getRoomHistory(room).then([=](MessageModel* model)
					{
						model->removeRow(model->idToIndex(id).row());
					});
		});
	connect(_dispatcher, &ClientMethodDispatcher::messagePosted, this, [=](const QVariantHash& data)
		{
			qDebug() << data;
			bool st;
			int room = MessageModel::MessageData::checkRoomId(data, st);
			if (!st)
				return;
			int id = MessageModel::MessageData::checkId(data, st);
			if (st)
				_chatController->getRoomHistory(room).then([=](MessageModel* model)
					{
						if(!model->insertRow(model->rowCount()))
							return;
						model->setData(model->index(model->rowCount()-1), data, MessageModel::HashRole);
					});
		});
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
