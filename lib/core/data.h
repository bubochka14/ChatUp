#pragma once 
#include <qvarianthash.h>
#include <qdatetime.h>
#include <qvideosink.h>
#include <QMetaEnum>
#include "core_include.h"
#include "core.h"
#include <QString>
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_DATA);
namespace Message
{
	static constexpr int invalidID = 0;
	static constexpr int invalidIndex = -1;
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
	struct CC_CORE_EXPORT Data
	{
		Data() = default;
		void fromHash(QVariantHash other);
		QVariantHash toHash() const;
		int id = invalidID;
		QString name;
		QString tag;

	};
	CC_CORE_EXPORT void to_json(json& j, const Data& p);
	CC_CORE_EXPORT void from_json(const json& j, Data& p);
	struct CC_CORE_EXPORT ExtendedData
	{
		ExtendedData() = default;
		void fromHash(QVariantHash other);
		QVariantHash toHash() const;
		int id = invalidID;
		int messageCount	= 0;
		int foreignReadings = 0;
		int localReadings	= 0;
		int lastSender;
		QDateTime lastMessageTime;
		QString lastBody;
		QString name;
		QString tag;

	};
	CC_CORE_EXPORT void to_json(json& j, const ExtendedData& p);
	CC_CORE_EXPORT void from_json(const json& j, ExtendedData& p);
}
namespace Direct
{
	struct CC_CORE_EXPORT Data
	{
		Data() = default;
		void fromHash(QVariantHash other);
		QVariantHash toHash() const;
		int id	   = Direct::invalidID;
		int userID = User::invalidID;
	};
	CC_CORE_EXPORT void to_json(json& j, const Data& p);
	CC_CORE_EXPORT void from_json(const json& j, Data& p);
}
namespace User
{
	Q_NAMESPACE;
	CC_CORE_EXPORT enum Status
	{
		Offline,
		Online,
	}; Q_ENUM_NS(Status)
	struct CC_CORE_EXPORT Data
	{
		Data() = default;
		bool isValid() const;
		QVariantHash toHash() const;
		void fromHash(QVariantHash other);
		Status status = Offline;
		QString name;
		QString tag;
		int id = User::invalidID;
	};
	CC_CORE_EXPORT void to_json(json& j, const Data& p);
	CC_CORE_EXPORT void from_json(const json& j, Data& p);
}
namespace Message
{
	Q_NAMESPACE;

	struct CC_CORE_EXPORT Data 
	{
		Data() = default;
		QVariantHash toHash() const;
		void fromHash(QVariantHash other);
		bool isValid() const;
		int id = Message::invalidID;
		int userID = User::invalidID;
		int roomID = Group::invalidID;
		QDateTime  time;
		QByteArray body;
		uint32_t messageIndex=0;
	};
	CC_CORE_EXPORT void to_json(json& j, const Data& p);
	CC_CORE_EXPORT void from_json(const json& j, Data& p);
}
namespace Participate {
	struct CC_CORE_EXPORT Data
	{
		Data() = default;
		int userID = User::invalidID;
		int roomID = Group::invalidID;
		bool hasAudio = false;
		bool hasVideo = false;
		QVariantHash toHash() const;
		void fromHash(QVariantHash hash);
		bool isValid() const;

	};
	CC_CORE_EXPORT void to_json(json& j, const Data& p);
	CC_CORE_EXPORT void from_json(const json& j, Data& p);
}