#pragma once
#include "media.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_H264DEMUXER)
namespace Media::Audio {
	class CC_MEDIA_EXPORT OpusDemuxer
	{
	public:
		OpusDemuxer();
		void start(std::shared_ptr<Media::RawPipe>);
		std::shared_ptr<Media::PacketPipe> output();
		struct ReadingOpaque
		{
			const uint8_t* data;
			size_t size = 0;
			size_t totalWritten = 0;
		};
	private:
		AVFormatContext* _ctx;
		std::shared_ptr<Media::PacketPipe> _out;
		ReadingOpaque _readingOpaque;

	};
}
namespace Media::Video
{
	class CC_MEDIA_EXPORT H264Demuxer
	{
	public:
		H264Demuxer();
		void start(std::shared_ptr<Media::RawPipe>);
		std::shared_ptr<Media::PacketPipe> output();
		struct ReadingOpaque
		{
			const  uint8_t* data;
			size_t size = 0;
			size_t totalWritten = 0;
		};
	private:
		AVFormatContext* _ctx;
		std::shared_ptr<Media::PacketPipe> _out;
		ReadingOpaque _readingOpaque;

	};
}