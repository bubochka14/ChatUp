#include "qmlprofileviewer.h"
Q_LOGGING_CATEGORY(LC_PROFILE_VIEWER,"ProfileViewer")

QmlProfileViewer::QmlProfileViewer(QObject* parent)
	:QObject(parent)
	,_view(nullptr)
{

}
void QmlProfileViewer::showProfile(User::Handle* h)
{
	if (!_view)
	{
		qCWarning(LC_PROFILE_VIEWER) << "Cannot display user profile, view is not set";
		return;
	}
	QMetaObject::invokeMethod(_view, "showProfile",
		Q_ARG(User::Handle*, h));
}
QQuickItem* QmlProfileViewer::view()
{
	return _view;
}
void QmlProfileViewer::setView(QQuickItem* other)
{
	if (_view == other)
		return;
	_view = other;
	emit viewChanged();
}