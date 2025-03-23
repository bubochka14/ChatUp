#pragma once
#include "data.h"
#include <qqmlengine.h>
#include "core_include.h"
#include <qobject.h>
namespace User {
	class CC_CORE_EXPORT Handle : public QObject
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(UserHandle);
		QML_UNCREATABLE("");
		Q_PROPERTY(User::Status status READ status WRITE setStatus NOTIFY statusChanged);
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
		Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged);
		Q_PROPERTY(int id READ id WRITE setID NOTIFY idChanged);
	public:
		explicit Handle(User::Data&& data, QObject* parent = nullptr);
		void release();
		QString name() const;
		QString tag() const;
		int id() const;
		User::Status status() const;
		void setID(int other);
		void setTag(const QString& other);
		void setName(const QString& other);
		void setStatus(Status other);
	signals:
		void statusChanged();
		void nameChanged();
		void tagChanged();
		void idChanged();
	private:
		User::Data _data;
	};
}