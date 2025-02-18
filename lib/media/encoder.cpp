#include "encoder.h"
using namespace Media::Video;
Q_LOGGING_CATEGORY(LC_ENCODER, "Encoder");
AbstractEncoder::AbstractEncoder()
    :_out(Media::createPacketPipe())
	,_dts(0)
	,_pts(0)
{
}
std::shared_ptr<Media::PacketPipe> AbstractEncoder::output()
{
    return _out;
}
bool AbstractEncoder::start(std::shared_ptr<Media::FramePipe> input, SourceConfig source)
{
	_pts = 0;
	_dts = 0;
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, codecContext().get());
	codecPar->width = source.width;
	codecPar->height = source.height;
	codecPar->format = source.format;
	avcodec_parameters_to_context(codecContext().get(), codecPar);

	int ret = avcodec_open2(codecContext().get(), codec(), NULL);
	if (ret < 0) {
		qCCritical(LC_ENCODER) << "Cannot start encoder: " <<Media::av_err2string(ret);
		return false;
	}
    input->onDataChanged([this,wctx =std::weak_ptr(codecContext()), input](std::weak_ptr<AVFrame> weak,size_t index) {
        if (auto ptr = weak.lock())
        {
			auto ctx = wctx.lock();
			if (!ctx)
				return;
            int response = avcodec_send_frame(ctx.get(), input->storedData(index).get());
			input->unmapReading(index);
			if (response < 0)
				qCWarning(LC_ENCODER) << "Cannot send packet to encoder: " << Media::av_err2string(response);
            while (response >= 0) {
				auto outPacket = _out->holdForWriting();
                response = avcodec_receive_packet(ctx.get(), outPacket.ptr.get());
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
					_out->unmapWriting(index,false);
					break;
                }
                else if(response <0){
                    printf("Error while receiving packet from encoder: %d", response);
					_out->unmapWriting(index, false);
                    break;
                }

				outPacket.ptr->stream_index = 0;
				outPacket.ptr->pts = ++_pts;
				outPacket.ptr->dts = ++_dts;
				outPacket.ptr->time_base.num = 1;
				outPacket.ptr->time_base.den = 30;
				_out->unmapWriting(index, true);

            }
        }
        });
    return true;
}
H264Encoder::H264Encoder()
{
	_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!_codec)
	{
		qCCritical(LC_ENCODER) << "Cannot find h264 encoder";
		return;
	}
	_ctx = createCodecContext(_codec);
	if (_ctx == nullptr)
	{
		qCCritical(LC_ENCODER) << "Cannot create h264 codec context";
		return;
	}
	_ctx->max_b_frames = 0;            // не отправлять B фреймы
	_ctx->refs = 1;                    // количество кадров "ссылок"
	_ctx->level = 13;               // уровень качества

	av_opt_set(_ctx->priv_data, "profile", "high422", 0);
	av_opt_set(_ctx->priv_data, "preset", "fast", 0);           // скорость кодирования. обратна пропорциональна качеству
	av_opt_set(_ctx->priv_data, "tune", "zerolatency", 0);
	_ctx->time_base = av_make_q(1, 30);
	_ctx->gop_size = 10;
	_ctx->bit_rate = 5 * 1000 * 1000;
	_ctx->rc_buffer_size = 4 * 1000 * 1000;
	_ctx->rc_max_rate = 5 * 1000 * 1000;
}
std::shared_ptr<AVCodecContext> H264Encoder::codecContext()
{
	return _ctx;
}
const AVCodec* H264Encoder::codec()
{
	return _codec;
}