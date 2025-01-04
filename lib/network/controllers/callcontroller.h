#pragma once
#include <QString>
#include <QFuture>
#include <qvariant.h>
#include <qhash.h>
#include <network_include.h>
#include <qvideosink.h>
#include <qqmlengine.h>
#include "participatemodel.h"
#include "abstractcontroller.h"
#include "api/call.h"
#include "networkmanager.h"
//#include "media.h"
//#include "audiooutput.h"
namespace Call {
	class Controller;
	class Handler;
	class CC_NETWORK_EXPORT Handler : public QObject
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(CallHandler);
		QML_UNCREATABLE("");

	public:
		enum State
		{
			Disconnected,
			HasCall,
			InsideTheCall
		};Q_ENUM(State)
		explicit Handler(Controller* controller, int roomID);
		State state() const;
		Participate::Model* participants();
		int roomID() const;
		void setIsMuted(bool other);
		bool isMuted() const;
		Q_INVOKABLE QFuture<void> disconnect();
		Q_INVOKABLE QFuture<void> join();
		Q_INVOKABLE void release();
	signals:
		void participantsChanged();
		void isMutedChanged();
		void stateChanged();
	protected:
		void setState(State other);
	private:
		Q_PROPERTY(Participate::Model* participants READ participants NOTIFY participantsChanged);
		Q_PROPERTY(State state READ state NOTIFY stateChanged);
		Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged);
		Q_PROPERTY(int roomID READ roomID CONSTANT)

		Controller* _controller;
		Participate::Model* _prt;
		const int _roomID;
		bool _isMuted;
		State _state;
		friend class Controller;
	};
	class CC_NETWORK_EXPORT Controller : public AbstractController
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(CallController)
		QML_UNCREATABLE("");
	public:
		explicit Controller(NetworkManager* manager, QObject* parent = nullptr);
		Q_INVOKABLE Handler* handler(int roomID);

		QFuture<void> disconnect(Handler* h);
		QFuture<void> join(Handler* h);
		void release(Handler* h);

	private:
		QHash<int, Handler*> _handlers;
		NetworkManager* _manager;
	};
}