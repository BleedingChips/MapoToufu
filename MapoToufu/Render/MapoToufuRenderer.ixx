module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuScene;
import MapoToufuGameContext;

export namespace MapoToufu
{

	export struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr frame_renderer;
		std::pmr::vector<SystemNode::Ptr> reference_node;
	};

};