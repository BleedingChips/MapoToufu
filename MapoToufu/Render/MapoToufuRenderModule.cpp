module;
#include <cassert>
module MapoToufuRenderModule;

namespace MapoToufu
{

	FormEventStorage::Ptr FormEventStorage::Create(std::pmr::memory_resource* resource)
	{
		auto ir = Potato::IR::MemoryResourceRecord::Allocate<FormEventStorage>(resource);
		if (ir)
		{
			return new(ir.Get()) FormEventStorage{ir};
		}
		return {};
	}

	FormEvent::Respond FormEventStorage::RespondEvent(FormEvent event)
	{
		if (event.IsModify())
		{
			auto modify = event.GetModify();
			if (modify.message == decltype(modify.message)::DESTROY)
			{
				Dumpling::Form::PostQuitEvent();
			}
		}
		std::lock_guard lg(receive_mutex);
		receive_events.emplace_back(std::move(event), false);
		return FormEvent::Respond::PASS;
	}

	void FormEventStorage::SwapReceiveEvent()
	{
		std::lock(respond_mutex, receive_mutex);
		std::lock_guard lg1(respond_mutex, std::adopt_lock);
		std::lock_guard lg2(receive_mutex, std::adopt_lock);
		std::swap(respond_events, receive_events);
		receive_events.clear();
	}


	auto RendererModule::Create(Config config) -> Ptr
	{
		Dumpling::Device::InitDebugLayer();
		auto render = Dumpling::Device::Create();
		if(render)
		{
			auto re = Potato::IR::MemoryResourceRecord::Allocate<RendererModule>(std::pmr::get_default_resource());
			if(re)
			{
				return new(re.Get()) RendererModule{re, config, std::move(render)};
			}
		}
		return {};
	}

	RendererModule::RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer)
		: MemoryResourceRecordIntrusiveInterface(record), config(config), renderer(std::move(renderer))
	{
		
	}

	void RendererModule::Renderer_FlushFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_form, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const>)
	{
		if (c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				ptr->frame_renderer->FlushToLastFrame();
			}
		}

		std::size_t ite = 0;
		while (c_form.IterateComponent(wrapper, ite++))
		{
			auto span = c_form.AsSpan<Form>();
			for (auto& ite : span)
			{
				ite.form_wrapper->Present();
			}
		}
		
	}

	void RendererModule::Renderer_DispatchPass(SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const>)
	{
		if(c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				for (auto& ite : ptr->reference_node)
				{
					wrapper.AddTemporarySystemNode(*ite, {});
				}
				ptr->reference_node.clear();
			}
		}
	}

	void RendererModule::Renderer_CommitedFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_filter, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer>)
	{
		if (c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				ptr->frame_renderer->CommitFrame();
			}
		}

		std::size_t ite = 0;
		while (c_filter.IterateComponent(wrapper, ite++))
		{
			auto span = c_filter.AsSpan<Form>();
			for (auto& ite : span)
			{
				ite.form_wrapper->LogicPresent();
				if(ite.is_primary)
				{
					ite.ForeachEvent([&](FormEvent const& event) ->FormEvent::Respond
						{
							if(event.IsModify())
							{
								auto modify = event.GetModify();
								if(modify.message == decltype(modify.message)::DESTROY)
								{
									wrapper.GetContext().Quit();
								}
							}
							return FormEvent::Respond::PASS;
						}
						);
				}
				ite.event_storage->SwapReceiveEvent();
			}
		}
	}

	bool RendererModule::CreateRenderer(Scene& scene)
	{
		FrameRenderer f_renderer;
		f_renderer.frame_renderer = renderer->CreateFrameRenderer();
		if (scene.AddSingleton(std::move(f_renderer)))
		{
			scene.CreateAndAddAutoSystem(
				Renderer_CommitedFrame,
				{
					{ config.render_layer, config.render_priority_first, config.render_priority_second, 0 },
					u8"renderer_module%commit_frame"
				}
			);


			scene.CreateAndAddAutoSystem(
				Renderer_DispatchPass,
				{
				{ config.render_layer, config.render_priority_first, config.render_priority_second, 1 },
				u8"renderer_module%dispatch_pass"
				});


			scene.CreateAndAddAutoSystem(
				Renderer_FlushFrame,
				{
					{config.render_layer, config.render_priority_first, config.render_priority_second, 2 },
					u8"renderer_module%flush_frame"
				}
			);

			return true;
		}
		return false;
	}

	bool RendererModule::CreateForm(Entity& entity, Scene& scene, FormConfig const& config)
	{

		Dumpling::Form::Config real_config = config;
		auto storage = FormEventStorage::Create();
		real_config.event_capture = storage;

		auto form = Dumpling::Form::Create(std::move(real_config));
		if(form)
		{
			auto wrapper = renderer->CreateFormWrapper(form, {});
			if(wrapper)
			{
				wrapper->LogicPresent();
				Form real_form;
				real_form.form = std::move(form);
				real_form.is_primary = config.is_primary;
				real_form.event_storage = std::move(storage);
				real_form.form_wrapper = wrapper;
				if (scene.AddEntityComponent(entity, std::move(real_form)))
				{
					return true;
				}
			}
			form.DestroyForm();
		}
		return false;
	}
}