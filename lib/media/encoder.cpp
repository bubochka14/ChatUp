#include "encoder.h"
using namespace Media;
Q_LOGGING_CATEGORY(LC_ENCODER, "Encoder");
AbstractEncoder::AbstractEncoder(const AVCodec* cdc)
	:_cdc(cdc)
	,_cCtx(nullptr)
	,_out(Media::createPacketPipe())
	,_pts(0)
	,_dts(0)
{

}
void AbstractEncoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
}
Video::Encoder::Encoder( Media::Video::SourceConfig config, const AVCodec* cdc)
    :AbstractEncoder(cdc)
	,_sws(nullptr)
	,_rescaledFrame(nullptr)
	,_config(std::move(config))
{
}
bool AbstractEncoder::isStarted()
{
	return _isStarted;
}
const AVCodec* AbstractEncoder::codec()
{
	return _cdc;
}
void AbstractEncoder::setCodecContext(std::shared_ptr<AVCodecContext> other)
{
	_cCtx = other;
}
bool AbstractEncoder::start(std::shared_ptr<Media::FramePipe> input)
{
	_isStarted = true;
	return true;
}
AbstractEncoder::~AbstractEncoder()
{
	close();
}
uint64_t AbstractEncoder::dts()
{
	return _dts;
}
uint64_t AbstractEncoder::pts()
{
	return _pts;
}
void AbstractEncoder::setPTS(uint64_t other)
{
	_dts = other;
}
void AbstractEncoder::setDTS(uint64_t other)
{
	_pts = other;
}

