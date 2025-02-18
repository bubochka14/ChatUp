#pragma once
#include "network_include.h"
#include <qobject.h>
#include <qqmlengine.h>
#include <qfuture.h>
#include "usermodel.h"
#include "api/user.h"
#include "networkmanager.h"
#include "abstractcontroller.h"
#include "userhandle.h"
namespace User{
	class CC_NETWORK_EXPORT  Controller : public AbstractController
	{
		Q_OBJECT;
		QML_ELEMENT;
		QML_UNCREATABLE("Abstract controller");
		Q_PROPERTY(Handle* empty READ empty CONSTANT)
	public:
		explicit Controller(QObject* parent = nullptr);
		Q_INVOKABLE virtual QFuture<Model*> find(const QVariantHash& pattern, int limit) = 0;
		Q_INVOKABLE virtual QFuture<Handle*> get(int userID) = 0;
		Q_INVOKABLE virtual QFuture<Handle*> get() = 0;
		Q_INVOKABLE virtual QFuture<void> update(const QVariantHash& data) = 0;
		Q_INVOKABLE virtual QFuture<void> create(const QString& password,const QString& log) = 0;

		//Q_INVOKABLE virtual Handler* currentUser() = 0;
		Q_INVOKABLE virtual QFuture<void> remove() = 0;
		Handle* empty() const;
	protected:
		virtual Handle* getEmpty() const =0;

	};
	class CC_NETWORK_EXPORT CallerController final: public Controller
	{
		Q_OBJECT;
	public:
		explicit CallerController(std::shared_ptr<NetworkCoordinator> manager,
			QObject* parent = nullptr);
		QFuture<Model*> find(const QVariantHash& pattern, int limit) override;
		QFuture<Handle*> get(int userID) override;
		QFuture<Handle*> get() override;
		QFuture<void> create(const QString& password, const QString& log);
		QFuture<void> update(const QVariantHash& data) override;
		QFuture<void> remove() override;
		QFuture<void> initialize() override;
	protected:
		Handle* getEmpty() const override;


	protected:
		void connectToDispatcher();
	private:
		std::shared_ptr<NetworkCoordinator> _manager;
		QHash<int, Handle*> _userHandlers;
		QHash<int, QFuture<Handle*>> _pendingRequests;
		Handle* _empty;
	};
}