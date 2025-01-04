#include "participatemodel.h"
using namespace Participate;

static const QHash<int, QByteArray> roles = {
	{Model::UserID, "userID"},
	{Model::HasVideo, "hasVideo"},
	{Model::HasAudio, "hasAudio"}
};
Model::Model(QObject* parent)
	:IdentifyingModel(roles, parent)
{
}
bool Model::edit(Data& par, const QVariant& var, int row, int role)
{
	switch (role)
	{
	case UserID:
		if (var.canConvert<int>() && var.toInt() != par.userID)
		{
			par.userID = var.toInt();
			return true;
		}
	case HasVideo:
		if (var.canConvert<bool>() && var.toBool() != par.hasVideo)
		{
			par.hasVideo = var.toBool();
			return true;
		}
	case HasAudio:
		if (var.canConvert<bool>() && var.toBool() != par.hasAudio)
		{
			par.hasAudio = var.toBool();
			return true;

		}
	}
	return false;
}
QVariant Model::read(const Data& par, int row, int role) const
{
	switch (role)
	{
	case UserID:
		return par.userID;
	case HasVideo:
		return par.hasVideo;
	case HasAudio:
		return par.hasVideo;
	}
	return QVariant();
}