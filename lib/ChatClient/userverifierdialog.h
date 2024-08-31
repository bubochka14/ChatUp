//#pragma once
//#include <QObject>
//#include <QLoggingCategory>
//#include <qquickwindow.h>
//#include <QQuickitem>
//#include <qqmlengine.h>
//#include <qqmlcomponent.h>
//#include "abstractuserverifier.h"
//Q_DECLARE_LOGGING_CATEGORY(LC_VERIFY_DIALOG);
//class UserVerifierDialog : public AbstractUserVerifier
//{
//	Q_OBJECT;
//public:
//	explicit UserVerifierDialog(QQmlEngine* engine, QObject * parent = nullptr);
//	QQmlEngine* engine() const;
//	void show();
//	void hide();
//	~UserVerifierDialog();
//public slots:
//	void setErrorString(const QString& error);
//private:
//	QQmlEngine* _engine;
//	QScopedPointer<QQuickWindow> _view;
//};