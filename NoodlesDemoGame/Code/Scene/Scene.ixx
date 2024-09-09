module;

export module Scene;

export import NoodlesContext;
export import PotatoIR;

export
{

	using SceneWrapper = Noodles::ContextWrapper;


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

