#include "abstractchatwindow.h"
AbstractChatWindow::AbstractChatWindow(QObject* parent)
	:QObject(parent)
	//,_settings(settings)
{}
void AbstractChatWindow::setController(AbstractChatController* other)
{
	if (other == _controller)
		return;
	_controller = other;
	emit controllerChanged();
}
AbstractChatController* AbstractChatWindow::controller() const
{
	return _controller;
}
//ApplicationSettings* AbstractChatWindow::settings() const
//{
//	return _settings;
//}