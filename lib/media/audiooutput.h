#pragma once
#include <qaudiooutput.h>
#include "media.h"
#include <qmediadevices.h>
#include <qaudioformat.h>
#include <qaudiodevice.h>
#include <qiodevice.h>
#include <QAudioSink>
namespace media {

	class AudioOutput : public QAudioOutput
	{
	public:
		AudioOutput(QAudioDevice dev)
			:device(std::move(dev))
		{}
		bool start(std::shared_ptr<FramePipe>pipe,media::Audio::Source sr)
		{
			QAudioFormat format;

			format.setSampleRate(sr.par->sample_rate);
			format.setChannelCount(sr.par->channels);
			format.setSampleFormat(Audio::toQtFormat((AVSampleFormat)sr.par->format));
			if (!device.isFormatSupported(format)) {
				qWarning() << "Raw audio format not supported by backend, cannot play audio.";
				return false;
			}
			QAudioSink* sink = new QAudioSink(QMediaDevices::defaultAudioOutput(),format);
			PipeAudioBuffer* buf = new PipeAudioBuffer(pipe);
			auto open = buf->open(QIODevice::ReadOnly);
			connect(sink, &QAudioSink::stateChanged, this, [=](QAudio::State st) {
				});
			sink->start(buf);

		}
	private:
		class PipeAudioBuffer : public QIODevice
		{
		public:
			PipeAudioBuffer(std::shared_ptr<FramePipe> p)
				:pipe(p)
			{
				pipe->onDataChanged([this](std::weak_ptr<AVFrame>wfr, size_t index) {
					auto frame = wfr.lock();
					if (!frame)
						return;
					{
						std::lock_guard g(mutex);
						frameDeque.emplace_back(std::make_pair(frame, index));
						//samples += frame->channels * frame->sample_rate;
					}
					emit readyRead();
					});
					
			}
		public:
			qint64 readLineData(char* data, qint64 maxSize) override
			{
				return 2;
			}
			qint64 bytesAvailable() const override
			{
				std::lock_guard g(mutex);

				return 44200*2*frameDeque.size();
			}
			qint64 readData(char* data, qint64 maxSize) override {
				int bytesPopped = 0;
				{
					std::lock_guard g(mutex);
					memcpy(data, frameDeque.front().first->data[0], maxSize);
					pipe->unmapReading(frameDeque.front().second);
					frameDeque.pop_front();
					samples -= maxSize;
				}
	
				return maxSize;
			}

			/* Only readable, not writable */
			qint64 writeData(const char*, qint64) override {
				return -1;
			}

			std::shared_ptr<FramePipe> pipe;
			mutable std::mutex mutex;
			size_t byteOffset;
			uint64_t samples=0;
			int last =-1; 
			std::deque<std::pair<std::shared_ptr<AVFrame>, size_t>> frameDeque;
		};
		QAudioDevice device;
	};

}