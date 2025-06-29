module;

export module MapoToufuRenderPass;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuGameContext;

export namespace MapoToufu
{

	struct PipelineRequester
	{
		bool is_enable = false;
		Dumpling::PipelineRequester::Ptr requester;
		Dumpling::PipelineInstance::Ptr instance;
		Potato::IR::StructLayoutObject::Ptr pipeline_object;
	};

	struct PipelineInfo
	{
		Dumpling::PipelineRequester::Ptr requester;
		Dumpling::PassRenderer pass_renderer;
	};

	using PipelineData = SystemUserDataDefault<PipelineInfo>;
	
};