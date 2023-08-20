// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef ECHOCLIENT_H
#define ECHOCLIENT_H
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(const QUrl& url, bool debug = false, QObject* parent = nullptr);

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString message);
    void sendMessage(const QString& message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    bool m_debug;
};

#endif // ECHOCLIENT_H
