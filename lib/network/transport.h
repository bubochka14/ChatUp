//#pragma once
//
//#include "network_include.h"
//class Transport : public QObject
//{
//    Q_OBJECT;
//public:
//    QString lastError() const { return _error; }
//    bool isConnected() const {
//        return _isConnected;
//    }
//    virtual bool sendText(QString txt) = 0;
//    virtual void connect2Server(QUrl url) = 0;
//    void connect2Server() { connect2Server(_serverUrl); };
//    QUrl serverUrl() const { return _serverUrl; }
//    void setServerUrl(QUrl other) { _serverUrl = std::move(other); }
//signals:
//    void closed();
//    void connected();
//    void textReceived(const QString&);
//    void errorReceived(const QString&);
//protected:
//    Transport(QObject* parent = nullptr) :_isConnected(false) {};
//    void setIsConnected(bool st) { _isConnected = st; };
//    void setLastError(QString other) { _error = std::move(other); };
//private:
//    QUrl _serverUrl;
//    QString _error;
//    bool _isConnected = false;
//};