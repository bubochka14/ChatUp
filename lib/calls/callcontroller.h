#pragma once
#include <QString>
#include <QFuture>
#include <qvariant.h>
#include <qhash.h>
#include <calls_include.h>

#include <qvideosink.h>
namespace Calls{
	class CC_CALS_EXPORT Handler
	{
	public:
		void disconnect();
		void mute();
		void unmute();
		bool isValid();
	};
	class CC_CALS_EXPORT Controller
	{
	public:
		QFuture<Handler*> connect(const QString& roomID);
		void disconnect(Handler* other);
		Handler* getCurrentCall();
		bool isConnected();

	};
}