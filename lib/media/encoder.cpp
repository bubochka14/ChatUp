//#include "encoder.h"
//using namespace media;
//Encoder::Encoder()
//    :_out(std::make_shared<PacketPipe>([this]()
//        {
//            auto pack = av_packet_alloc();
//            return pack;
//        },
//        [](AVPacket* p)
//        {
//            return av_packet_free(&p);
//        }))
//{
//    _cCtx  = CCtx();
//    _codec = codec();
//}
//std::shared_ptr<media::DataPipe<AVPacket>> Encoder::output()
//{
//    return _out;
//}
//bool Encoder::start(std::shared_ptr<media::FramePipe> input)
//{
//    input->onDataChanged([this,input](std::weak_ptr<AVFrame> weak,size_t index) {
//        if (auto ptr = weak.lock())
//        {
//            int response = avcodec_send_frame(_cCtx, input->storedData(index).get());
//
//            while (response >= 0) {
//                response = avcodec_receive_packet(cCtx, packet);
//                frameReady = false;
//                if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
//                    break;
//                }
//                else if (response < 0) {
//                    printf("Error while receiving packet from encoder: %d", response);
//                    break;
//                }
//
//                packet->stream_index = 0;
//                //av_packet_rescale_ts(packet, decoder->video_avs->time_base, encoder->video_avs->time_base);
//                av_packet_rescale_ts(packet, { 1, 10000000 }, outStream->time_base);
//
//                printf("%d", av_write_frame(outCtx, packet));
//
//            }
//            input.unmapReading();
//        }
//        });
//    return true;
//}
