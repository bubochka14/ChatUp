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
		Q_PROPERTY(User::Status status READ status NOTIFY statusChanged);
		Q_PROPERTY(QString name READ name NOTIFY nameChanged);
		Q_PROPERTY(QString tag READ tag NOTIFY tagChanged);
		Q_PROPERTY(int id READ id NOTIFY idChanged);
	public:
		explicit Handle(User::Data&& data, QObject* parent = nullptr);
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
}