module;

export module MapoToufuModule;

import std;
import Potato;
import Dumpling;
import MapoToufuInstance;

export namespace MapoToufu
{

	struct Module
	{
		struct Wrapper
		{
			void AddRef(Module const* ptr) { return ptr->AddModuleRef(); }
			void SubRef(Module const* ptr) { return ptr->SubModuleRef(); }
		};

		using Ptr = Potato::Pointer::IntrusivePtr<Module, Wrapper>;

		virtual void Load(Instance& instance) {}
		virtual void UnLoad(Context& context) {}
		

	protected:

		virtual void AddModuleRef() const = 0;
		virtual void SubModuleRef() const = 0;
		friend struct Wrapper;
	};

	/*
	struct ModuleManager
	{
		struct Wrapper
		{
			void AddRef(ModuleManager const* ptr) { return ptr->AddModuleManagerRef(); }
			void SubRef(ModuleManager const* ptr) { return ptr->SubModuleManagerRef(); }
		};

		using Ptr = Potato::Pointer::IntrusivePtr<ModuleManager, Wrapper>;

		static Ptr Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		struct ModuleInfo
		{
			Module::Ptr module;
			std::size_t reference = 0;
			bool mark_as_need_unload = 0;
		};

	protected:

		std::shared_mutex moudle_mutex;
		std::vector<ModuleInfo> module_infos;
		std::size_t version = 0;

		virtual void AddModuleManagerRef() const = 0;
		virtual void SubModuleManagerRef() const = 0;

		friend struct Wrapper;
	};

	export struct ModuleWrapper
	{
		std::size_t module_version = 0;
	};
	*/



	/*
	export struct GameContext;

	export struct GameContext
	{

		using FormEventCapturePlatform = Dumpling::FormEventCapturePlatform;
		using FormEventCapture = Dumpling::FormEventCapture;

		struct Config
		{
			std::pmr::memory_resource* scene_resource = std::pmr::get_default_resource();
			std::pmr::memory_resource* renderer_resource = std::pmr::get_default_resource();
		};

		Scene::Ptr CreateScene();
		void Loop();

		GameContext(Config config = {});

		bool CommitTask(Potato::Task::Node& task, Potato::Task::Property property) { return task_context.Commit(task, std::move(property)); }
		void InsertEventCapture(FormEventCapturePlatform::Ptr capture);
		bool Launch(Scene& scene);
		static std::size_t GetMainLoopGroupID() { return std::numeric_limits<std::size_t>::max(); }

	protected:

		Noodles::StructLayoutManager::Ptr manager;
		Config const config;

		Potato::Task::Context task_context;
	};
	*/
}
