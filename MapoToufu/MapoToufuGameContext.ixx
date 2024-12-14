module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;
import MapoToufuRenderer;

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

		using FormEventCapturePlatform = Dumpling::FormEventCapturePlatform;
		using FormEventCapture = Dumpling::FormEventCapture;

		struct Config
		{
			std::pmr::memory_resource* scene_resource = std::pmr::get_default_resource();
			std::pmr::memory_resource* renderer_resource = std::pmr::get_default_resource();
			std::int32_t renderer_priority_layout = 0;
			std::int32_t renderer_priority_first = 0;
			bool with_renderer = true;
		};

		bool RegisterModule(ModuleInterface::Ptr ptr);
		Scene::Ptr CreateScene();
		void Loop();

		GameContext(Config config = {});

		ModuleInterface::Ptr FindModule(StructLayout const& layout) const;
		template<typename Type>
		ModuleInterface::Ptr FindModule() const { return FindModule(StructLayout::GetStatic<Type>()); }

		bool CommitTask(Potato::Task::Task::Ptr task, Potato::Task::TaskProperty property) { return context.CommitTask(std::move(task), std::move(property)); }
		void InsertEventCapture(FormEventCapturePlatform::Ptr capture);

	protected:

		Config const config;
		std::thread::id const current_thread_id;

		Potato::Task::TaskContext context;

		mutable std::shared_mutex module_mutex;
		std::pmr::vector<std::tuple<StructLayout::Ptr, ModuleInterface::Ptr>> modules;

		Renderer renderer;

		mutable std::shared_mutex event_mutex;
		std::pmr::vector<Dumpling::FormEventCapturePlatform::Ptr> event_captures;
	};

}
