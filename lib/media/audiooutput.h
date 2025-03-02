#pragma once
#include <qaudiooutput.h>
#include "media.h"
#include <qmediadevices.h>
#include <qaudioformat.h>
#include <qaudiodevice.h>
#include <qiodevice.h>
#include <QAudioSink>
#include "core.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libavutil/audio_fifo.h"

}
namespace Media::Audio {
	class CC_MEDIA_EXPORT Output : public QAudioOutput
	{
		QtEventLoopEmplacer* emp;
	public:
		Output();
		bool start(std::shared_ptr<Media::FramePipe>pipe);

	private:
		QIODevice* dev;
		QAudioDevice device;
	};
}