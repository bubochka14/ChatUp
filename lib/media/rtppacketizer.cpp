#include "rtppacketizer.h"
using namespace Media;
Q_LOGGING_CATEGORY(LC_RTP_PACKETIZER, "RtpPacketizer");
static int write_packaged(void* op,const uint8_t* buf, int size)
{
	RtpPacketizer* pipeline = (RtpPacketizer*)op;
	std::shared_ptr<Media::RawPipe> out = pipeline->output();
	std::optional<Media::RawPipe::PipeData> raw = out->tryHoldForWriting();
	if (raw.has_value())
	{
		raw->ptr->raw = buf;
		raw->ptr->size = size;
		out->unmapWriting(raw->subpipe, true);
	}
	else
		qCWarning(LC_RTP_PACKETIZER) << "Output pipe overflow";
	return 0;
}
RtpPacketizer::RtpPacketizer(PacketizationConfig config)
	:_output(std::make_shared<RawPipe>())
	, _packetizationCxt(avformat_alloc_context())
	, _config(std::move(config))
{
	if(!_config.ecnCtx) {
		qCCritical(LC_RTP_PACKETIZER) << "Config does not contain encoder context";
		return;
	}

	int ret = avformat_alloc_output_context2(&_packetizationCxt, nullptr, "rtp", 0);
	if (!_packetizationCxt || ret < 0) {
		qCCritical(LC_RTP_PACKETIZER) << "Cannot create output context:" << Media::av_err2string(ret);
		return;
	}
	uint8_t* avio_ctx_buffer = (uint8_t*)av_malloc(_config.bufferSize);
	if (!avio_ctx_buffer) {
		qCCritical(LC_RTP_PACKETIZER) << "Cannot alloc buffer";
		return;
	}
	AVIOContext* avio_ctx = avio_alloc_context(avio_ctx_buffer, _config.bufferSize,
		1, this, 0, write_packaged, NULL);

	if (!avio_ctx)
	{
		qCCritical(LC_RTP_PACKETIZER) << "Cannot alloc avio context";
		return;
	}
	avio_ctx->max_packet_size = _config.maxPacketSize;
	_packetizationCxt->pb = avio_ctx;
	AVStream* out_stream = avformat_new_stream(_packetizationCxt, NULL);
	if (!out_stream)
	{
		qCCritical(LC_RTP_PACKETIZER) << "Cannot create output stream";
		return;
	}
	ret = avcodec_parameters_from_context(out_stream->codecpar, _config.ecnCtx.get());
	if (ret < 0)
	{
		qCCritical(LC_RTP_PACKETIZER) << "Cannot initialize output stream:" << Media::av_err2string(ret);
	}
	out_stream->time_base = _config.ecnCtx->time_base;

}
std::shared_ptr<RawPipe> RtpPacketizer::output()
{
	return _output;
}
bool RtpPacketizer::start(std::shared_ptr<PacketPipe> input)
{
	int ret = avformat_write_header(_packetizationCxt, NULL);
	if (ret < 0) {
		qCCritical(LC_RTP_PACKETIZER) << "Error occurred when starting packetizer" << Media::av_err2string(ret);
		return false;
	}
	input->onDataChanged([this, winput = std::weak_ptr(input)](std::weak_ptr<AVPacket> wp, int index) {
		auto pack = wp.lock();
		auto input = winput.lock();
		if (!pack || !input)
			return;		
		//av_packet_rescale_ts(pack.get(),
		//	_config.ecnCtx->time_base,
		//	_packetizationCxt->streams[0]->time_base);
		int ret = av_interleaved_write_frame(_packetizationCxt, pack.get());
		if (ret < 0)
			qCWarning(LC_RTP_PACKETIZER) << "Cannot write packet:" << Media::av_err2string(ret);
		input->unmapReading(index);
		});
	return true;
}
