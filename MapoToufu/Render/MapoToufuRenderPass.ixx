module;

export module MapoToufuRenderPass;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuScene;
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
	
};