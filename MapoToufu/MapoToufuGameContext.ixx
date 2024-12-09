module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;

export namespace MapoToufu
{
	export struct GameContext;
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

		virtual void PostRegister(GameContext& context) { }
		virtual bool MainThreadTick() { return false; }

		virtual void AddModuleInterfaceRef() const = 0;
		virtual void SubModuleInterfaceRef() const = 0;

		friend struct GameContext;
	};

	export struct GameContext
	{

		struct Config
		{
			std::pmr::memory_resource* scene_resource = std::pmr::get_default_resource();
		};

		bool RegisterModule(ModuleInterface::Ptr ptr);
		Scene::Ptr CreateScene();
		void Loop();
		GameContext(Config config = {}) : scene_resource(config.scene_resource) {}

		ModuleInterface::Ptr FindModule(StructLayout const& layout) const;
		template<typename Type>
		ModuleInterface::Ptr FindModule() const { return FindModule(StructLayout::GetStatic<Type>()); }

		bool CommitTask(Potato::Task::Task::Ptr task, Potato::Task::TaskProperty property) { return context.CommitTask(std::move(task), std::move(property)); }

	protected:

		std::thread::id GetWindowMessageThreadID_AssumedLocked() const;

		std::pmr::memory_resource* const scene_resource;

		Potato::Task::TaskContext context;

		mutable std::shared_mutex module_mutex;
		std::pmr::vector<std::tuple<StructLayout::Ptr, ModuleInterface::Ptr>> modules;

		std::mutex form_init;
		std::thread::id current_thread_id;
		Dumpling::Device::Ptr renderer_device;
	};

}