std::shared_ptr<Media::PacketPipe> AbstractEncoder::output()
{
    return _out;
}
void AbstractEncoder::close()
{
	//resets codecContext
	setCodecContext(nullptr);
	_isStarted = false;
}
std::shared_ptr<AVCodecContext> AbstractEncoder::codecContext()
{
	return _cCtx;
}
std::shared_ptr<Media::FramePipe> Video::Encoder::input()
{
	return _input;	
}
void Video::Encoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	AbstractEncoder::fillContext(ctx);
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	auto g = qScopeGuard([codecPar]() mutable {	avcodec_parameters_free(&codecPar); });
	AVPixelFormat requiredFormat = _config.format;
	if (!checkPixelFormat(requiredFormat))
	{
		qCDebug(LC_ENCODER) << "Pixel format" << av_get_pix_fmt_name(requiredFormat)
			<< "is not supported, switching to" << av_get_pix_fmt_name(codec()->pix_fmts[0]);
		requiredFormat = codec()->pix_fmts[0];
	}
	avcodec_parameters_from_context(codecPar, ctx.get());
	codecPar->width = _config.width;
	codecPar->height = _config.height;
	codecPar->format = requiredFormat;
	ctx->time_base = av_make_q(1, 30);
	avcodec_parameters_to_context(ctx.get(), codecPar);
}
Video::SourceConfig Video::Encoder::config()
{
	return _config;
}
bool Video::Encoder::start(std::shared_ptr<Media::FramePipe> input)
{
	if (isStarted())
		close();
	auto cCtx = Media::createCodecContext(codec());
	if (!cCtx)
	{
		qCCritical(LC_ENCODER) << "Cannot create codec context";
		return false;
	}
	int ret = 0;
	setCodecContext(cCtx);
	fillContext(cCtx);
	Video::SourceConfig sourceConfig = config();
	if (cCtx->pix_fmt != _config.format)
	{
		_sws = std::shared_ptr<SwsContext>(sws_getContext(sourceConfig.width,
			sourceConfig.height,
			sourceConfig.format,
			_config.width,
			_config.height,
			cCtx->pix_fmt,
			SWS_BILINEAR,
			nullptr, nullptr, nullptr), [](SwsContext* p) {sws_freeContext(p); });
		if (!_sws)
		{
			qCCritical(LC_ENCODER) << "Cannot alloc SwrContext";
			return false; 
		}
		_rescaledFrame = std::shared_ptr<AVFrame>(av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p); });
		if (!_rescaledFrame)
		{
			qCCritical(LC_ENCODER) << "Cannot alloc rescaledFrame";
			return false;
		}
		_rescaledFrame->width  = _config.width;
		_rescaledFrame->height = _config.height;
		_rescaledFrame->format = cCtx->pix_fmt;
		if ((ret = av_frame_get_buffer(_rescaledFrame.get(), 32)) < 0)
		{
			qCCritical(LC_ENCODER) << "Error while creating rescaled frame buffer:" << Media::av_err2string(ret);
			return false;
		}

	}
	if ((ret = avcodec_open2(cCtx.get(), codec(), nullptr)) < 0)
	{
		qCCritical(LC_ENCODER) << "Cannot open encoder" << Media::av_err2string(ret);
		return false;
	}
	
	_input = input;
	_listenerIndex = input->onDataChanged([this](std::shared_ptr<AVFrame> frame, size_t index) {
		auto cCtx = codecContext();
		if (!cCtx)
			return;
		int response;
		//if we should rescale frame
		if (_sws)
		{
			sws_scale(_sws.get(), (const uint8_t* const*)frame->data,
				frame->linesize, 0, frame->height, _rescaledFrame->data, _rescaledFrame->linesize);
			response = avcodec_send_frame(cCtx.get(), _rescaledFrame.get());
		}
		else
			response = avcodec_send_frame(cCtx.get(), frame.get());
		_input->unmapReading(index);
		if (response < 0)
			qCWarning(LC_ENCODER) << "Cannot send packet to encoder: " << Media::av_err2string(response);
		while (response >= 0 /*|| response == AVERROR(EAGAIN)*/) {
			auto outPacket = output()->tryHoldForWriting();
			if (!outPacket.has_value())
			{
				qCWarning(LC_ENCODER) << "Output pipe overflow";
				return;
			}
			response = avcodec_receive_packet(cCtx.get(), outPacket->ptr.get());
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				output()->unmapWriting(outPacket->subpipe, false);
				break;
			}
			else if (response < 0) {
				qCWarning(LC_ENCODER) << "Error while receiving packet from encoder:" << Media::av_err2string(response);
				output()->unmapWriting(outPacket->subpipe, false);
				break;
			}
			outPacket->ptr->stream_index = 0;
			setPTS(pts() + 1);
			setDTS(dts() + 1);
			outPacket->ptr->pts = pts();
			outPacket->ptr->dts = dts();
			outPacket->ptr->time_base.num = 1;
			outPacket->ptr->time_base.den = 30;
			output()->unmapWriting(outPacket->subpipe, true);
		}
	});
	return AbstractEncoder::start(input);
}
void Video::Encoder::close()
{
	if (_input && _listenerIndex.has_value())
	{
		_input->removeListener(_listenerIndex.value());
		_listenerIndex = std::nullopt;
	}
	AbstractEncoder::close();
}
Audio::Encoder::Encoder(SourceConfig config, const AVCodec* cdc)
	:AbstractEncoder(cdc)
	,_config(std::move(config))
{}
bool Audio::Encoder::checkSampleFormat(AVSampleFormat fmt)
{
	auto cdc = codec();
	if (!cdc)
		return false;
	const enum AVSampleFormat* p = cdc->sample_fmts;

	while (*p != AV_SAMPLE_FMT_NONE) {
		if (*p == fmt)
			return true;
		p++;
	}
	return false;
}
bool Audio::Encoder::checkSampleRate(int rate)
{
	auto cdc = codec();
	if (!cdc)
		return false;
	const int* p = cdc->supported_samplerates;

	while (*p) {
		if (*p == rate)
			return true;
		p++;
	}
	return false;
}
void Audio::Encoder::close()
{
	if (_input && _listenerIndex.has_value())
	{
		_input->removeListener(_listenerIndex.value());
		_listenerIndex = std::nullopt;
		_input.reset();
	}
	_samplesConverted = 0;
	AbstractEncoder::close();
}
bool Video::Encoder::checkPixelFormat(AVPixelFormat check)
{
	if (!codec())
		return false;
	for (auto fmt = codec()->pix_fmts; *fmt != -1; fmt++)
	{
		if ((AVPixelFormat)*fmt == check)
			return true;
	}
	return false;
}
void Audio::Encoder::fillContext(std::shared_ptr<AVCodecContext> ctx) {
	AbstractEncoder::fillContext(ctx);
	int requiredSampleRate = 48000;
	AVChannelLayout requiredLayout;
	av_channel_layout_from_string(&requiredLayout, "mono");
	AVSampleFormat requiredFormat = (AVSampleFormat)_config.par->format;
	if (!checkSampleFormat(requiredFormat))
	{
		qCDebug(LC_ENCODER) << "Pixel format" << av_get_sample_fmt_name(requiredFormat)
			<< "is not supported, switching to" << av_get_sample_fmt_name(codec()->sample_fmts[0]);
		requiredFormat = codec()->sample_fmts[0];
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, ctx.get());
	codecPar->ch_layout = requiredLayout;
	codecPar->sample_rate = requiredSampleRate;
	codecPar->format = requiredFormat;
	avcodec_parameters_to_context(ctx.get(), codecPar);

}
Audio::SourceConfig Audio::Encoder::config()
{
	return _config;
}
bool Audio::Encoder::encodeFrame(AVFrame* fr)
{
	int response = 0;
	auto ctx = codecContext();

	response = avcodec_send_frame(ctx.get(), fr);
	if (response < 0)
		qCWarning(LC_ENCODER) << "Cannot send packet to encoder: " << Media::av_err2string(response);
	while (response >= 0) {
		auto outPacket = output()->tryHoldForWriting();
		if (!outPacket.has_value())
		{
			qCWarning(LC_ENCODER) << "Output pipe overflow";
			return false;
		}
		response = avcodec_receive_packet(ctx.get(), outPacket->ptr.get());
		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
			output()->unmapWriting(outPacket->subpipe, false);
			break;
		}
		else if (response < 0) {
			printf("Error while receiving packet from encoder: %d", response);
			output()->unmapWriting(outPacket->subpipe, false);
			break;
		}
		setPTS(pts() + 1);
		setDTS(dts() + 1);

		outPacket->ptr->stream_index = 0;
		outPacket->ptr->pts = pts();
		outPacket->ptr->dts = dts();
		outPacket->ptr->time_base.num = 1;
		outPacket->ptr->time_base.den = 30;
		output()->unmapWriting(outPacket->subpipe, true);
	}
	return true;
}
bool Audio::Encoder::start(std::shared_ptr<Media::FramePipe> input)
{
	if (isStarted())
		close();
	auto ctx = Media::createCodecContext(codec());
	if (!ctx)
	{
		qCCritical(LC_ENCODER) << "Cannot create codec context";
		return false;
	}
	int ret = 0;
	setCodecContext(ctx);
	fillContext(ctx);
	SourceConfig sourceConfig = config();
	//if ((AVSampleFormat)sourceConfig.par->format != ctx->sample_fmt || sourceConfig.par->sample_rate != ctx->sample_rate)
	//{
		SwrContext* swr = nullptr;
		ret = swr_alloc_set_opts2(&swr,
			&ctx->ch_layout,
			ctx->sample_fmt,
			ctx->sample_rate,
			&sourceConfig.par->ch_layout,
			(AVSampleFormat)sourceConfig.par->format,
			sourceConfig.par->sample_rate,
			0, nullptr
		);
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
	//}
	ret = avcodec_open2(ctx.get(), codec(), nullptr);
	if (ret < 0) {
		qCCritical(LC_ENCODER) << "Cannot start encoder: " << Media::av_err2string(ret);
		return false;
	}
	AVFrame* chunk		= av_frame_alloc();
	chunk->ch_layout	= ctx->ch_layout;
	chunk->format		= ctx->sample_fmt;
	chunk->sample_rate	= ctx->sample_rate;
	chunk->nb_samples	= ctx->frame_size;
	if ((ret = av_frame_get_buffer(chunk, 0)) < 0)
	{
		qCCritical(LC_ENCODER) << "Cannot allocate chunk frame: " << Media::av_err2string(ret);
		return false;
	}
	_input = input;
	chunkOffset = 0;
	_listenerIndex= input->onDataChanged([this, input,chunk](std::shared_ptr<AVFrame> frame, size_t index) {
		std::shared_ptr<AVCodecContext> cCtx = AbstractEncoder::codecContext();
		if (!cCtx)
		{
			qCWarning(LC_ENCODER) << "Invalid codec context received";
			return;
		}
		int response = 0;
		int perSample = av_get_bytes_per_sample((AVSampleFormat)chunk->format);

		response = swr_convert(
			_swr.get(),
			chunk->extended_data,
			cCtx->frame_size - chunkOffset,
			frame->extended_data,
			frame->nb_samples
		);
		if (!chunkOffset)
			_chunkBegin = chunk->extended_data[0];
		_samplesConverted = std::min(cCtx->frame_size - chunkOffset, frame->nb_samples);
		chunkOffset += _samplesConverted;
		*chunk->extended_data += _samplesConverted * perSample;
		if (chunkOffset == cCtx->frame_size)
		{
			*chunk->extended_data = _chunkBegin;
			chunkOffset = 0;
			encodeFrame(chunk);
		}
		while(_samplesConverted < frame->nb_samples)
		{
			//if (_samplesConverted >= frame->nb_samples)
			//	break;
			response = swr_convert(
				_swr.get(),
				chunk->extended_data,
				cCtx->frame_size - chunkOffset,
				nullptr,
				frame->nb_samples - _samplesConverted
			);
			int converted = std::min(cCtx->frame_size - chunkOffset, frame->nb_samples- _samplesConverted);
			_samplesConverted += converted;
			chunkOffset += converted;
			*chunk->extended_data += converted * perSample;

			if (response < 0)
			{
				qCWarning(LC_ENCODER) << "Cannot convert audio frame:" << Media::av_err2string(response);
				break;
			}
			if (chunkOffset == cCtx->frame_size)
			{
				*chunk->extended_data = _chunkBegin;
				chunkOffset = 0;
				encodeFrame(chunk);
			}
		}
		input->unmapReading(index);




















		//*chunk->extended_data += _samplesConverted * perSample;
		//response = swr_convert(_swr.get(),
		//	chunk->extended_data,
		//	cCtx->frame_size-_samplesConverted* perSample,
		//	frame->data,
		//	frame->nb_samples
		//);
		//if(response <0)
		//{
		//	qCWarning(LC_ENCODER) << "Cannot convert audio frame:" << Media::av_err2string(response);
		//	return;
		//}
		//_samplesConverted += std::min(cCtx->frame_size, frame->nb_samples);
		//if (_samplesConverted < cCtx->frame_size);
		//	return;
		//	chunk->extended_data = _chunkBegin;
		//int samplesEncoded = std::min(cCtx->frame_size, frame->nb_samples);
		//int convertSamples = 0;
		//do
		//{
		//	response = avcodec_send_frame(cCtx.get(), chunk);
		//	samplesEncoded += convertSamples;

		//	if (response < 0)
		//		qCWarning(LC_ENCODER) << "Cannot send packet to encoder: " << Media::av_err2string(response);
		//	while (response >= 0) {
		//		auto outPacket = output()->tryHoldForWriting();
		//		if (!outPacket.has_value())
		//		{
		//			qCWarning(LC_ENCODER) << "Output pipe overflow";
		//			return;
		//		}
		//		response = avcodec_receive_packet(cCtx.get(), outPacket->ptr.get());
		//		if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
		//			output()->unmapWriting(outPacket->subpipe, false);
		//			break;
		//		}
		//		else if (response < 0) {
		//			printf("Error while receiving packet from encoder: %d", response);
		//			output()->unmapWriting(outPacket->subpipe, false);
		//			break;
		//		}
		//		setPTS(pts() + 1);
		//		setDTS(dts() + 1);

		//		outPacket->ptr->stream_index = 0;
		//		outPacket->ptr->pts = pts();
		//		outPacket->ptr->dts = dts();
		//		outPacket->ptr->time_base.num = 1;
		//		outPacket->ptr->time_base.den = 30;
		//		output()->unmapWriting(outPacket->subpipe, true);
		//	}
		//	convertSamples = std::min(cCtx->frame_size, frame->nb_samples - samplesEncoded);
		//	if(convertSamples)
		//	{
		//		response = swr_convert(_swr.get(), chunk->data, convertSamples, nullptr, 0);

		//		if (response < 0) {
		//			qCWarning(LC_ENCODER) << "Failed to convert audio frame:" << Media::av_err2string(response);
		//			input->unmapReading(index);//????
		//			return;
		//		}
		//	}
		//} while (samplesEncoded < frame->nb_samples);
		//	
	});
	return AbstractEncoder::start(input);
}
void Video::H264Encoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	Encoder::fillContext(ctx);
	av_opt_set(ctx->priv_data, "profile", "high422", 0); 
	av_opt_set(ctx->priv_data, "preset", "ultrafast", 0);// скорость кодирования. обратна пропорциональна качеству
	av_opt_set(ctx->priv_data, "tune", "zerolatency", 0);
}
Video::H264Encoder::H264Encoder(Media::Video::SourceConfig config)
	:Encoder(std::move(config), avcodec_find_encoder(AV_CODEC_ID_H264))
{
}
void Audio::AACEncoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	Encoder::fillContext(ctx);
	ctx->time_base = av_make_q(1, 30);
	ctx->bit_rate = 20 * 1000 * 1000;
	//ctx->rc_buffer_size = 4 * 1000 * 1000;
	//ctx->rc_max_rate = 5 * 1000 * 1000;
}
Audio::OpusEncoder::OpusEncoder(SourceConfig config)
	:Encoder(std::move(config),avcodec_find_encoder(AV_CODEC_ID_OPUS))
{

}
Audio::AACEncoder::AACEncoder(SourceConfig config)
	:Encoder(std::move(config),avcodec_find_decoder(AV_CODEC_ID_AAC))
{

}
void Audio::OpusEncoder::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	Encoder::fillContext(ctx);
	av_opt_set(ctx->priv_data, "application", "voip", 0);
	//av_opt_set(ctx->priv_data, "frame_duration", "60", 0);
	//ctx->bit_rate = 256000;
}