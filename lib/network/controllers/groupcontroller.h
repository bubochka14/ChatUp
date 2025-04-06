#pragma once
#include "network_include.h"
#include <qobject.h>
#include <qqmlengine.h>
#include <qfuture.h>
#include "groupmodel.h"
#include "api/group.h"
#include "api/message.h"
#include "networkcoordinator.h"
#include <qtimer>
#include <queue>
#include "abstractcontroller.h"
namespace Group {
	class CC_NETWORK_EXPORT  Controller: public AbstractController
	{
		Q_OBJECT;
		Q_PROPERTY(Model* model READ model NOTIFY modelChanged);
	public:
		explicit Controller(QObject* parent = nullptr);
		Q_INVOKABLE virtual QFuture<int> create(const QString& name) = 0;
		Q_INVOKABLE virtual QFuture<void> addUser(int userID, int roomId) = 0;
		Q_INVOKABLE virtual QFuture<void> remove(int id) = 0;
		Q_INVOKABLE virtual QFuture<void> update(const QVariantHash& data) = 0;
		Q_INVOKABLE virtual QFuture<void> load() = 0;
		Q_INVOKABLE virtual QFuture<void> setLocalReadings(int roomID, size_t count) =0;
		Q_INVOKABLE virtual void setLastSender(int roomID, int id) =0;
		Q_INVOKABLE virtual void setLastMessageBody(int roomID, const QString& body) =0;
		Q_INVOKABLE virtual void setLastMessageTime(int roomID, const QDateTime& time) =0;
		Q_INVOKABLE virtual bool incrementMessageCount(int roomID,size_t count) =0;
		Model* model() const;
		void reset() override;
	signals:
		void modelChanged();
	protected:
		void setModel(Model* other);
	private:
		Model* _model;

	};
	class CC_NETWORK_EXPORT CallerController : public Controller
	{
		Q_OBJECT;
	public:
		explicit CallerController(std::shared_ptr<NetworkCoordinator> manager,
			QObject* parent = nullptr);
		QFuture<int>  create(const QString& name) override;
		QFuture<void> addUser(int userID, int roomId) override;
		QFuture<void> remove(int id) override;
		QFuture<void> update(const QVariantHash& data) override;
		QFuture<void> load() override;
		QFuture<void> initialize() override;
		QFuture<void> setLocalReadings(int roomID, size_t count) override;
		bool incrementMessageCount(int roomID, size_t count) override;
		void setLastSender(int roomID, int id) override;
		void setLastMessageBody(int roomID,const QString& body) override;
		void setLastMessageTime(int roomID, const QDateTime& time) override;
	private:
		std::shared_ptr<NetworkCoordinator> _manager;
		std::queue<int> _readingsUpdateQueue;
		QTimer* _updateReadingsTimer;
		std::mutex mutex;
	};
}