#pragma once
#include <media.h>
#include <qloggingcategory.h>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
Q_DECLARE_LOGGING_CATEGORY(LC_ABSTRACT_CODEC)
namespace Media {
	class CC_MEDIA_EXPORT AbstractCodec
	{
	public:
		AbstractCodec(const AVCodec* codec);
		bool initialize();
		bool isInitialized();
		virtual void reset();
		const AVCodec* codec();
		std::string codecName();
		virtual ~AbstractCodec();
	protected:
		std::shared_ptr<AVCodecContext> codecContext();
		virtual bool fillContext(std::shared_ptr<AVCodecContext> ctx);

	private:
		const AVCodec* _codec;
		std::shared_ptr<AVCodecContext> _cCtx;
		std::atomic<bool> _isInitialized = { false };
	};
}