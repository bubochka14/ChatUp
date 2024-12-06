#pragma once
#include <QString>
#include <QFuture>
#include <qvariant.h>
#include <qhash.h>
#include <network_include.h>
#include "participatemodel.h"
#include <qvideosink.h>
#include <qqmlengine.h>
namespace Calls{
	class Controller;

	class CC_NETWORK_EXPORT Handler : public QObject
	{
		Q_OBJECT;
		QML_ELEMENT;
		QML_UNCREATABLE("");
		Q_PROPERTY(ParticipateModel* participants READ participants NOTIFY participantsChanged);
		Q_PROPERTY(bool hasCall READ hasCall NOTIFY hasCallChanged);
		Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged);
		Q_PROPERTY(int roomID READ roomID CONSTANT)
	public:
		explicit Handler(Controller* controller, int roomID);
		bool hasCall() const;
		ParticipateModel* participants();
		int roomID() const;
		void setIsMuted(bool other);
		bool isMuted() const;
		Q_INVOKABLE QFuture<void> disconnect();
		Q_INVOKABLE QFuture<bool> join();
		Q_INVOKABLE void release();
	signals:
		void participantsChanged();
		void hasCallChanged();
		void isMutedChanged();
	protected:
		void setHasCall(bool other);

	private:
		Controller* _controller;
		ParticipateModel* _prt;
		const int _roomID;
		bool _hasCall;
		bool _isMuted;
	friend class Controller;
	};
	class CC_NETWORK_EXPORT Controller : public QObject
	{
		Q_OBJECT;
		QML_ELEMENT;
		QML_UNCREATABLE("");
	public:
		explicit Controller(QObject* parent = nullptr);
		Handler* handler(int roomID);

		Q_INVOKABLE QFuture<void> disconnect(Handler* h);
		Q_INVOKABLE QFuture<bool> join(Handler* h);
		Q_INVOKABLE void release(Handler* h);
		
	private:
		QHash<int, Handler*> _handlers;

	};
}