module;

export module MapoToufuRendererIMGui;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;

export namespace MapoToufu
{
	struct IGHud
	{
		bool enable = true;
		Dumpling::IGHeadUpDisplay::Ptr hud;
	};

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
	
};