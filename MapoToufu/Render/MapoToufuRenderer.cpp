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

	Form FrameRenderer::CreateForm(FormConfig form_config) const
	{
		Form result;
		Dumpling::Form::Config config;
		config.title = form_config.title;
		config.style = form_config.style;
		config.rectangle = form_config.rectangle;
		config.event_hook = form_config.event_hook;
		if (!config.event_hook)
		{
			auto hook = DefaultFormEventHoot::Create();
			config.event_hook = hook;
			result.event_hook = hook;
		}
		result.platform_form = Dumpling::Form::Create(config);
		result.form_wrapper = device->CreateFormWrapper(result.platform_form);
		return result;
	}

	IGHud FrameRenderer::CreateIGHUD(Form& form, Dumpling::IGWidget::Ptr wight) const
	{
		IGHud hud;
		hud.hud = Dumpling::IGHeadUpDisplay::Create(form.platform_form, *frame_renderer, std::move(wight));
		return hud;
	}

	FrameRenderer::~FrameRenderer()
	{
		if (frame_renderer)
		{
			frame_renderer->FlushToLastFrame();
		}
	}
}