module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;

export namespace MapoToufu
{

	struct Form;

	struct FormEventHook : public Dumpling::FormEventHook
	{
		using Ptr = Potato::Pointer::IntrusivePtr<FormEventHook, Dumpling::FormEventHook::Wrapper>;
		virtual void UpdateEventHook(Context& context, Form& form) {}
	};

	struct FormConfig
	{
		Dumpling::Form::Rectangle rectangle;
		Dumpling::FormStyle::Ptr style = Dumpling::FormStyle::GetFixedStyle();
		FormEventHook::Ptr event_hook;
		wchar_t const* title = L"MapoToufu Form";
	};

	struct Form
	{
		RendererResource::Ptr GetRenderTarget() const { return form_wrapper->GetAvailableRenderResource(); }
		Dumpling::Form platform_form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		FormEventHook::Ptr event_hook;
		bool need_skip_first_present = true;
	};

	struct IGHud
	{
		bool enable = true;
		Dumpling::IGHeadUpDisplay::Ptr hud;
	};

	using PassRenderer = Dumpling::PassRenderer;

	struct FrameRenderer
	{
		FrameRenderer(FrameRenderer const&) = default;
		FrameRenderer(FrameRenderer&&) = default;
		FrameRenderer(Dumpling::FrameRenderer::Ptr renderer, Dumpling::Device::Ptr device) : frame_renderer(std::move(renderer)), device(std::move(device)){}
		
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