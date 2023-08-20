#include "chatclient.h"
#include <QtCore/QDebug>

QT_USE_NAMESPACE

//! [constructor]
ChatClient::ChatClient(const QUrl& url, bool debug, QObject* parent) :
    QObject(parent),
    m_url(url),
    m_debug(debug)
{
    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &ChatClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &ChatClient::closed);
    m_webSocket.open(QUrl(url));
}
//! [constructor]

//! [onConnected]`  
void ChatClient::onConnected()
{
    if (m_debug)
        qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
        this, &ChatClient::onTextMessageReceived);
    //m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}
//! [onConnected]

//! [onTextMessageReceived]
void ChatClient::onTextMessageReceived(QString message)
{
    if (m_debug)
        qDebug() << "Message received:" << message;
    m_webSocket.close();
}
//! [onTextMessageReceived]
void ChatClient::sendMessage(const QString& message)
{
    qDebug() << "Sent " <<m_webSocket.sendTextMessage(message) << " bytes.";
    
}