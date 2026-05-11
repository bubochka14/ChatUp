#include "filters.h"
Q_LOGGING_CATEGORY(LC_NOISE_REDUCTION_FILTER, "NoiseReductionFilter")
using namespace chatup;
AbstractFilter::AbstractFilter()
	:_out(createFramePipe())
{

}

std::shared_ptr<FramePipe> AbstractFilter::output()
{
	return _out;
}
void AbstractFilter::close()
{
	_input.reset();
}

Audio::NoiseReductionFilterFactory::NoiseReductionFilterFactory(NoiseReductionFilter::Description desc)
	:_desc(std::move(desc))
{}
std::shared_ptr<Audio::Filter> Audio::NoiseReductionFilterFactory::createFilter(SourceConfig config)
{
	return std::make_shared<NoiseReductionFilter>(_desc, std::move(config));
}
std::optional<Audio::SourceConfig>  Audio::Filter::open(std::shared_ptr<FramePipe> input)
{
	_input = input;
	return std::nullopt;
}
std::shared_ptr<FramePipe>  Audio::Filter::input()
{
	return _input;
}
Audio::NoiseReductionFilter::NoiseReductionFilter(Description desc, SourceConfig config)
	:_desc(std::move(desc))
	,_inputConfig(std::move(config))
{

}

std::optional<Audio::SourceConfig> Audio::NoiseReductionFilter::open(std::shared_ptr<FramePipe> input)
{
	AVFilterGraph* filter_graph;
	AVFilterContext* abuffer_ctx;
	const AVFilter* abuffer;
	AVFilterContext* arnndn_ctx;
	const AVFilter* arnndn;
	AVFilterContext* aformat_ctx;
	const AVFilter* aformat;
	AVFilterContext* abuffersink_ctx;
	const AVFilter* abuffersink;
	AVFilterContext* afftdn_ctx;
	const AVFilter* afftdn;
	avfilter_init_str(abuffer_ctx, NULL);
	AVDictionary* options_dict = NULL;
	
	char buffer[64];
	int ret = 0;
	afftdn = avfilter_get_by_name("afftdn");
	if (!afftdn)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Afftdn not found";
		return std::nullopt;
	}
	afftdn_ctx = avfilter_graph_alloc_filter(filter_graph, afftdn, "afftdn");
	ret = snprintf(buffer, sizeof buffer, "%f", -80 + _desc.noiseFloor * 60);
	if (ret < 0)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Internal memory error";
		return std::nullopt;
	}
	ret = av_dict_set(&options_dict, "nf", buffer, 0);
	if (ret < 0)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Setting option error:"<<av_err2string(ret);
		return std::nullopt;
	}
	ret = snprintf(buffer, sizeof buffer, "%f", 0.1 + _desc.noiseReduction * 96);
	if (ret < 0)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Internal memory error";
		return std::nullopt;

	}
	ret = av_dict_set(&options_dict, "nr", buffer, 0);
	if (ret < 0)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Setting option error:" << av_err2string(ret);
		return std::nullopt;
	}
	ret = avfilter_init_dict(afftdn_ctx, &options_dict);
	if (ret < 0)
	{
		qCCritical(LC_NOISE_REDUCTION_FILTER) << "Setting option error:" << av_err2string(ret);
		return std::nullopt;
	}
	av_dict_free(&options_dict);

	arnndn = avfilter_get_by_name("arnndn");
	arnndn_ctx = avfilter_graph_alloc_filter(filter_graph, arnndn, "arnndn");
	av_dict_set(&options_dict, "m", "cb.rnnn", 0);
	avfilter_init_dict(arnndn_ctx, &options_dict);
	av_dict_free(&options_dict);

	abuffersink = avfilter_get_by_name("abuffersink");
	abuffersink_ctx = avfilter_graph_alloc_filter(filter_graph, abuffersink, "sink");

	avfilter_init_str(abuffersink_ctx, NULL);
	avfilter_link(abuffer_ctx, 0, arnndn_ctx, 0);
	avfilter_link(arnndn_ctx, 0, afftdn_ctx, 0);
	avfilter_link(afftdn_ctx, 0, abuffersink_ctx, 0);
	avfilter_graph_config(filter_graph, NULL);
	input->AddUploadListener([this, abuffersink_ctx, abuffer_ctx, input](auto frameHandle) {
		av_buffersrc_add_frame(abuffer_ctx, frameHandle.Get());
		auto out = output();
		auto pipeData = out->holdForWriting();
		while (av_buffersink_get_frame(abuffersink_ctx, pipeData.ptr.get()) >= 0) {

			output()->unmapWriting(pipeData.subpipe, true);
			pipeData = out->holdForWriting();
		}
		out->unmapWriting(pipeData.subpipe, false);

	});
	SourceConfig outputConfig = _inputConfig;
	outputConfig.par->format = afftdn_ctx->outputs[0]->format;
	outputConfig.par->ch_layout = afftdn_ctx->outputs[0]->ch_layout;
	outputConfig.par->sample_rate = afftdn_ctx->outputs[0]->sample_rate;
	return outputConfig;
}