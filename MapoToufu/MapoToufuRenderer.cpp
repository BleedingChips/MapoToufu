module;
#include <cassert>
module MapoToufuRenderer;

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
			std::lock_guard lg(receive_mutex);
			receive_events.emplace_back(std::move(event), false);
		}
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

	void Renderer_FlushFormFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_form, AutoSingletonQuery<FrameRenderer> c_renderer)
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

	void Renderer_Dispath_renderer(SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> c_renderer)
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

	void Renderer_CommitedFormFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_filter, AutoSingletonQuery<FrameRenderer> c_renderer)
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
				Renderer_CommitedFormFrame,
				{
					{ config.priority_layout, config.priority_first, 0, 0 },
					u8"renderer_module:renderer_commit_renderer_frame"
				}
			);


			scene.CreateAndAddAutoSystem(
				Renderer_Dispath_renderer,
				{
				{ config.priority_layout, config.priority_first, 1, 0 },
				u8"renderer_module:renderer_commit_dispatch"
				});


			scene.CreateAndAddAutoSystem(
				Renderer_FlushFormFrame,
				{
					{config.priority_layout, config.priority_first, 2, 0 },
					u8"renderer_flush_renderer_frame"
				}
			);
			return true;
		}
		return false;
	}

	bool RendererModule::CreateForm(Entity& entity, Scene& scene)
	{

		Dumpling::Form::Config config;
		auto storage = FormEventStorage::Create();
		config.event_capture = storage;

		auto form = Dumpling::Form::Create(std::move(config));
		if(form)
		{
			auto wrapper = renderer->CreateFormWrapper(form, {});
			if(wrapper)
			{
				wrapper->LogicPresent();
				Form real_form{ std::move(form), std::move(wrapper), std::move(storage)};

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