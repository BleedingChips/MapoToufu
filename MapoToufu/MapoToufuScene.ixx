module;

export module MapoToufuScene;

import std;
import Noodles;
import Potato;

export namespace MapoToufu
{

	export struct GameContext;
	export struct GameContextWrapper;
	using StructLayout = Potato::IR::StructLayout;

	struct ModuleInterface
	{
		struct Wrapper
		{
			void AddRef(ModuleInterface const* ptr) { ptr->AddModuleInterfaceRef(); }
			void SubRef(ModuleInterface const* ptr) { ptr->SubModuleInterfaceRef(); }
		
		};

		using Ptr = Potato::Pointer::IntrusivePtr<ModuleInterface, Wrapper>;
		virtual StructLayout::Ptr GetStructLayout() const = 0;

	protected:

		virtual bool PostRegister(GameContextWrapper& context) { return true; }

		virtual void AddModuleInterfaceRef() const = 0;
		virtual void SubModuleInterfaceRef() const = 0;

		friend struct GameContext;
	};

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

	export struct GameContext
	{

		struct Config
		{
			std::pmr::memory_resource* scene_resource = std::pmr::get_default_resource();
		};

		bool RegisterModule(ModuleInterface::Ptr ptr);
		Scene::Ptr CreateScene();
		void Loop() { context.ProcessTaskUntillNoExitsTask({}); }
		GameContext(Config config = {}) : scene_resource(config.scene_resource) {}

		ModuleInterface::Ptr FindModule(StructLayout const& layout) const
		{
			std::shared_lock sl(module_mutex);
			return FindModule_AssumedLocked(layout);
		}
		template<typename Type>
		ModuleInterface::Ptr FindModule() const
		{
			std::shared_lock sl(module_mutex);
			return FindModule_AssumedLocked<Type>();
		}

		bool CommitTask(Potato::Task::Task::Ptr task, Potato::Task::TaskProperty property) { return context.CommitTask(std::move(task), std::move(property)); }

	protected:

		std::thread::id GetWindowMessageThreadID_AssumedLocked() const;
		ModuleInterface::Ptr FindModule_AssumedLocked(StructLayout const& layout) const;
		template<typename Type>
		ModuleInterface::Ptr FindModule_AssumedLocked() const { return this->FindModule(*StructLayout::GetStatic<Type>()); }

		std::pmr::memory_resource* const scene_resource;

		Potato::Task::TaskContext context;
		mutable std::shared_mutex module_mutex;
		std::pmr::vector<std::tuple<StructLayout::Ptr, ModuleInterface::Ptr>> modules;

		friend struct GameContextWrapper;
	};

	export struct GameContextWrapper
	{
		bool CommitTask(Potato::Task::Task::Ptr task, Potato::Task::TaskProperty property) { return context.CommitTask(std::move(task), std::move(property)); }
		ModuleInterface::Ptr FindModule(StructLayout const& layout) const { return context.FindModule_AssumedLocked(std::move(layout)); }
		template<typename Type>
		ModuleInterface::Ptr FindModule() const { return context.FindModule_AssumedLocked<Type>();}
		std::thread::id GetWindowMessageThreadID() const { return context.GetWindowMessageThreadID_AssumedLocked(); };
	protected:
		GameContextWrapper(GameContext& context) : context(context) {}
		GameContext& context;
		friend struct GameContext;
	};
};

