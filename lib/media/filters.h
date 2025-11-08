#pragma once
extern "C"
{
#include <libavdevice/avdevice.h>
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
}
#include <optional>
#include <media.h>
#include <qloggingcategory.h>
namespace Media {
	class CC_MEDIA_EXPORT AbstractFilter
	{
	public:
		std::shared_ptr<Media::FramePipe> output();
		virtual void close();
		virtual ~AbstractFilter() = default;
	protected:
		AbstractFilter();
	private:
		std::shared_ptr<Media::FramePipe> _out;
		std::shared_ptr<Media::FramePipe> _input;
	};
}
Q_DECLARE_LOGGING_CATEGORY(LC_NOISE_REDUCTION_FILTER)

namespace Media::Audio
{
	class CC_MEDIA_EXPORT Filter : public AbstractFilter
	{
	public:
		virtual std::optional<SourceConfig> open(std::shared_ptr<Media::FramePipe> input);
		std::shared_ptr<Media::FramePipe> input();

	private:
		std::shared_ptr<Media::FramePipe> _input;
	};

	class CC_MEDIA_EXPORT FilterFactory
	{
	public:
		virtual std::shared_ptr<Filter> createFilter(SourceConfig config) = 0;
	};

	class CC_MEDIA_EXPORT NoiseReductionFilter : public Filter
	{
	public:
		struct Description
		{
			std::string rnnnModelPath;
			float noiseReduction = 0.2;
			float noiseFloor = 0.3;
		};
		NoiseReductionFilter(Description desc, SourceConfig config);
		std::optional<SourceConfig> open(std::shared_ptr<Media::FramePipe> input) override;
	private:
		Description _desc;
		SourceConfig _inputConfig;
	};
	class CC_MEDIA_EXPORT NoiseReductionFilterFactory : public FilterFactory
	{
	public:
		NoiseReductionFilterFactory(NoiseReductionFilter::Description desc);
		std::shared_ptr<Filter> createFilter(SourceConfig config) override;

	private:
		NoiseReductionFilter::Description _desc;
	};

}
