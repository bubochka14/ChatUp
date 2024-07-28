#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")

ChatClient::ChatClient(QObject* parent)
	:QObject(parent)
	, _WSClient(QSharedPointer<WSClient>::create(QWebSocketProtocol::VersionLatest))
	, _qmlEngine(new QQmlEngine(this))
	,_currentTranslator(nullptr)
{
	setAppLanguage();
	_qmlEngine->rootContext()->setContextProperty("roomModel",&_model);
	_window = QSharedPointer<ChatWindow>::create(_qmlEngine);
	_dialog = QSharedPointer<UserVerifyDialog>::create(_qmlEngine);


}

void ChatClient::run(const QUrl& url)
{
	setupWSClient(url);
	authenticateUser();
	_window->show();
	auto rr = _WSClient.get();;
	ServerMethodCaller caller(rr);
	RoomList rooms = caller.getUserRooms(authMaster->userInfo().id).result();
	_model.extractFromRoomList(rooms);
	connect(_WSClient.get(), &WSClient::postMessage, [=](int roomID, const ChatRoomMessage& m)
		{

			QModelIndexList list = _model.match(_model.index(0), ChatRoomModel::IDRole, QVariant::fromValue(roomID));
			_model.data(list.at(0), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>()->pushMessage(m);

		});
	for (size_t i = 0; i < _model.rowCount(); i++)
	{
		auto historyModel = _model.data(_model.index(i), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>();
		historyModel->upload(caller.getRoomHistory(authMaster->userToken(),
			_model.data(_model.index(i), ChatRoomModel::IDRole).value<int>()).result());
	}
	connect(_window.get(), &ChatWindow::languageChanged, this, &ChatClient::setAppLanguage);
	connect(_window.get(), &ChatWindow::chatMessage, this, [&](const QString& m, long id)
		{
			ServerMethodCaller caller(_WSClient.get());
			caller.sendChatMessage(authMaster->userToken(), id, m);
		});
	
}
QString ChatClient::authenticateUser()
{

	_dialog->show();
	QEventLoop eLoop;

	connect(_dialog.get(), &UserVerifyDialog::loginPassed, this, [&](const QString& username, const QString& password)
		{
			_dialog->setLoadingScreen(true);
			if (authMaster->loginUser(_WSClient.get(), {username,password}, AUTH_CONNECTION_SEC))
			{
				_dialog->close();
				eLoop.exit();
			}
			else {
				_dialog->setLoadingScreen(false);
				_dialog->setErrorString(tr("Unable to login: ") + authMaster->errorString());
			}
	});
	connect(_dialog.get(), &UserVerifyDialog::registerPassed, this, [&](const QString& username, const QString& password)
	{
		_dialog->setLoadingScreen(true);
		if (authMaster->registerUser(_WSClient.get(), {username,password}, AUTH_CONNECTION_SEC))
		{
			eLoop.exit();
		}
		else {
			_dialog->setLoadingScreen(false);
			_dialog->setErrorString(tr("Unable to register: ") + authMaster->errorString());
		}
	});
	while (!authMaster->isAuthenticated()) {
		eLoop.exec();
	}
	_dialog->close();
	return authMaster->userToken(); 

}
bool ChatClient::setupWSClient(const QUrl& url)
{
	return _WSClient->connect2Server(url);
	
}
void ChatClient::setAppLanguage(const QString& lan)
{
	QString tmp = lan;
	ApplicationSettings settings;

	if (lan.isEmpty())
		tmp = settings.language();

	QTranslator* tr = _translators.value(tmp);
	if (!tr)
	{
		tr = new QTranslator(this);
		if (!tr->load(QLocale(tmp), QLatin1String("comp"), QLatin1String("_"), QString(QM_DIR)))
		{
			qCCritical(LC_ChatClient) << "Unable to find translation files";
			tr->deleteLater();
			return;
		}
		_translators.insert(tmp, tr); 
	}
	if (tr == _currentTranslator)
	{
		qCDebug(LC_ChatClient) << "Received same translator";
		return;
	}
	if(QCoreApplication::installTranslator(tr))
	{
		if(_currentTranslator)
			QCoreApplication::removeTranslator(_currentTranslator);
		_currentTranslator = tr;
		_qmlEngine->retranslate();
		_qmlEngine->setUiLanguage(tmp);
		settings.setLanguage(tmp);
		qCDebug(LC_ChatClient) << "Language was set to " << tmp;
	}
	else
	{
		qCCritical(LC_ChatClient) << "Unable to set language: " << tmp;

	}

	
}
