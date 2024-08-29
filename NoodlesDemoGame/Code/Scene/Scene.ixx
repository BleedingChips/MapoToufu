module;

export module Scene;

export import NoodlesContext;
export import PotatoIR;

export
{
	struct SceneContext
	{
		SceneContext(Noodles::ExecuteContext& context)
			: noodles_context(context.noodles_context){}
		SceneContext(SceneContext const&) = default;
		void Quit() { noodles_context.Quit(); }
	protected:
		Noodles::Context& noodles_context;
	};


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

