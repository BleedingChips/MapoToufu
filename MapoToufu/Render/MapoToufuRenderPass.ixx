module;

export module MapoToufuRenderPass;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;
import MapoToufuRenderer;

export namespace MapoToufu
{
	

	struct CleanViewTargetPass
	{
		struct Property
		{
			Dumpling::RenderTargetSet target;
			std::array<Dumpling::Color, Dumpling::RenderTargetSet::max_render_target_count> clean_color;
		};
		static std::wstring_view GetPassName();
		static SystemNode::Ptr GetPassSystem();
	};
	
};