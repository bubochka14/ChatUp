#pragma once
#include "network_include.h"
#include <qobject.h>
#include <qqmlengine.h>
#include <qfuture.h>
#include "groupmodel.h"
#include "api/group.h"
#include "networkmanager.h"
#include "abstractcontroller.h"
namespace Group {
	class CC_NETWORK_EXPORT  Controller: public AbstractController
	{
		Q_OBJECT;
		//QML_ELEMENT;
		//QML_UNCREATABLE("Abstract class");
		Q_PROPERTY(Model* model READ model NOTIFY modelChanged);
	public:
		explicit Controller(QObject* parent = nullptr);
		Q_INVOKABLE virtual QFuture<int> create(const QString& name) = 0;
		Q_INVOKABLE virtual QFuture<void> addUser(int userID, int roomId) = 0;
		Q_INVOKABLE virtual QFuture<void> remove(int id) = 0;
		Q_INVOKABLE virtual QFuture<void> update(const QVariantHash& data) = 0;
		Q_INVOKABLE virtual QFuture<void> load(int count) = 0;
		Model* model() const;
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
		explicit CallerController(NetworkManager* manager,
			QObject* parent = nullptr);
		QFuture<int>  create(const QString& name) override;
		QFuture<void> addUser(int userID, int roomId) override;
		QFuture<void> remove(int id) override;
		QFuture<void> update(const QVariantHash& data) override;
		QFuture<void> load(int count) override;
		QFuture<void> initialize() override;

	protected:
		void connectToDispatcher();
	private:
		NetworkManager* _manager;

	};
}