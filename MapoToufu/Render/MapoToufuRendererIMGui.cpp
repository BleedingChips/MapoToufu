module;

module MapoToufuRendererIMGui;
import MapoToufuDefine;
import MapoToufuRenderer;

namespace MapoToufu
{
	std::u8string_view IGHUDPass::GetPassName() { return u8"IGHUDPass:MapoToufu"; }
	SystemNode::Ptr IGHUDPass::GetPassSystem()
	{
		static auto system = AutoSystemNodeStatic(
			[](Context& context, AutoSingletonQuery<FrameRenderer const, PassDistributor const> singleton) {

				auto [render, distributor] = singleton.Query(context)->GetPointerTuple();

				if (render == nullptr || distributor == nullptr)
					return;

				auto pass_index = distributor->GetPassIndex(GetPassName());

				Dumpling::PassRequest request;
				Dumpling::PassRenderer pass_render;
				std::size_t request_iterator = 0;

				while (distributor->PopRequest(pass_index, request, request_iterator))
				{
					request_iterator += 1;
					if (render->BeginPass(pass_render, request))
					{
						auto property = request.TryGetParameter<Property>();
						if (property != nullptr)
						{
							if (property->hud)
							{
								pass_render.SetRenderTargets(property->target);
								property->hud->Draw(pass_render);
							}
						}
						render->EndPass(pass_render);
					}
				}
			}
		);
		return &system;
	}
}