module;
#include <cassert>
#include "windows.h"
module MapoToufuRenderer;

namespace MapoToufu
{

	bool FrameRenderer::BeginPass(PassRenderer& pass_renderer, PassRequest const& request) const
	{
		return frame_renderer->PopPassRenderer(pass_renderer, request);
	}

	bool FrameRenderer::EndPass(PassRenderer& pass_renderer) const
	{
		return frame_renderer->FinishPassRenderer(pass_renderer);
	}
}