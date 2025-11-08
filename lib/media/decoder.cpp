#include "decoder.h"
Q_LOGGING_CATEGORY(LC_DECODER, "Decoder");
using namespace Media;
AbstractDecoder::AbstractDecoder(const AVCodec* cdc)
	:AbstractCodec(cdc)
	,_out(Media::createFramePipe())
	,_drainFrame(av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p);})
{}
//void AbstractDecoder::initialize(std::shared_ptr<AVCodecContext> ctx, const AVCodec* codec, std::shared_ptr<FramePipe> out)
//{ 
//	_codec = codec;
//	_ctx = ctx;
//	_out = out;
//	_drainFrame->height = out->storedData(0)->height;
//	_drainFrame->width = out->storedData(0)->width;
//	_drainFrame->format = out->storedData(0)->format;
//	av_frame_get_buffer(_drainFrame.get(), 32);
//
//}
std::shared_ptr<FramePipe> AbstractDecoder::output()
{
	return _out;
}
bool AbstractDecoder::open(std::shared_ptr<PacketPipe> input)
{
	if (!AbstractCodec::initialize())
		return false;
	int ret = 0;
	if((ret = avcodec_open2(codecContext().get(), codec(), nullptr))<0)
	{
		qCWarning(LC_DECODER) << "Cannot open codec:" << Media::av_err2string(ret);
		return false;
	}
	_input = input;
	_pool.setMaxThreadCount(1);
	_inputListenIndex = input->onDataChanged([this](std::shared_ptr<AVPacket> pack, size_t index) {
		if (pack->pts - timestamp < 0)
			qCWarning(LC_DECODER) << codecName() << "NEGATIVE TIMESTAMP:" << pack->pts - timestamp << "PTS" << pack->pts << "DTS" << pack->dts;
		timestamp = pack->pts;
		QtConcurrent::run(& _pool,[pack,index,this](){
			std::lock_guard g(_decodeMutex);
			if(pack->pts - delta < 0)
			qCWarning(LC_DECODER) << codecName() << "NEGATIVE DELTA:" << pack->pts - delta << "PTS" << pack->pts << "DTS" << pack->dts;
			delta = pack->pts;
			if (!pack)
			{
				qCWarning(LC_DECODER) << codecName() << "empty packet received";
				_input->unmapReading(index);
				return;
			}
			int resp = avcodec_send_packet(codecContext().get(), pack.get());
			if (resp < 0)
			{
				qCDebug(LC_DECODER) << codecName() << "cannot send packet:" << av_err2str(resp);
				_input->unmapReading(index);
			}
			while (resp >= 0)
			{
				auto outFrame = _out->tryHoldForWriting();
				if(!outFrame.has_value())
				{
					qCWarning(LC_DECODER) << codecName() << "Output pipe overflow";
					resp = avcodec_receive_frame(codecContext().get(), _drainFrame.get());
					return;
				}

				resp = avcodec_receive_frame(codecContext().get(), outFrame->ptr.get());
				if (resp < 0)
				{
					_out->unmapWriting(outFrame->subpipe, false);
					if (resp != AVERROR(EAGAIN) && resp != AVERROR_EOF)
						qCWarning(LC_DECODER) << codecName() << "Error while sending a packet to the decoder:" << Media::av_err2string(resp);
					break;
				}
				else
				{
					_out->unmapWriting(outFrame->subpipe, true);
			/*		while (_holdingPackets.size())
					{
						_input->unmapReading(_holdingPackets.front());
						_holdingPackets.pop();
					}*/
				}
			}
			_input->unmapReading(index);

		});
	});
	return true;

}
void AbstractDecoder::close()
{
	std::lock_guard g(_decodeMutex);
	if (_inputListenIndex.has_value()&& _input)
	{
		_input->removeListener(_inputListenIndex.value());
		_input.reset();
	}
}
bool Video::Decoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	int ret = 0;
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	if ((ret = avcodec_parameters_from_context(codecPar, ctx.get())) < 0)
	{
		qCWarning(LC_DECODER) << Media::av_err2string(ret);
		return false;
	}
	codecPar->width  = _config.width;
	codecPar->height = _config.height;
	codecPar->format = _config.format;
	if ((ret = avcodec_parameters_to_context(ctx.get(), codecPar)) < 0)
	{
		qCWarning(LC_DECODER) << Media::av_err2string(ret);
		return false;
	}
	avcodec_parameters_free(&codecPar);
	return true;
}
Video::Decoder::Decoder(SourceConfig src)
	:AbstractDecoder(avcodec_find_decoder(src.codecID))
	,_config(std::move(src))
{}
std::optional<Audio::SourceConfig> Audio::Decoder::open(std::shared_ptr<PacketPipe> input)
{
	if (AbstractDecoder::open(input))
		return _config;
	else return std::nullopt;
}
std::optional<Video::SourceConfig> Video::Decoder::open(std::shared_ptr<PacketPipe> input)
{
	if (AbstractDecoder::open(input))
		return _config;
	else return std::nullopt;
}
bool Audio::Decoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	if (int ret = avcodec_parameters_to_context(ctx.get(), _config.par) < 0)
	{
		qCWarning(LC_DECODER) << codecName() << Media::av_err2string(ret);
		return false;
	}
	return true;
}
Audio::Decoder::Decoder(Audio::SourceConfig src)
	:AbstractDecoder(avcodec_find_decoder(src.par->codec_id))
	,_config(std::move(src))
{}
Audio::OpusDecoder::OpusDecoder()
	:AbstractDecoder(avcodec_find_decoder(AV_CODEC_ID_OPUS))
{}
Video::H264Decoder::H264Decoder()
	:AbstractDecoder(avcodec_find_decoder(AV_CODEC_ID_H264))

{}
bool Audio::OpusDecoder::open(std::shared_ptr<PacketPipe> input)
{
	return AbstractDecoder::open(input);
}
bool Video::H264Decoder::open(std::shared_ptr<PacketPipe> input)
{
	return AbstractDecoder::open(input);

}
bool Audio::OpusDecoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	AVChannelLayout requiredLayout;
	av_channel_layout_from_string(&ctx->ch_layout, "mono");
	return true;
}