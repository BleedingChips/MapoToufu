module;

module MapoToufuRenderer;

namespace MapoToufu
{
	auto RendererModule::Create(Config config, std::pmr::memory_resource* resource)
		->Ptr
	{
		auto re = Potato::IR::MemoryResourceRecord::Allocate<RendererModule>(resource);
		if (re)
		{
			return new(re.Get()) RendererModule{ re, std::move(config) };
		}
		return {};
	}

	RendererModule::RendererModule(Potato::IR::MemoryResourceRecord record, Config config)
		: MemoryResourceRecordIntrusiveInterface(record)
	{
		Dumpling::Device::InitDebugLayer();
	}
}