#include "decoder.h"
using namespace media;
static int counter = 0;

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
	:out(std::make_shared<FramePipe>())

{
}
bool AbstractDecoder::start(std::shared_ptr<PacketPipe> input)
{
	auto cCtx = codexContext();
	out->reset([=]() {
		auto frame = av_frame_alloc();
		frame->height = cCtx->height;
		frame->width = cCtx->width;
		frame->format = cCtx->pix_fmt;
		av_frame_get_buffer(frame, 32);
		return frame;
		},
		[](AVFrame*p) {
			av_frame_free(&p);
		});
	if (avcodec_open2(cCtx.get(), codec(), nullptr) < 0)
	{
		qWarning() << "Cannot open codec";
		return false;
	}
	input->onDataChanged([=](std::weak_ptr<AVPacket> wpack, size_t index) {
		quene.enqueue([=]() {
			auto pack = wpack.lock();
			if (!pack)
				return;
			int resp = avcodec_send_packet(cCtx.get(), pack.get());
			if (resp < 0)
				qDebug() << "cannot send packet " << av_err2str(resp);
			input->unmapReading(index);
			while (resp >= 0)
			{
				auto outFrame = out->holdForWriting();
				resp = avcodec_receive_frame(cCtx.get(), outFrame.ptr.get());
				if (resp < 0)
				{
					if(resp != AVERROR(EAGAIN) && resp != AVERROR_EOF)
						printf("Error while sending a packet to the decoder: %s", av_err2str(resp));
					out->unmapWriting(outFrame.subpipe, false);
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
}
AbstractDecoder::~AbstractDecoder()
{
	stop();
}
Video::Decoder::Decoder(const Video::Source& src)
{
	cdc = avcodec_find_decoder(src.codecID);
	if (!cdc)
	{
		qCritical() << "Cannot find decoder with id" << src.codecID;
		return;
	}
	cCtx = createCodecContext(cdc);
	if (cCtx == nullptr)
	{
		qWarning() << "Cannot create codec context";
		return;
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	avcodec_parameters_from_context(codecPar, cCtx.get());
	codecPar->width	 = src.width;
	codecPar->height = src.height;
	codecPar->format = src.format;
	avcodec_parameters_to_context(cCtx.get(), codecPar);
}
const AVCodec* Video::Decoder::codec()
{
	return cdc;
}
const std::shared_ptr<AVCodecContext> Video::Decoder::codexContext()
{
	return cCtx;
}

Audio::Decoder::Decoder(const Audio::Source& src)
{
	cdc = avcodec_find_decoder(src.codecID);
	if (!cdc)
	{
		qCritical() << "Cannot find decoder with id" << src.codecID;
		return;
	}
	cCtx = createCodecContext(cdc);
	if (cCtx == nullptr)
	{
		qWarning() << "Cannot create codec context";
		return;
	}
	AVCodecParameters* codecPar = avcodec_parameters_alloc();
	//cCtx->request_sample_fmt = av_get_alt_sample_fmt(cCtx->sample_fmt, 0);
	cCtx->channel_layout = av_get_default_channel_layout(src.channelCount);
	cCtx->channels = src.channelCount;
	cCtx->sample_fmt = src.format;

	avcodec_parameters_to_context(cCtx.get(), src.par);

}
const AVCodec* Audio::Decoder::codec()
{
	return cdc;
}
const std::shared_ptr<AVCodecContext> Audio::Decoder::codexContext()
{
	return cCtx;
}