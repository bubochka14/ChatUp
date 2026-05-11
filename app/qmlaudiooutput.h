#pragma once
#include "audiooutput.h"
#include <qqmlengine.h>
class QMLAudioOutput : public Audio::Output
{
	Q_OBJECT;
	QML_SINGLETON;

};