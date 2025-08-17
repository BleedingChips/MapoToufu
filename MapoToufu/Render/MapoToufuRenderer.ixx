module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;
import MapoToufuRendererForm;
import MapoToufuRendererIMGui;


export namespace MapoToufu
{

	using PassRenderer = Dumpling::PassRenderer;

	struct FrameRenderer
	{
		FrameRenderer(FrameRenderer const&) = default;
		FrameRenderer(FrameRenderer&&) = default;
		FrameRenderer(Dumpling::FrameRenderer::Ptr renderer, Dumpling::Device::Ptr device) : frame_renderer(std::move(renderer)), device(std::move(device)){}
		~FrameRenderer();
		bool CommitFrame() { return frame_renderer->CommitFrame().has_value(); }
		bool WaitLastFrameFinish() const { return frame_renderer->FlushToLastFrame(); }
		bool BeginPass(PassRenderer& pass_renderer, PassRequest const& request) const;
		bool EndPass(PassRenderer& pass_renderer) const;
		Form CreateForm(FormConfig config = {}) const;
		IGHud CreateIGHUD(Form& form, Dumpling::IGWidget::Ptr wight = {}) const;
	protected:
		Dumpling::FrameRenderer::Ptr frame_renderer;
		Dumpling::Device::Ptr device;
	};

	
};