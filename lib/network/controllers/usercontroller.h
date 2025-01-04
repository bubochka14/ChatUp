#pragma once
#include "network_include.h"
#include <qobject.h>
#include <qqmlengine.h>
#include <qfuture.h>
#include "usermodel.h"
#include "api/user.h"
#include "networkmanager.h"
#include "abstractcontroller.h"

namespace User {
	Q_NAMESPACE;
	class CC_NETWORK_EXPORT Handler : public QObject
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(UserHandler);
		QML_UNCREATABLE("");
		Q_PROPERTY(User::Status status READ status NOTIFY statusChanged);
		Q_PROPERTY(QString name READ name NOTIFY nameChanged);
		Q_PROPERTY(QString tag READ tag NOTIFY tagChanged);
		Q_PROPERTY(int id READ id NOTIFY idChanged);
	public:
		explicit Handler(User::Data&& data, QObject* parent = nullptr);
		void release();
		QString name() const;
		QString tag() const;
		int id() const;
		User::Status status() const;
	signals:
		void statusChanged();
		void nameChanged();
		void tagChanged();
		void idChanged();
	private:
		User::Data _data;
	};
	class CC_NETWORK_EXPORT  Controller : public AbstractController
	{
		Q_OBJECT;
		QML_ELEMENT;
		QML_UNCREATABLE("Abstract controller");
	public:
		explicit Controller(QObject* parent = nullptr);
		Q_INVOKABLE virtual QFuture<Model*> find(const QVariantHash& pattern, int limit) = 0;
		Q_INVOKABLE virtual QFuture<Handler*> get(int userID) = 0;
		Q_INVOKABLE virtual QFuture<void> update(const QVariantHash& data) = 0;
		Q_INVOKABLE virtual Handler* currentUser() = 0;
		Q_INVOKABLE virtual QFuture<void> remove() = 0;

	};
	class CC_NETWORK_EXPORT CallerController : public Controller
	{
		Q_OBJECT;
	public:
		explicit CallerController(NetworkManager* manager,
			QObject* parent = nullptr);
		QFuture<Model*> find(const QVariantHash& pattern, int limit) override;
		QFuture<Handler*> get(int userID) override;
		Handler* currentUser() override;
		QFuture<void> update(const QVariantHash& data) override;
		QFuture<void> remove() override;
		QFuture<void> initialize() override;

	protected:
		void connectToDispatcher();
	private:
		NetworkManager* _manager;
		QHash<int, Handler*> _userHandlers;
		QHash<int, QFuture<Handler*>> _pendingRequests;
		Handler* _empty;
	};
}