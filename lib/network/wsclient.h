#pragma once
#include "transport.h"
#include "rtc/websocket.hpp"
#include <QString>
class WSClient : public Transport
{
	Q_OBJECT;
public:
	explicit WSClient(QObject* parent = nullptr);
	bool sendText(QString txt) noexcept override ;
	void connect2Server(QUrl url) noexcept override;

private:
	std::shared_ptr<rtc::WebSocket> _ws;
};