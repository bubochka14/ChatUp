#include "app.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")

ChatClient::ChatClient(QObject* parent)
	:QObject(parent)
	,_wsClient(WSClient(QWebSocketProtocol::VersionLatest))
	,_qmlEngine(new QQmlApplicationEngine(this))
	,_currentTranslator(nullptr)
	,_window(new ChatWindow(_qmlEngine))
	,_dialog(new UserVerifierDialog(_qmlEngine))
	,_authMaster(new AuthenticationMaster(this))
{
	setAppLanguage();
	_qmlEngine->rootContext()->setContextProperty("roomModel",&_model);


}

void ChatClient::run(const QUrl& url)
{
	setupWSClient(url);
	authenticateUser();
	_window->show();
	ServerMethodCaller caller(&_wsClient);
	RoomList rooms = caller.getUserRooms(_authMaster->userInfo().id).result();
	_model.extractFromRoomList(rooms);
	connect(&_wsClient, &WSClient::postMessage, [=](int roomID, const ChatRoomMessage& m)
		{

			QModelIndexList list = _model.match(_model.index(0), ChatRoomModel::IDRole, QVariant::fromValue(roomID));
			_model.data(list.at(0), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>()->pushMessage(m);

		});
	for (size_t i = 0; i < _model.rowCount(); i++)
	{
		auto historyModel = _model.data(_model.index(i), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>();
		historyModel->upload(caller.getRoomHistory(_authMaster->userToken(),
			_model.data(_model.index(i), ChatRoomModel::IDRole).value<int>()).result());
	}
	connect(_window, &ChatWindow::languageChanged, this, &ChatClient::setAppLanguage);
	connect(_window, &ChatWindow::chatMessage, this, [&](const QString& m, long id)
		{
			ServerMethodCaller caller(&_wsClient);
			caller.sendChatMessage(_authMaster->userToken(), id, m);
		});
	
}
QString ChatClient::authenticateUser()
{

	_dialog->show();
	QEventLoop eLoop;

	connect(_dialog, &UserVerifierDialog::loginPassed, this, [&](const QString& username, const QString& password)
		{
			_dialog->setState(UserVerifierDialog::Loading);
			if (_authMaster->loginUser(&_wsClient, {username,password}, 5
			))
			{
				_dialog->hide();
				eLoop.exit();
			}
			else {
				_dialog->setState(UserVerifierDialog::Error);
				_dialog->setErrorString(tr("Unable to login: ") + _authMaster->errorString());
			}
	});
	connect(_dialog, &UserVerifierDialog::registerPassed, this, [&](const QString& username, const QString& password)
	{
		_dialog->setState(UserVerifierDialog::Loading);
		if (_authMaster->registerUser(&_wsClient, {username,password}, 5))
		{
			eLoop.exit();
		}
		else {
			_dialog->setState(UserVerifierDialog::Error);
			_dialog->setErrorString(tr("Unable to register: ") + _authMaster->errorString());
		}
	});
	eLoop.exec();
	return _authMaster->userToken(); 

}
bool ChatClient::setupWSClient(const QUrl& url)
{
	return _wsClient.connect2Server(url);
	
}
void ChatClient::setAppLanguage(const QString& lan)
{
	QString tmp = lan;
	ApplicationSettings settings(APP_NAME,ORG_NAME);

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
