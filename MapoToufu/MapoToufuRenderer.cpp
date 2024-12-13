module;
#include <cassert>
module MapoToufuRenderer;

namespace MapoToufu
{
	auto RendererModule::Create(Config config) -> Ptr
	{
		auto render = Dumpling::Device::Create();
		if(render)
		{
			Dumpling::Device::InitDebugLayer();
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

	void Renderer_FlushFormFrame(SceneWrapper& wrapper, AtomicSingletonFilter<FrameRenderer> c_renderer)
	{
		auto wra = c_renderer.GetWrapper(wrapper);
		auto ptr = c_renderer.Get<FrameRenderer>(wra);
		if (ptr != nullptr)
		{
			ptr->frame_renderer->FlushToLastFrame();
		}
	}

	void Renderer_CommitedFormFrame(SceneWrapper& wrapper, AtomicComponentFilter<Form> c_filter, AtomicSingletonFilter<FrameRenderer> c_renderer)
	{
		auto wra = c_renderer.GetWrapper(wrapper);
		auto ptr = c_renderer.Get<FrameRenderer>(wra);
		if (ptr != nullptr)
		{
			ptr->frame_renderer->CommitFrame();
		}

		std::size_t ite = 0;
		while (true)
		{
			auto wra = c_filter.IterateComponent_AssumedLocked(wrapper, ite++);
			if (wra.has_value())
			{
				auto span = c_filter.AsSpan<Form>(*wra);
				for (auto& ite : span)
				{
					ite.form_wrapper->LogicPresent();
				}
			}
			else
			{
				break;
			}
		}
	}

	bool RendererModule::CreateRenderer(Scene& scene)
	{
		FrameRenderer f_renderer;
		f_renderer.frame_renderer = renderer->CreateFrameRenderer();
		if(scene.AddSingleton(std::move(f_renderer)))
		{
			Noodles::SystemNodeProperty node_property;
			node_property.priority = Noodles::Priority{ config.priority_layout, config.priority_first, 1, 0 };
			node_property.order_function = [](Noodles::SystemName self, Noodles::SystemName other) {  return Noodles::Order::BIGGER;  };

			scene.CreateAndAddTickedAutomaticSystem(
				Renderer_CommitedFormFrame,
				{ u8"renderer_commit_renderer_frame", config.group_name }, node_property
			);

			node_property.priority = Noodles::Priority{ config.priority_layout, config.priority_first, 0, 0 };
			scene.CreateAndAddTickedAutomaticSystem(
				Renderer_FlushFormFrame,
				{ u8"renderer_flush_renderer_frame", config.group_name }, node_property
			);
			return true;
		}
		return false;
	}

	Form RendererModule::CreateForm()
	{
		auto form = Dumpling::Form::Create({});
		if(form)
		{
			auto wrapper = renderer->CreateFormWrapper(form, {});
			if(wrapper)
			{
				return Form{ form, wrapper};
			}
		}
		return {};
	}
}