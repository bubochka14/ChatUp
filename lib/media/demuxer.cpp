#include "demuxer.h"
Q_LOGGING_CATEGORY(LC_H264DEMUXER, "H264Demuxer");

using namespace chatup;
static int h264_read(void* opaque, uint8_t* buf, int size) noexcept
{
	using namespace Video;
	H264Demuxer::ReadingOpaque* ro = (H264Demuxer::ReadingOpaque*)opaque;
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
H264Demuxer::H264Demuxer()
	:_ctx(avformat_alloc_context())
	, _out(createPacketPipe())
{
	if (!_ctx) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc input context";
		return;
	}

	const AVInputFormat* infmt = av_find_input_format("h264");

	uint8_t* avio_ctx_buffer = (uint8_t*)av_malloc(25000);
	if (!avio_ctx_buffer) {
		qCCritical(LC_H264DEMUXER) << "Cannot alloc buffer";
		return;
	}
	AVIOContext* avio_ctx = avio_alloc_context(avio_ctx_buffer, 25000,
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
		qCCritical(LC_H264DEMUXER) << "Cannot open input context:" << av_err2string(ret);
		return;
	}
}
void H264Demuxer::start(std::shared_ptr<RawPipe> in)
{
	in->AddUploadListener([this, wInput = std::weak_ptr<RawPipe>(in)](std::weak_ptr<Raw> wRaw, size_t index) {
		auto raw = wRaw.lock();
		auto input = wInput.lock();
		if (!raw || !input)
			return;
		_readingOpaque.data = raw->raw;
		_readingOpaque.size = raw->size;
		_readingOpaque.totalWritten = 0;
		auto packet = _out->HoldForUploading();
		av_packet_unref(packet.ptr.get());
		do {
			int ret = av_read_frame(_ctx, packet.ptr.get());
			input->UnmapReading(index);
			if (ret < 0)
			{
				_out->unmapWriting(packet.subpipe, false);
				qCWarning(LC_H264DEMUXER) << "read packet error" << av_err2string(ret);
				return;
			}
		} while (_readingOpaque.totalWritten != _readingOpaque.size);
		_out->unmapWriting(packet.subpipe, true);

		});
}
std::shared_ptr<PacketPipe> H264Demuxer::output()
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
	, _out(createPacketPipe())
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
		qCCritical(LC_H264DEMUXER) << "Cannot open input context:" << av_err2string(ret);
		return;
	}
}
void Audio::OpusDemuxer::start(std::shared_ptr<RawPipe> in)
{
	in->AddUploadListener([this, wInput = std::weak_ptr<RawPipe>(in)](std::weak_ptr<Raw> wRaw, size_t index) {
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
			input->UnmapReading(index);
			if (ret < 0)
			{
				_out->unmapWriting(packet.subpipe, false);
				qCWarning(LC_H264DEMUXER) << "read packet error" << av_err2string(ret);
				return;
			}
		} while (_readingOpaque.totalWritten != _readingOpaque.size);
		_out->unmapWriting(packet.subpipe, true);

		});
}
std::shared_ptr<PacketPipe> Audio::OpusDemuxer::output()
{
	return _out;
}
