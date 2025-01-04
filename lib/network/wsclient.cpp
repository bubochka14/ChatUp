#include "wsclient.h"
using namespace Qt::StringLiterals;
WSClient::WSClient(QObject* parent)
    :Transport(parent)
    ,_ws(std::make_shared<rtc::WebSocket>())
{
    _ws->onError([this](std::string s) {
        emit errorReceived(QString::fromStdString(s));
        });
    _ws->onMessage([this](auto data) {
        // data holds either std::string or rtc::binary
        if (!std::holds_alternative<std::string>(data))
            return;
        emit textReceived(QString::fromStdString(std::get<std::string>(data)));
        }); 
    _ws->onClosed([this]() {emit closed(); });
    _ws->onOpen([this]() {
        emit connected();
        });

}
void WSClient::connect2Server(QUrl url) noexcept
{
    try {
        _ws->open(url.toString().toStdString());
    }
    catch (std::string err)
    {
        emit errorReceived(QString::fromStdString(std::move(err)));
    }
    catch (...)
    {
        emit errorReceived("Unknown WebSocket error"_L1);
    }
}
bool WSClient::sendText(QString message) noexcept
{
    try {
        _ws->send(message.toStdString().c_str());
    }
    catch (std::string err)
    {
        emit errorReceived(QString::fromStdString(std::move(err)));
    }
    catch (...)
    {
        emit errorReceived("Unknown WebSocket error"_L1);
    }
    return true;
}
