#include "core_include.h"
#include <QString>
#include <QVideoSink>
#include "idmodel.h"
struct CC_CORE_EXPORT Participate
{
	Participate();
	int userID;
	bool muted;
	std::optional<QVideoSink*> videoSink;
	QVariantHash toHash() const;
	void fromHash(const QVariantHash& hash);

};
class CC_CORE_EXPORT ParticipateModel : public IdentifyingModel<Participate>
{
	Q_OBJECT;
public:
	enum Roles
	{
		UserID,
		Muted,
		VideoSink
	}; Q_ENUM(Roles);
	explicit ParticipateModel(QObject* parent = nullptr);
protected:
	bool edit(Participate&, const QVariant&, int row, int role) override;
	QVariant read(const Participate&, int row, int role) const override;
};