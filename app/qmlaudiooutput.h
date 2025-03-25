#pragma once
#include "audiooutput.h"
#include <qqmlengine.h>
class QMLAudioOutput : public Media::Audio::Output
{
	Q_OBJECT;
	QML_SINGLETON;

};