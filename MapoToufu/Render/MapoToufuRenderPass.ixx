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
	struct IGHUDPass
	{
		struct Property
		{
			void SetParameter(RendererResource const& resource, IGHud const& hud)
			{
				target.Clear();
				target.AddRenderTarget(resource);
				this->hud = hud.hud;
			}
			Dumpling::RenderTargetSet target;
			Dumpling::IGHeadUpDisplay::Ptr hud;
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