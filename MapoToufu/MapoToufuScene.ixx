module;

export module MapoToufuScene;

import std;
import Noodles;
import Potato;
import Dumpling;

export namespace MapoToufu
{

	using Noodles::AtomicComponentFilter;
	using Noodles::AtomicSingletonFilter;

	using SceneWrapper = Noodles::ContextWrapper;
	using Noodles::Entity;

	struct Scene : public Noodles::Context, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<Scene>;

		static Ptr Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource()) { 
			return Potato::IR::MemoryResourceRecord::AllocateAndConstruct<Scene>(resource);
		}

		Scene(Potato::IR::MemoryResourceRecord record) : MemoryResourceRecordIntrusiveInterface(record) {}

	protected:

		void AddContextRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubContextRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
	};

	
};

