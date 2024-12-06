#include "participatemodel.h"
Participate::Participate()
	:userID(-1)
	,videoSink(std::nullopt)
	,muted(false)
{}
static const QHash<int, QByteArray> roles = {
	{ParticipateModel::UserID, "userID"},
	{ParticipateModel::Muted, "muted"},
	{ParticipateModel::VideoSink, "videoSink"}
};
ParticipateModel::ParticipateModel(QObject* parent)
	:IdentifyingModel(roles, parent)
{
}
bool ParticipateModel::edit(Participate& par, const QVariant& var, int row, int role)
{
	switch (role)
	{
	case UserID :
		if(var.canConvert<int>() && var.toInt() != par.userID)
		{
			par.userID = var.toInt();
			return true;
		}
	case VideoSink:
		if (var.canConvert<QVideoSink*>() && var.value<QVideoSink*>() != par.videoSink.value())
		{
			par.videoSink = var.value<QVideoSink*>();
			return true;
		}
	case Muted:
		if (var.canConvert<bool>() && var.toBool() != par.muted)
		{
			par.muted = var.toBool();
			return true;
		}
	}
	return false;
}
QVariant ParticipateModel::read(const Participate& par, int row, int role) const
{
	switch (role)
	{
	case UserID:
		return par.userID;
	case VideoSink:
		if(par.videoSink.has_value())
			return QVariant::fromValue(par.videoSink.value());
		return QVariant();
	case Muted:
		return par.muted;
	}
	return QVariant();
}