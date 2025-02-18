#include "decoder.h"
Q_LOGGING_CATEGORY(LC_DECODER, "Decoder");
Q_LOGGING_CATEGORY(LC_H264DEMUXER, "H264Demuxer");
using namespace Media;

void DataBuffer::pushData(Binary bin)
{
	data.emplace_back(std::move(bin));

}
int DataBuffer::popData(uint8_t*ptr, int size)
{
	int bytesPopped = 0;
	while (bytesPopped <size)
	{
		if (data.empty())
			break;
		if(size-bytesPopped>= data.front().size()- byteOffset)
		{
			memcpy(ptr, data.front().data()+ byteOffset, data.front().size()- byteOffset);
			bytesPopped += data.front().size() - byteOffset;
			data.pop_front();
			byteOffset = 0;
		}
		else
		{
			memcpy(ptr + bytesPopped, data.front().data() + byteOffset, size - bytesPopped);
			byteOffset += size - bytesPopped;
			bytesPopped = size;
		}
	}
	return bytesPopped;

}
bool DataBuffer::empty() 
{
	return data.empty();
}

static int avio_read(void* opaque, uint8_t*buf, int size)
{
	auto deq = (DataBuffer*)opaque;
	if (deq->empty())
	{
		return AVERROR_EOF;
	}
	return deq->popData(buf,size);
}
std::shared_ptr<FramePipe> AbstractDecoder::output()
{
	return out;
}

AbstractDecoder::AbstractDecoder()
	:out(std::make_shared<FramePipe>(false))
	,_codec(nullptr)
	,_ctx(nullptr)


{

}
void AbstractDecoder::initialize(std::shared_ptr<AVCodecContext> ctx, const AVCodec* codec)
{ 
	_codec = codec;
	_ctx = std::move(ctx);
	int height = _ctx->height;
	int width = _ctx->width;
	int fmt = _ctx->pix_fmt;

	out->reset([height, width, fmt]() {
		auto frame = av_frame_alloc();
		frame->height = height;
		frame->width = width;
		frame->format = fmt;
		av_frame_get_buffer(frame, 32);
		return frame;
		}, [](AVFrame* p) {
			av_frame_free(&p);
		});
}

bool AbstractDecoder::start(std::shared_ptr<PacketPipe> input)
{
	if (!_codec || !_ctx)
	{
		qCWarning(LC_DECODER) << "Not initialized";
		return false;
	}
	input->onDataChanged([this, wInput = std::weak_ptr(input)](std::shared_ptr<AVPacket> pack, size_t index) {
		quene.enqueue([this,wInput, wPack = std::weak_ptr(pack),index]() {
			auto input = wInput.lock();
			auto pack = wPack.lock();
			if (!input || !pack)
				return;
			int resp = avcodec_send_packet(_ctx.get(), pack.get());
			input->unmapReading(index);
			if (resp < 0)
				qCDebug(LC_DECODER) << "cannot send packet " << av_err2str(resp);
			while (resp >= 0)
			{
				auto outFrame = out->holdForWriting();
				resp = avcodec_receive_frame(_ctx.get(), outFrame.ptr.get());
				if (resp < 0)
				{
					out->unmapWriting(outFrame.subpipe, false);
					if (resp != AVERROR(EAGAIN) && resp != AVERROR_EOF)
						qCWarning(LC_DECODER) << "Error while sending a packet to the decoder:" << Media::av_err2string(resp);
					break;
				}
				else
				{
					out->unmapWriting(outFrame.subpipe,true);
				}
			}
			});
		});
	return true;

}
void AbstractDecoder::stop()
{
	
	if (inputListenIndex.has_value())
	{
		input->removeListener(inputListenIndex.value());
		input.reset();
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
	int ret = avcodec_open2(cCtx.get(), cdc, nullptr);
	if (ret < 0)
	{
		qCWarning(LC_DECODER) << "Cannot open codec:" << Media::av_err2string(ret);
		return;
	}
	initialize(std::move(cCtx), cdc);
}
Audio::Decoder::Decoder(const Audio::Source& src)
{
	//cdc = avcodec_find_decoder(src.codecID);
	//if (!cdc)
	//{
	//	qCCritical(LC_DECODER) << "Cannot find decoder with id" << src.codecID;
	//	cCtx = nullptr;
	//	return;
	//}
	//cCtx = createCodecContext(cdc);
	//if (cCtx == nullptr)
	//{
	//	qCWarning(LC_DECODER) << "Cannot create codec context";
	//	return;
	//}
	//AVCodecParameters* codecPar = avcodec_parameters_alloc();
	//////cCtx->request_sample_fmt = av_get_alt_sample_fmt(cCtx->sample_fmt, 0);
	////cCtx->ch_layout. = av_get_default_channel_layout(src.channelCount);
	////cCtx-> = src.channelCount;
	////cCtx->sample_fmt = src.format;

	//avcodec_parameters_to_context(cCtx.get(), src.par);

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
	initialize(std::move(cCtx), cdc);
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
