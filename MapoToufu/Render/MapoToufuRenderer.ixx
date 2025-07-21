module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuDefine;
import MapoToufuRenderPass;

export namespace MapoToufu
{
	using FormEvent = Dumpling::FormEvent;
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

	using PassRenderer = Dumpling::PassRenderer;

	struct FrameRenderer
	{
		FrameRenderer(FrameRenderer const&) = default;
		FrameRenderer(FrameRenderer&&) = default;
		FrameRenderer(Dumpling::FrameRenderer::Ptr renderer) : frame_renderer(std::move(renderer)) {}
		
		bool CommitFrame() { return frame_renderer->CommitFrame().has_value(); }
		bool FlushFrame() { return frame_renderer->FlushToLastFrame(); }
		bool BeginPass(PassRenderer& pass_renderer, PassRequest const& request) const;
		bool EndPass(PassRenderer& pass_renderer) const;
	protected:
		Dumpling::FrameRenderer::Ptr frame_renderer;
	};

	struct Form
	{
		Dumpling::Form platform_form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		Dumpling::IGHeadUpDisplay::Ptr hud;
		FormEventHook::Ptr event_hook;
	};

	struct IGHud
	{
		Dumpling::IGHeadUpDisplay::Ptr hud;
	};
};