//#pragma once
//#include <functional>
//#include <qobject.h>
//#include "datapipe.h"
//#include <thread>
//#include <mutex>
//#include "media.h"
//#include "taskqueue.h"
//#include <condition_variable>
//extern "C"
//{
//#include <libavcodec/avcodec.h>
//}
//namespace media {
//	class Encoder 
//	{
//	public:
//		std::shared_ptr<media::PacketPipe> output();
//		bool start(std::shared_ptr<media::FramePipe> input);
//		virtual ~Encoder() = default;
//	protected:
//		explicit Encoder();
//		virtual AVCodecContext* CCtx() = 0;
//		virtual const AVCodec* codec() = 0;
//	public:
//		const AVCodec* _codec;
//		AVCodecContext* _cCtx;
//		AVPacket* _pack;
//		TaskQueue queue;
//		std::shared_ptr<media::DataPipe<AVPacket>> _out;
//	};
//}