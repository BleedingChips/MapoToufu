module;
#include <cassert>
#include "windows.h"
module MapoToufuRenderer;

namespace MapoToufu
{

	struct DefaultFormEventHoot : public FormEventHook, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = FormEventHook::Ptr;

		static Ptr Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource());


		virtual FormEvent::Respond Hook(FormEvent& event) override
		{
			if (event.IsFormDestory())
			{
				need_quit = true;
				FormEvent::PostQuitEvent();
			}
			return event.RespondMarkAsSkip();
		}
		virtual void UpdateEventHook(Context& context, Form& form) override
		{
			if (need_quit)
			{
				context.GetInstance().RequireQuit();
			}
		}
		DefaultFormEventHoot(Potato::IR::MemoryResourceRecord record) : MemoryResourceRecordIntrusiveInterface(record) {}
		virtual void AddFormEventHookRef() const { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		virtual void SubFormEventHookRef() const { MemoryResourceRecordIntrusiveInterface::SubRef(); }
	protected:
		std::atomic_bool need_quit = false;
	};

	DefaultFormEventHoot::Ptr DefaultFormEventHoot::Create(std::pmr::memory_resource* resource)
	{
		auto re = Potato::IR::MemoryResourceRecord::Allocate<DefaultFormEventHoot>(resource);
		if (re)
		{
			return new (re.Get()) DefaultFormEventHoot{ re };
		}
		return {};
	}

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
		result.form_wrapper->LogicPresent();
		return result;
	}

	IGHud FrameRenderer::CreateIGHUD(Form& form, Dumpling::IGWidget::Ptr wight) const
	{
		IGHud hud;
		hud.hud = Dumpling::IGHeadUpDisplay::Create(form.platform_form, *frame_renderer, std::move(wight));
		return hud;
	}
}