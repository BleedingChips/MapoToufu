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
}