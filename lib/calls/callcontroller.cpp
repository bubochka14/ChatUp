#include "callcontroller.h"
using namespace Calls;
QVariantHash Participate::toHash() const
{
	return {{ "userID",userID }, { "muted",muted }};
}
void Participate::fromHash(const QVariantHash& hash)
{
	if (hash.contains("userID"))
		userID = hash["userID"].toString();
	if (hash.contains("muted"))
		muted = hash["muted"].toBool();

}