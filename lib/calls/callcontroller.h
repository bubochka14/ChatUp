#pragma once
#include <QString>
#include <QFuture>
#include <qstandarditemmodel.h>
#include <qhash.h>
#include <calls_include.h>
namespace Calls{
	struct CC_CALS_EXPORT Participate
	{
		QString userID;
		bool muted;
		QVariantHash toHash() const;
		void fromHash(const QVariantHash& hash);

	};
	/*class  CC_CALS_EXPORT ParticipateModel : QAbstractListModel
	{
		Q_OBJECT;
	public:
		enum Roles
		{
			UserID,
			Mooted
		}; Q_ENUM(Roles);
		explicit ParticipateModel(QObject* parent = nullptr);
		enum RoleNames
		{
			IdRole,
			UserIdRole,
			StatusRole,
			BodyRole,
			TimeRole,
			MessageIndexRole,
			HashRole
		}; Q_ENUM(RoleNames)
		explicit ParticipateModel(QObject* parent = nullptr);
		bool setData(const QModelIndex& index, const QVariant& value, int role = IdRole) override;
		int  rowCount(const QModelIndex& parent = QModelIndex()) const override;
		bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
		QVariant data(const QModelIndex& index, int role = IdRole) const override;
		QModelIndex idToIndex(int id) const;
	};*/
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