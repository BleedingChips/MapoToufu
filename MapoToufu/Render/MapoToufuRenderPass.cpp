module;
#include <cassert>
module MapoToufuRenderPass;

import MapoToufuRenderer;

namespace MapoToufu
{
	std::wstring_view IGHUDPass::GetPassName() { return L"IGHUDPass:MapoToufu"; }
	SystemNode::Ptr IGHUDPass::GetPassSystem()
	{
		static auto system = AutoSystemNodeStatic(
			[](Context& context, AutoSingletonQuery<FrameRenderer const, PassDistributor const> singleton_query) {
				auto [render, distrubutor] = singleton_query.Query(context)->GetPointerTuple();
				if (render == nullptr || distrubutor == nullptr)
					return;

				auto cur_index = distrubutor->GetPassIndex(GetPassName());
				if (!cur_index)
					return;

				std::size_t iterator = 0;
				Dumpling::PassRenderer pass_render;
				Dumpling::PassRequest request;
				while (distrubutor->PopRequest(cur_index, request, iterator))
				{
					++iterator;

					if (request.parameter && request.parameter->GetStructLayout()->IsStatic<IGHUDPass::Property>())
					{
						if (render->BeginPass(pass_render, request))
						{
							auto property = request.parameter->GetStaticCastData<IGHUDPass::Property>();
							if (property->target_hud)
							{
								property->target_hud->Draw(pass_render);
							}
							render->EndPass(pass_render);
						}
					}
				}
			}
		);
		return &system;
	}
}