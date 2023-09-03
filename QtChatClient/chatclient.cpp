#include "chatclient.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")

ChatClient::ChatClient(QObject* parent)
	:QObject(parent)
	, _WSClient(QSharedPointer<WSClient>::create(QWebSocketProtocol::VersionLatest))
	//, _qmlEngine(QQmlApplicationEngine(this))
{
	_qmlEngine.rootContext()->setContextProperty("roomModel",&_model);
	_window = QSharedPointer<ChatWindow>::create(&_qmlEngine);
}

void ChatClient::run(const QUrl& url)
{
	setupWSClient(url);
	authenticateUser();
	qDebug() << "TOKEN " << authMaster->userToken();
	qDebug() << "Username " << authMaster->userInfo().name;
	_window->show();
	auto rr = _WSClient.get();;
	ServerMethodCaller caller(rr);
	RoomList rooms = caller.getUserRooms(authMaster->userInfo().id).result();
	_model.extractFromRoomList(rooms);
	connect(_WSClient.get(), &WSClient::postMessage, [=](int roomID, const ChatRoomMessage& m)
		{

			try{
				qDebug() << "Trying to upload message to roomID: " << roomID << " MESSAGE " << m.body << " FROM " << m.user.name << " TIME " << m.time;
				QModelIndexList list = _model.match(_model.index(0), ChatRoomModel::IDRole, QVariant::fromValue(roomID));
				_model.data(list.at(0), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>()->pushMessage(m);
				qDebug() << "UPLOADED";
			}
			catch (...)
			{
				qDebug() << "EXCEPTION ERROR";
			}
		});
	for (size_t i = 0; i < _model.rowCount(); i++)
	{
		auto historyModel = _model.data(_model.index(i), ChatRoomModel::HistoryModelRole).value<MessageHistoryModel*>();
		historyModel->upload(caller.getRoomHistory(authMaster->userToken(),
			_model.data(_model.index(i), ChatRoomModel::IDRole).value<int>()).result());
	}
	connect(_window.get(), &ChatWindow::chatMessage, this, [&](const QString& m, long id)
		{
			ServerMethodCaller caller(_WSClient.get());
			 caller.sendChatMessage(authMaster->userToken(), id, m);
		});
	
}
QString ChatClient::authenticateUser()
{
	QSharedPointer<UserVerifyDialog> startDialog = QSharedPointer<UserVerifyDialog>::create(&_qmlEngine);
	startDialog->show();
		QEventLoop eLoop;

	connect(startDialog.get(), &UserVerifyDialog::loginPassed, this, [&](const QString& username, const QString& password)
		{
			startDialog->setLoadingScreen(true);
			if (authMaster->loginUser(_WSClient.get(), {username,password}, AUTH_CONNECTION_SEC))
			{
				startDialog->close();
				eLoop.exit();
			}
			else {
				startDialog->setLoadingScreen(false);
				startDialog->setErrorString(tr("Unable to login: ") + authMaster->errorString());
			}
	});
	connect(startDialog.get(), &UserVerifyDialog::registerPassed, this, [&](const QString& username, const QString& password)
	{
		startDialog->setLoadingScreen(true);
		if (authMaster->registerUser(_WSClient.get(), {username,password}, AUTH_CONNECTION_SEC))
		{
			eLoop.exit();
		}
		else {
			startDialog->setLoadingScreen(false);
			startDialog->setErrorString(tr("Unable to register: ") + authMaster->errorString());
		}
	});
	while (!authMaster->isAuthenticated()) {
		eLoop.exec();
	}
	startDialog->close();
	return authMaster->userToken(); 

}
bool ChatClient::setupWSClient(const QUrl& url)
{
	return _WSClient->connect2Server(url);
	
}