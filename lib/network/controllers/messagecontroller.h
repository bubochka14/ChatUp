#pragma once
#include "network_include.h"
#include <qobject.h>
#include <qqmlengine.h>
#include <qfuture.h>
#include "api/message.h"
#include "messagemodel.h"
#include "networkcoordinator.h"
#include "abstractcontroller.h"
#include <QStringBuilder>

namespace Message {
	
	class CC_NETWORK_EXPORT  Controller : public AbstractController
	{
		Q_OBJECT;
		//QML_ELEMENT;
		//QML_UNCREATABLE("Abstract controller");
	public:
		explicit Controller(QObject* parent = nullptr);
		Q_INVOKABLE virtual Model* model(int roomID) = 0;
		Q_INVOKABLE virtual QFuture<void> updateMessageBody(const QString& body, int roomID) = 0;
		Q_INVOKABLE virtual QFuture<int> create(const QString& body, int roomID) = 0;
		Q_INVOKABLE virtual QFuture<void> remove(int roomid, int messId) = 0;
		Q_INVOKABLE virtual QFuture<void> load(int roomID, int row, int fromIndex, int  toIndex) =0;

	};
	class CC_NETWORK_EXPORT CallerController : public Controller
	{
		Q_OBJECT;
	public:
		explicit CallerController(std::shared_ptr<NetworkCoordinator>,
			QObject* parent = nullptr);
		Model* model(int roomID) override;
		QFuture<void> load(int roomID,int row, int fromIndex, int  toIndex) override;
		QFuture<void> updateMessageBody(const QString& body, int roomID) override;
		QFuture<int> create(const QString& body, int roomID) override;
		QFuture<void> remove(int roomID, int messID) override;
		QFuture<void> initialize() override;
		void reset() override;
	private:
		std::shared_ptr<NetworkCoordinator> _manager;
		std::unordered_map<int, Model*> _history;
		int _tempMessageCounter;

	};
}