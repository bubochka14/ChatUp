#include "userverifydialog.h"
Q_LOGGING_CATEGORY(LC_VERIFY_DIALOG, "UserVerifyDialog")
UserVerifyDialog::UserVerifyDialog(QQmlEngine* engine, QWindow* parent)
	:QQuickWindow(parent)
{
	QQmlEngine* mEngine = engine;
	//if (!mEngine)
	//	mEngine = new QQmlEngine(this);
	QQmlComponent component(mEngine, (QUrl("qrc:qt/qml/components/AuthorizationDialog.qml")));
	//QQmlComponent component(mEngine, (QUrl("qrc:/components/AuthorizationDialog.qml")));

	_content.reset(component.create());
	if (_content.isNull())
	{
		qCritical() << "Unable to load dialog: " << component.errorString();
		qApp->quit();
	}
	qobject_cast<QQuickItem*>(_content.get())->setParentItem(contentItem());

	connect(_content.get(), SIGNAL(registerUser(QString, QString)), this, SIGNAL(registerPassed(QString, QString)));
	connect(_content.get(), SIGNAL(loginUser(QString, QString)), this, SIGNAL(loginPassed(QString, QString)));
	
	setFlag(Qt::FramelessWindowHint);
	setWidth(280);
	setHeight(320);
}
//
//bool UserVerifyDialog::verified() const
//{
//	return _verified;
//}
//bool UserVerifyDialog::opened() const
//{
//	return _dialogRun;
//}
//QString UserVerifyDialog::userToken() const
//{
//	return _userToken;
//}
//void UserVerifyDialog::run()
//{
//	if (_dialogRun )
//		return;
//	//QQmlComponent* component = new QQmlComponent(_engine,this);
//	//component->loadUrl(QUrl("qrc:/qml/AuthorizationDialog.qml"));
//	//_qmlDialogRoot = component->create();
//	//QQuickWindow* window = qobject_cast<QQuickWindow*>(_qmlDialogRoot);
//	//window->show();
//	QQmlComponent component(_engine, (QUrl("qrc:/qml/AuthorizationDialog.qml")));
//	QQuickWindow* window =new QQuickWindow;
//	qobject_cast<QQuickItem*>(component.create())->setParentItem(window->contentItem());
//	window->show();
//	_dialogRun = true;
//	//QObject::connect(_qmlDialogRoot, SIGNAL(registerUser(QString, QString)),
//	//	this, SLOT(_registerUser(QString, QString)));
//	//QObject::connect(_qmlDialogRoot, SIGNAL(loginUser(QString, QString)),
//	//	this, SLOT(_loginUser(QString, QString)));
//	//QObject::connect(_engine, &QQmlEngine::quit, this, &UserVerifyDialog::closed);
//}
//void UserVerifyDialog::_registerUser(const QString& user, const QString& password)
//{
//	setLoadingScreen(true);
//	AuthResponse response = _authMaster.registerUser(UserCredentials{user,password}, _awaitTime);
//	if (_handleResponse(response))
//	{
//		_verified = true;
//		_userToken = response.userToken;
//		emit authenticated(response.userToken);
//	}
//	else
//	{
//		setLoadingScreen(false);
//	}
//}
//void UserVerifyDialog::_loginUser(const QString& user, const QString& password)
//{
//	setLoadingScreen(true);
//	AuthResponse response = _authMaster.loginUser(UserCredentials{user,password},_awaitTime);
//	if (_handleResponse(response))
//	{
//		_verified = true;
//		_userToken = response.userToken;
//		emit authenticated(response.userToken);
//	}
//	else
//	{
//		setLoadingScreen(false);
//	}
//}
void UserVerifyDialog::setLoadingScreen(bool st)
{
	QMetaObject::invokeMethod(_content.get(), "setLoadingScreen",
		Q_ARG(QVariant, st));
}
//void UserVerifyDialog::close()
//{
//	if (!_dialogRun)
//		return;
//	qCDebug(LC_VERIFY_DIALOG) << "Closing dialog...";
//	QMetaObject::invokeMethod(_qmlDialogRoot, "close");
//	_dialogRun = false;
//	emit closed();
//}
//bool UserVerifyDialog::_handleResponse(const AuthResponse& response)
//{
//	switch (response.status)
//	{
//	case AuthResponse::AuthError:
//		_setErrorToDialog(response.errorString);
//		return false;
//	case AuthResponse::InvalidCredentials:
//		_setErrorToDialog(tr("Invalid credentials"));
//		return false;
//	case AuthResponse::Reregistration:
//		_setErrorToDialog(tr("User with same name already existed"));
//		return false;
//	case AuthResponse::NetworkTimeout:
//		_setErrorToDialog(tr("Unable to connect to server: timeout"));
//		return false;
//	case AuthResponse::NetworkError:
//		_setErrorToDialog(tr("Unable to connect to server"));
//		return false;
//	case AuthResponse::Success:
//		qCDebug(LC_VERIFY_DIALOG) << "User successfully authenticated";
//		return true;
//	default:
//		return false;
//		break;
//	}
//}
void UserVerifyDialog::setErrorString(const QString& error)
{
	QMetaObject::invokeMethod(_content.get(), "setError",
		Q_ARG(QVariant, error));
}
