#include "abstractchatwindow.h"
AbstractChatWindow::AbstractChatWindow(QObject* parent)
	:QObject(parent)
{}
QFuture<void> AbstractChatWindow::initialize()
{
	return QtFuture::makeReadyVoidFuture();
}