module;

export module MapoToufuRenderPass;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;

export namespace MapoToufu
{

	using PassSequencer = Dumpling::PassSequencer;
	using PassRequest = Dumpling::PassRequest;
	using PassIndex = Dumpling::PassIndex;
	using PassDistributor = Dumpling::PassDistributor;

	struct IGHUDPass
	{
		struct Property
		{
			Dumpling::IGHeadUpDisplay::Ptr target_hud;
		};

		static std::wstring_view GetPassName();
		static SystemNode::Ptr GetPassSystem();
	};

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