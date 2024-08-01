#pragma once
#include <QtPlugin>
class AbstractUserVerifier : public QObject
{
	Q_OBJECT
public:
	enum VerifyState
	{
		Inital,
		Error,
		Loading,
	};
	VerifyState state() const;
	void setState(VerifyState other);
	virtual ~AbstractUserVerifier() = default;
signals:
	void stateChanged();
	void registerPassed(const QString& user, const QString& password);
	void loginPassed(const QString& user, const QString& password);
protected:
	explicit AbstractUserVerifier(QObject* parent = nullptr);
private:
	Q_PROPERTY(VerifyState state READ state WRITE setState NOTIFY stateChanged);
	VerifyState _state;

};