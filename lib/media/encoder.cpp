#include "encoder.h"
using namespace Media;
Q_LOGGING_CATEGORY(LC_ENCODER, "Encoder");
Video::AbstractEncoder::AbstractEncoder()
    :_out(Media::createPacketPipe())
	,_dts(0)
	,_pts(0)
	,_cdc(nullptr)
	,_cCtx(nullptr)
{
}
std::shared_ptr<Media::PacketPipe> Video::AbstractEncoder::output()
{
    return _out;
}
std::shared_ptr<AVCodecContext> Video::AbstractEncoder::codecContext()
{
	return _cCtx;
}
void Video::AbstractEncoder::initialize(std::shared_ptr<AVCodecContext> cctx, const AVCodec* cdc)
{
	_cCtx = cctx;
	_cdc = cdc;
}

bool Video::AbstractEncoder::start(std::shared_ptr<Media::FramePipe> input)
{
	_pts = 0;
	_dts = 0;
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, _cCtx.get());
	codecPar->width = input->storedData(0)->width;
	codecPar->height = input->storedData(0)->height;
	codecPar->format = input->storedData(0)->format;
	avcodec_parameters_to_context(_cCtx.get(), codecPar);

	int ret = avcodec_open2(_cCtx.get(), _cdc, NULL);
	if (ret < 0) {
		qCCritical(LC_ENCODER) << "Cannot start encoder: " <<Media::av_err2string(ret);
		return false;
	}
    input->onDataChanged([this,wctx =std::weak_ptr(_cCtx), input](std::weak_ptr<AVFrame> weak,size_t index) {
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
				auto outPacket = _out->tryHoldForWriting();
				if (!outPacket.has_value())
				{
					qCWarning(LC_ENCODER) << "Output pipe overflow";
					return;
				}
                response = avcodec_receive_packet(ctx.get(), outPacket->ptr.get());
                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
					_out->unmapWriting(index,false);
					break;
                }
                else if(response <0){
                    printf("Error while receiving packet from encoder: %d", response);
					_out->unmapWriting(index, false);
                    break;
                }
				outPacket->ptr->stream_index = 0;
				outPacket->ptr->pts = ++_pts;
				outPacket->ptr->dts = ++_dts;
				outPacket->ptr->time_base.num = 1;
				outPacket->ptr->time_base.den = 30;
				_out->unmapWriting(index, true);

            }
        }
        });
    return true;
}
Audio::AbstractEncoder::AbstractEncoder()
	:_out(Media::createPacketPipe())
	,_dts(0)
	,_pts(0)
	,_cdc(nullptr)
	,_cCtx(nullptr)
{
}
std::shared_ptr<Media::PacketPipe> Audio::AbstractEncoder::output()
{
	return _out;
}
std::shared_ptr<AVCodecContext> Audio::AbstractEncoder::codecContext()
{
	return _cCtx;
}
void Audio::AbstractEncoder::initialize(std::shared_ptr<AVCodecContext> cctx, const AVCodec* cdc)
{
	_cCtx = cctx;
	_cdc = cdc;
}
bool Audio::AbstractEncoder::checkSampleFormat(AVSampleFormat fmt)
{
	if (!_cdc)
		return false;
	const enum AVSampleFormat* p = _cdc->sample_fmts;

	while (*p != AV_SAMPLE_FMT_NONE) {
		if (*p == fmt)
			return true;
		p++;
	}
	return false;
}
bool Audio::AbstractEncoder::checkSampleRate(int rate)
{
	if (!_cdc)
		return false;
	const int* p = _cdc->supported_samplerates;

	while (*p) {
		if (*p == rate)
			return true;
		p++;
	}
	return false;
}
AVFrame makeFrameChunk(std::shared_ptr<AVFrame> src, size_t offset, size_t samples)
{
	AVFrame out;
	out.nb_samples;
	return out;
}
bool Audio::AbstractEncoder::start(std::shared_ptr<Media::FramePipe> input)
{
	_pts = 0;
	_dts = 0;
	//viewing frame properties, not data
	std::shared_ptr<AVFrame> frame = input->storedData(0);
	int requiredSampleRate = frame->sample_rate;
	AVChannelLayout requiredLayout;
	av_channel_layout_from_string(&requiredLayout, "stereo");
	AVSampleFormat requiredFormat = (AVSampleFormat)frame->format;
	if (!checkSampleRate(frame->sample_rate))
		requiredSampleRate = _cdc->supported_samplerates[0];
	if (!checkSampleFormat((AVSampleFormat)frame->format))
		requiredFormat = _cdc->sample_fmts[0];			
	if ((AVSampleFormat)frame->format != requiredFormat || frame->sample_rate != requiredSampleRate)
	{
		SwrContext* swr = nullptr;
		int ret = 0;
		AVAudioFifo* fifo;
		if (!(fifo = av_audio_fifo_alloc(requiredFormat,requiredLayout.nb_channels, requiredSampleRate)))
		{
			qCCritical(LC_ENCODER) << "Cannot alloc convert buffer:" << Media::av_err2string(ret);
			return false;
		}
		swr_alloc_set_opts2(&swr, &requiredLayout, requiredFormat, requiredSampleRate,
			&frame->ch_layout, (AVSampleFormat)frame->format,
			frame->sample_rate, 0, nullptr);
		_fifo = std::shared_ptr<AVAudioFifo>(fifo, [](AVAudioFifo* p) {av_audio_fifo_free(p); });
		if (ret < 0)
		{
			qCCritical(LC_ENCODER) << "Cannot alloc SwrContext:" << Media::av_err2string(ret);
			return false;
		}
		ret = swr_init(swr);
		if (ret < 0)
		{
			qCCritical(LC_ENCODER) << "Cannot init SwrContext:" << Media::av_err2string(ret);
			swr_free(&swr);
			return false;
		}
		_swr = std::shared_ptr<SwrContext>(swr, [](SwrContext* p) {swr_free(&p); });
		//_swrBuffer = (float*)av_malloc(4096 * sizeof(float));
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, _cCtx.get());
	codecPar->ch_layout = requiredLayout;
	codecPar->sample_rate = requiredSampleRate;
	codecPar->format = requiredFormat;
	avcodec_parameters_to_context(_cCtx.get(), codecPar);

	int ret = avcodec_open2(_cCtx.get(), _cdc, NULL);
	if (ret < 0) {
		qCCritical(LC_ENCODER) << "Cannot start encoder: " << Media::av_err2string(ret);
		return false;
	}
	AVFrame* chunk = av_frame_alloc();
	chunk->ch_layout = requiredLayout;
	chunk->format = requiredFormat;
	chunk->sample_rate = requiredSampleRate;
	chunk->nb_samples = _cCtx->frame_size;
	if ((ret = av_frame_get_buffer(chunk, 0)) < 0)
	{
		qCCritical(LC_ENCODER) << "Cannot allocate chunk frame: " << Media::av_err2string(ret);
		return false;
	}
	input->onDataChanged([this, input,chunk](std::shared_ptr<AVFrame> frame, size_t index) {
		queue.enqueue([this, input, chunk,frame,index]() {
			int response = 0;
			response = swr_convert(_swr.get(), chunk->extended_data, _cCtx->frame_size,
				frame->extended_data, frame->nb_samples);
			int samplesEncoded = _cCtx->frame_size;
			int convertSamples = 0;
			while (samplesEncoded < frame->nb_samples)
			{
				response = avcodec_send_frame(_cCtx.get(), chunk);
				input->unmapReading(index);
				if (response < 0)
					qCWarning(LC_ENCODER) << "Cannot send packet to encoder: " << Media::av_err2string(response);
				while (response >= 0) {
					auto outPacket = _out->holdForWriting();
					response = avcodec_receive_packet(_cCtx.get(), outPacket.ptr.get());
					if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
						_out->unmapWriting(index, false);
						break;
					}
					else if (response < 0) {
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
				convertSamples = std::min(_cCtx->frame_size, frame->nb_samples - samplesEncoded);
				response = swr_convert(_swr.get(), chunk->extended_data, convertSamples, nullptr, 0);
				samplesEncoded += _cCtx->frame_size;
				if (response < 0) {
					qCWarning(LC_ENCODER) << "Failed to convert audio frame:" << Media::av_err2string(response);
					input->unmapReading(index);
					return;
				}
			}
			});
		});
	return true;
}
Video::H264Encoder::H264Encoder()
{
	const AVCodec* cdc = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!cdc)
	{
		qCCritical(LC_ENCODER) << "Cannot find h264 encoder";
		return;
	}
	auto ctx = createCodecContext(cdc);
	if (!ctx)
	{
		qCCritical(LC_ENCODER) << "Cannot create h264 codec context";
		return;
	}
	ctx->max_b_frames = 0;            // не отправлять B фреймы
	ctx->refs = 1;                    // количество кадров "ссылок"
	ctx->level = 13;               // уровень качества

	av_opt_set(ctx->priv_data, "profile", "high422", 0);
	av_opt_set(ctx->priv_data, "preset", "fast", 0);           // скорость кодирования. обратна пропорциональна качеству
	av_opt_set(ctx->priv_data, "tune", "zerolatency", 0);
	ctx->time_base = av_make_q(1, 30);
	ctx->gop_size = 10;
	ctx->bit_rate = 5 * 1000 * 1000;
	ctx->rc_buffer_size = 4 * 1000 * 1000;
	ctx->rc_max_rate = 5 * 1000 * 1000;
	initialize(ctx, cdc);
}
Audio::AACEncoder::AACEncoder()
{
	const AVCodec* cdc = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!cdc)
	{
		qCCritical(LC_ENCODER) << "Cannot find AAC encoder";
		return;
	}
	auto ctx = createCodecContext(cdc);
	if (!ctx)
	{
		qCCritical(LC_ENCODER) << "Cannot create AAC codec context";
		return;
	}
	ctx->time_base = av_make_q(1, 30);
	ctx->bit_rate = 5 * 1000 * 1000;
	ctx->rc_buffer_size = 4 * 1000 * 1000;
	ctx->rc_max_rate = 5 * 1000 * 1000;
	initialize(ctx, cdc);
}
Audio::OpusEncoder::OpusEncoder()
{
	const AVCodec* cdc = avcodec_find_encoder(AV_CODEC_ID_OPUS);
	if (!cdc)
	{
		qCCritical(LC_ENCODER) << "Cannot find Opus encoder";
		return;
	}
	auto ctx = createCodecContext(cdc);
	if (!ctx)
	{
		qCCritical(LC_ENCODER) << "Cannot create Opus codec context";
		return;
	}
	ctx->time_base = av_make_q(1, 30);
	ctx->bit_rate = 80000;
	ctx->rc_buffer_size = 4 * 1000 * 1000;
	ctx->rc_max_rate = 256000;
	initialize(ctx, cdc);
}