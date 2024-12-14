module;

module MapoToufuRenderer;

namespace MapoToufu
{
	bool Renderer::Init(std::pmr::memory_resource* resource)
	{
		if (resource != nullptr)
		{
			Dumpling::Device::InitDebugLayer();
			renderer = Dumpling::Device::Create(resource);
			return renderer;
		}
		return false;
	}

	void Renderer::UpdateFormFrame(SceneWrapper& wrapper, AtomicComponentFilter<Form> c_filter, AtomicSingletonFilter<FrameRenderer::Ptr> c_renderer)
	{
		volatile int i = 0;
	}

	void Renderer::OnSceneCreated(Scene& scene, SystemProperty property)
	{

		Noodles::SystemNodeProperty node_property;
		node_property.priority = Noodles::Priority{property.priority_layout, property.priority_first, 0, 0};
		node_property.order_function = [](Noodles::SystemName self, Noodles::SystemName other) {  return Noodles::Order::BIGGER;  };
		auto FrameRenderer = renderer->CreateFrameRenderer();
		scene.AddSingleton(std::move(FrameRenderer));
		scene.CreateAndAddTickedAutomaticSystem(
			Renderer::UpdateFormFrame,
			{u8"renderer_update_renderer_frame", property.group_name}, node_property
		);
	}

	void Renderer::OnSceneCreated(Scene& scene)
	{
		
	}
}