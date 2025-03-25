#include "decoder.h"
Q_LOGGING_CATEGORY(LC_DECODER, "Decoder");
Q_LOGGING_CATEGORY(LC_H264DEMUXER, "H264Demuxer");
using namespace Media;
AbstractDecoder::AbstractDecoder()
	:_out(nullptr)
	,_codec(nullptr)
	,_ctx(nullptr)
	,_drainFrame(av_frame_alloc(), [](AVFrame* p) {av_frame_free(&p);})

{

}
void AbstractDecoder::initialize(std::shared_ptr<AVCodecContext> ctx, const AVCodec* codec, std::shared_ptr<FramePipe> out)
{ 
	_codec = codec;
	_ctx = ctx;
	_out = out;
	_drainFrame->height = out->storedData(0)->height;
	_drainFrame->width = out->storedData(0)->width;
	_drainFrame->format = out->storedData(0)->format;
	av_frame_get_buffer(_drainFrame.get(), 32);

}
std::shared_ptr<FramePipe> AbstractDecoder::output()
{
	return _out;
}
bool AbstractDecoder::start(std::shared_ptr<PacketPipe> input)
{
	if (!_codec || !_ctx || !_out)
	{
		qCWarning(LC_DECODER) << "Not initialized";
		return false;
	}
	_input = input;
	_pool.setMaxThreadCount(1);
	
	inputListenIndex = input->onDataChanged([this](std::shared_ptr<AVPacket> pack, size_t index) {
		QtConcurrent::run(& _pool,[pack,index,this](){
		std::lock_guard g(_decodeMutex);
		if(pack->pts - delta < 0)
		qDebug() <<avcodec_get_name(_codec->id) << "NEGATIVE DELTA:" << pack->pts - delta << "PTS" << pack->pts << "DTS" << pack->dts;
		delta = pack->pts;
		//quene.enqueue([this,pack,index]() {
			if (!pack)
			{
				_input->unmapReading(index);
				return;
			}
			int resp = avcodec_send_packet(_ctx.get(), pack.get());
			_input->unmapReading(index);
			if (resp < 0)
				qCDebug(LC_DECODER) << avcodec_get_name(_codec->id) << "cannot send packet" << av_err2str(resp);
			while (resp >= 0)
			{
				auto outFrame = _out->holdForWriting();
	/*			if (!outFrame)
				{
					qCWarning(LC_DECODER) << avcodec_get_name(_codec->id) << "Output pipe overflow";
					resp = avcodec_receive_frame(_ctx.get(), _drainFrame.get());

					return;
				}*/
			//	qCDebug(LC_DECODER) << "Writing to" << outFrame.subpipe;
				resp = avcodec_receive_frame(_ctx.get(), outFrame.ptr.get());
				if (resp < 0)
				{
				//	qCDebug(LC_DECODER) << "Finish writing widthout notify" << outFrame.subpipe;
					_out->unmapWriting(outFrame.subpipe, false);
					if (resp != AVERROR(EAGAIN) && resp != AVERROR_EOF)
						qCWarning(LC_DECODER) << "Error while sending a packet to the decoder:" << Media::av_err2string(resp);
					break;
				}
				else
				{
				//	qCDebug(LC_DECODER) << "Finish writing width notify" << outFrame.subpipe;
					_out->unmapWriting(outFrame.subpipe, true);
				}
			}
			});
			});
	return true;

}
void AbstractDecoder::stop()
{
	
	if (inputListenIndex.has_value()&& _input)
	{
		_input->removeListener(inputListenIndex.value());
		_input.reset();
	}
}
AbstractDecoder::~AbstractDecoder()
{
	stop();
}
Video::Decoder::Decoder(const Video::SourceConfig& src)
{

	const AVCodec* cdc = avcodec_find_decoder(src.codecID);
	if (!cdc)
	{
		qCCritical(LC_DECODER) << "Cannot find decoder with id" << src.codecID;
		return;
	}
	std::shared_ptr<AVCodecContext> cCtx = createCodecContext(cdc);
	if (!cCtx)
	{
		qCWarning(LC_DECODER) << "Cannot create codec context";
		return;
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, cCtx.get());
	codecPar->width	 = src.width;
	codecPar->height = src.height;
	codecPar->format = src.format;
	avcodec_parameters_to_context(cCtx.get(), codecPar);
	avcodec_parameters_free(&codecPar);
	int ret = avcodec_open2(cCtx.get(), cdc, nullptr);
	if (ret < 0)
	{
		qCWarning(LC_DECODER) << "Cannot open codec:" << Media::av_err2string(ret);
		return;
	}
	initialize(std::move(cCtx), cdc, createFramePipe(cCtx->width, cCtx->height, cCtx->pix_fmt));
}
Audio::Decoder::Decoder(const Audio::SourceConfig& src)
{
	const AVCodec* cdc = avcodec_find_decoder(src.par->codec_id);
	if (!cdc)
	{
		qCCritical(LC_DECODER) << "Cannot find audio decoder with id" << src.par->codec_id;
		return;
	}
	std::shared_ptr<AVCodecContext> cCtx = createCodecContext(cdc);
	if (!cCtx)
	{
		qCWarning(LC_DECODER) << "Cannot create audio codec context";
		return;
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_to_context(cCtx.get(), src.par);
	int ret = avcodec_open2(cCtx.get(), cdc, nullptr);
	if (ret < 0)
	{
		qCWarning(LC_DECODER) << "Cannot open codec:" << Media::av_err2string(ret);
		return;
	}
	avcodec_parameters_free(&codecPar);

	initialize(std::move(cCtx), cdc, createFramePipe(cCtx->ch_layout, cCtx->sample_fmt, cCtx->sample_rate));

}
Audio::OpusDecoder::OpusDecoder()
{
	const AVCodec* cdc = avcodec_find_decoder(AV_CODEC_ID_OPUS);
	if (!cdc)
	{
		qCCritical(LC_DECODER) << "Cannot find audio decoder with id" << AV_CODEC_ID_OPUS;
		return;
	}
	std::shared_ptr<AVCodecContext> cCtx = createCodecContext(cdc);
	if (!cCtx)
	{
		qCWarning(LC_DECODER) << "Cannot create audio codec context";
		return;
	}
	int ret = avcodec_open2(cCtx.get(), cdc, nullptr);
	if (ret < 0)
	{
		qCWarning(LC_DECODER) << "Cannot open codec:" << Media::av_err2string(ret);
		return;
	}
	initialize(std::move(cCtx), cdc, createFramePipe(cCtx->ch_layout, cCtx->sample_fmt, cCtx->sample_rate));

}
Video::H264Decoder::H264Decoder()
{
	const AVCodec* cdc = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!cdc)
	{
		qCCritical(LC_DECODER) << "Cannot find H264 decoder";
		return;
	}
	std::shared_ptr<AVCodecContext>cCtx = createCodecContext(cdc);
	if (!cCtx)
	{
		qCWarning(LC_DECODER) << "Cannot create H264 codec context";
		return;
	}
	int ret = avcodec_open2(cCtx.get(), cdc, nullptr);
	if (ret < 0)
	{
		qCWarning(LC_DECODER) << "Cannot open H264 codec:" << Media::av_err2string(ret);
		return;
	}
	initialize(std::move(cCtx), cdc, createFramePipe(cCtx->width, cCtx->height, cCtx->pix_fmt));
}
static int h264_read(void* opaque, uint8_t* buf, int size) noexcept
{
	using namespace Video;
	H264Demuxer::ReadingOpaque* ro = (H264Demuxer::ReadingOpaque*)opaque;
	if (ro->totalWritten == ro->size)
		return AVERROR_EOF;
	if (ro->size-ro->totalWritten >= size)
	{
		memcpy(buf, ro->data + ro->totalWritten, size);
			ro->totalWritten += size;
		return size;
	}
	else
	{
		memcpy(buf, ro->data + ro->totalWritten, ro->size - ro->totalWritten);
		int written = ro->size - ro->totalWritten;
		ro->totalWritten = ro->size;
		return written;
	}
}
Video::H264Demuxer::H264Demuxer()
	:_ctx(avformat_alloc_context())
	,_out(Media::createPacketPipe())
{
	if(!_ctx) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc input context";
		return;
	}

	const AVInputFormat* infmt = av_find_input_format("h264");

	uint8_t* avio_ctx_buffer = (uint8_t*)av_malloc(25000);
	if (!avio_ctx_buffer) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc buffer";
		return;
	}
	AVIOContext* avio_ctx = avio_alloc_context(avio_ctx_buffer,25000,
		0, &_readingOpaque, h264_read, NULL, NULL);

	if (!avio_ctx)
	{
		qCCritical(LC_H264DEMUXER) << "Cannot alloc avio context";
		return;
	}
	avio_ctx->max_packet_size = 25000;
	_ctx->pb = avio_ctx;
	int ret = avformat_open_input(&_ctx, "test", infmt, 0);
	if (!_ctx || ret < 0) {
		qCCritical(LC_H264DEMUXER) << "Cannot open input context:" << Media::av_err2string(ret);
		return;
	}
}
void Video::H264Demuxer::start(std::shared_ptr<Media::RawPipe> in)
{
	in->onDataChanged([this, wInput = std::weak_ptr<RawPipe>(in)](std::weak_ptr<Raw> wRaw, size_t index) {
		auto raw = wRaw.lock();
		auto input = wInput.lock();
		if (!raw || !input)
			return;
		_readingOpaque.data = raw->raw;
		_readingOpaque.size = raw->size;
		_readingOpaque.totalWritten = 0;
		auto packet = _out->holdForWriting();
		av_packet_unref(packet.ptr.get());
		do {
			int ret = av_read_frame(_ctx, packet.ptr.get());
			input->unmapReading(index);
			if (ret < 0)
			{
				_out->unmapWriting(packet.subpipe, false);
				qCWarning(LC_H264DEMUXER) << "read packet error" << Media::av_err2string(ret);
				return;
			}
		} while (_readingOpaque.totalWritten != _readingOpaque.size);
		_out->unmapWriting(packet.subpipe, true);

		});
}
std::shared_ptr<Media::PacketPipe> Video::H264Demuxer::output()
{
	return _out;
}
static int opus_read(void* opaque, uint8_t* buf, int size) noexcept
{
	using namespace Video;
	Audio::OpusDemuxer::ReadingOpaque* ro = (Audio::OpusDemuxer::ReadingOpaque*)opaque;
	if (ro->totalWritten == ro->size)
		return AVERROR_EOF;
	if (ro->size - ro->totalWritten >= size)
	{
		memcpy(buf, ro->data + ro->totalWritten, size);
		ro->totalWritten += size;
		return size;
	}
	else
	{
		memcpy(buf, ro->data + ro->totalWritten, ro->size - ro->totalWritten);
		int written = ro->size - ro->totalWritten;
		ro->totalWritten = ro->size;
		return written;
	}
}
Audio::OpusDemuxer::OpusDemuxer()
	:_ctx(avformat_alloc_context())
	, _out(Media::createPacketPipe())
{
	if (!_ctx) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc input context";
		return;
	}

	const AVInputFormat* infmt = av_find_input_format("opus");

	uint8_t* avio_ctx_buffer = (uint8_t*)av_malloc(25000);
	if (!avio_ctx_buffer) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc buffer";
		return;
	}
	AVIOContext* avio_ctx = avio_alloc_context(avio_ctx_buffer, 25000,
		0, &_readingOpaque, opus_read, NULL, NULL);

	if (!avio_ctx)
	{
		qCCritical(LC_H264DEMUXER) << "Cannot alloc avio context";
		return;
	}
	avio_ctx->max_packet_size = 25000;
	_ctx->pb = avio_ctx;
	int ret = avformat_open_input(&_ctx, "test", infmt, 0);
	if (!_ctx || ret < 0) {
		qCCritical(LC_H264DEMUXER) << "Cannot open input context:" << Media::av_err2string(ret);
		return;
	}
}
void Audio::OpusDemuxer::start(std::shared_ptr<Media::RawPipe> in)
{
	in->onDataChanged([this, wInput = std::weak_ptr<RawPipe>(in)](std::weak_ptr<Raw> wRaw, size_t index) {
		auto raw = wRaw.lock();
		auto input = wInput.lock();
		if (!raw || !input)
			return;
		_readingOpaque.data = raw->raw;
		_readingOpaque.size = raw->size;
		_readingOpaque.totalWritten = 0;
		auto packet = _out->holdForWriting();
		av_packet_unref(packet.ptr.get());
		do {
			int ret = av_read_frame(_ctx, packet.ptr.get());
			input->unmapReading(index);
			if (ret < 0)
			{
				_out->unmapWriting(packet.subpipe, false);
				qCWarning(LC_H264DEMUXER) << "read packet error" << Media::av_err2string(ret);
				return;
			}
		} while (_readingOpaque.totalWritten != _readingOpaque.size);
		_out->unmapWriting(packet.subpipe, true);

		});
}
std::shared_ptr<Media::PacketPipe> Audio::OpusDemuxer::output()
{
	return _out;
}
