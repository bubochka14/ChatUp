#pragma once 
#include <qvarianthash.h>
#include <qdatetime.h>
#include <qvideosink.h>
#include <QMetaEnum>
#include "core_include.h"

class CC_CORE_EXPORT Serializable
{
public:
	virtual void fromHash(QVariantHash other) {};
	virtual QVariantHash toHash() const { return QVariantHash(); };
};
namespace Message
{
	static constexpr int invalidID = 0;
}
namespace User
{
	static constexpr int invalidID = 0;
}
namespace Direct
{
	static constexpr int invalidID = 0;
}
namespace Group
{
	static constexpr int invalidID = 0;

	struct CC_CORE_EXPORT Data : public Serializable
	{
		Data() = default;
		void fromHash(QVariantHash other) override;
		QVariantHash toHash() const override;
		int id = invalidID;
		int messageCount = 0;
		QString name;
		QString tag;

	};
}
namespace Direct
{
	struct CC_CORE_EXPORT Data : public Serializable
	{
		Data() = default;
		void fromHash(QVariantHash other) override;
		QVariantHash toHash() const override;
		int id	   = Direct::invalidID;
		int userID = User::invalidID;
	};
}
namespace User
{
	Q_NAMESPACE;
	CC_CORE_EXPORT enum Status
	{
		Offline,
		Online,
	}; Q_ENUM_NS(Status)
	struct CC_CORE_EXPORT Data : public Serializable
	{
		Data() = default;
		void fromHash(QVariantHash other) override;
		QVariantHash toHash() const override;
		Status status;
		QString name;
		QString tag;
		int id = User::invalidID;
	};
}
namespace Message
{
	Q_NAMESPACE;

	struct CC_CORE_EXPORT Data : public Serializable
	{
		Data() = default;
		static constexpr int defaultID = 0;
		QVariantHash toHash() const override;
		void fromHash(QVariantHash other) override;
		bool isValid() const;
		int id = Message::invalidID;
		int userID = User::invalidID;
		int roomID = Group::invalidID;
		QDateTime  time;
		QByteArray body;
		uint32_t messageIndex=0;
	};
}
namespace Participate {
	struct CC_CORE_EXPORT Data
	{
		Data() = default;
		int userID = User::invalidID;
		bool hasAudio = false;
		bool hasVideo = false;
		QVariantHash toHash() const;
		void fromHash(QVariantHash hash);

	};
}