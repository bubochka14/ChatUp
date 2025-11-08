#include <mediacontext.h>
Q_LOGGING_CATEGORY(LC_ABSTRACT_CODEC, "AbstractCodec")
using namespace Media;
AbstractCodec::AbstractCodec(const AVCodec* codec)
	:_codec(codec)
{

}
const AVCodec* AbstractCodec::codec()
{
	return _codec;
}
std::shared_ptr<AVCodecContext> AbstractCodec::codecContext()
{
	return _cCtx;
}
bool AbstractCodec::fillContext(std::shared_ptr<AVCodecContext> ctx)
{
	return true;
}
bool AbstractCodec::initialize()
{
	if (isInitialized())
		reset();
	auto cCtx = Media::createCodecContext(codec());
	if (!cCtx)
	{
		qCCritical(LC_ABSTRACT_CODEC) << "Cannot create codec context";
		return false;
	}
	if (!fillContext(cCtx))
	{
		qCWarning(LC_ABSTRACT_CODEC) << "Cannot initialize codec context";
		return false;
	}
	_cCtx = cCtx;
	_isInitialized = true;
	return true;

}
std::string AbstractCodec::codecName()
{
	if (!codec())
		return "empty";
	return avcodec_get_name(codec()->id);
}
AbstractCodec::~AbstractCodec()
{
	reset();
}
bool AbstractCodec::isInitialized()
{
	return _isInitialized;
}
void AbstractCodec::reset()
{
	_cCtx.reset();
	_isInitialized = false;
}