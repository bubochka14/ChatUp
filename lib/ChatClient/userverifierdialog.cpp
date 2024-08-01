#include "userverifierdialog.h"
Q_LOGGING_CATEGORY(LC_VERIFY_DIALOG, "UserVerifyDialog")
UserVerifierDialog::UserVerifierDialog(QQmlEngine* engine, QObject* parent)
	:_engine(engine)
{
	QQmlComponent component(_engine);
	component.loadUrl(QUrl("qrc:/qt/qml/ChatClient/AuthorizationDialog.qml"));
	_view.reset(qobject_cast<QQuickWindow*>(component.create()));
	if (!_view.get())
	{
		qCCritical(LC_VERIFY_DIALOG) << "Unable to load qml file: " << component.errorString();
	}
	connect(_view.get(), SIGNAL(registerUser(QString, QString)), this, SIGNAL(registerPassed(QString, QString)));
	connect(_view.get(), SIGNAL(loginUser(QString, QString)), this, SIGNAL(loginPassed(QString, QString)));
	connect(this, &AbstractUserVerifier::stateChanged, this, [=]() {
		if (state() == Loading) {
			QMetaObject::invokeMethod(_view.get(), "setLoadingScreen",
				Q_ARG(QVariant, true));
		}else
			QMetaObject::invokeMethod(_view.get(), "setLoadingScreen",
				Q_ARG(QVariant, false));
	});
}
QQmlEngine* UserVerifierDialog::engine() const
{
	return _engine;
}
void UserVerifierDialog::setErrorString(const QString& error)
{
	QMetaObject::invokeMethod(_view.get(), "setError",
		Q_ARG(QVariant, error));
	setState(Error);
}

void UserVerifierDialog::show()
{
	if(_view)
	_view->setVisible(true);
}
void UserVerifierDialog::hide()
{
	if(_view)
	_view->setVisible(false);
}
UserVerifierDialog::~UserVerifierDialog()
{
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
