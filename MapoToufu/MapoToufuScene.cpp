module;

module MapoToufuScene;

namespace MapoToufu
{
	Scene::Ptr Scene::Create(Noodles::StructLayoutManager& manager, std::pmr::memory_resource* resource) {
		auto re = Potato::IR::MemoryResourceRecord::Allocate<Scene>(resource);
		if (re)
		{
			return new (re.Get()) Scene{manager, re};
		}
		return {};
	}

	Scene::Scene(Noodles::StructLayoutManager& manager, Potato::IR::MemoryResourceRecord record)
	: Context(manager), MemoryResourceRecordIntrusiveInterface(record)
	{}
}

