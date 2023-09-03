#include "chatclient.h"
Q_LOGGING_CATEGORY(LC_ChatClient, "ChatClient")

ChatClient::ChatClient(QObject* parent)
	:QObject(parent)
	,_WSClient(new WSClient(QWebSocketProtocol::VersionLatest,this))
	, _qmlEngine(new QQmlApplicationEngine(this))
{
}

void ChatClient::setHostUrl(const QUrl& other)
{
	if (_hostUrl != other)
	{
		_hostUrl = other;
		emit hostUrlChanged();
	}
	return;
}
QUrl ChatClient::hostUrl() const
{
	return _hostUrl;
}
void ChatClient::run(const QUrl& url)
{
	setHostUrl(url);
	setupWSClient(hostUrl());
	_userToken= authenticateUser();
	qDebug() << "TOKEN " << authMaster->userToken();
	qDebug() << "Username " << authMaster->userInfo().username;
	ChatRoomModel* model = new ChatRoomModel(this);
	ChatWindow* window = new ChatWindow(model);
	window->show();
	ServerMethodCaller* caller = new ServerMethodCaller(_WSClient);
	RoomList rooms = caller->getUserRooms(authMaster->userInfo().userID).result();
	model->extractFromRoomList(rooms);

	connect(window, &ChatWindow::chatMessage,this, [caller](const QString& m, long id)
		{
			return caller->sendChatMessage(authMaster->userToken(), id, m);
		});
}
QString ChatClient::authenticateUser()
{
	QSharedPointer<UserVerifyDialog> startDialog( new UserVerifyDialog(_qmlEngine));
	startDialog->show();
		QEventLoop eLoop;

	connect(startDialog.get(), &UserVerifyDialog::loginPassed, this, [&](const QString& username, const QString& password)
		{
			startDialog->setLoadingScreen(true);
			if (authMaster->loginUser(_WSClient, { username,password }, AUTH_CONNECTION_SEC))
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
		if (authMaster->registerUser(_WSClient, { username,password }, AUTH_CONNECTION_SEC))
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