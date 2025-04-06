#pragma once
#include <qobject.h>
#include <qqmlengine.h>
#include <qquickitem.h>
#include <qloggingcategory.h>
#include <userhandle.h>
Q_DECLARE_LOGGING_CATEGORY(LC_PROFILE_VIEWER)
class QmlProfileViewer: public QObject 
{
	Q_OBJECT;
	QML_SINGLETON;
	QML_NAMED_ELEMENT(ProfileViewer);
	Q_PROPERTY(QQuickItem* view READ view WRITE setView NOTIFY viewChanged);
public:
	explicit QmlProfileViewer(QObject* parent = nullptr);
	Q_INVOKABLE void showProfile(User::Handle* h);

	QQuickItem* view();
	void setView(QQuickItem* other);
signals:
	void viewChanged();
private:
	QQuickItem* _view;

};