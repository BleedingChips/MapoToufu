module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;

export namespace MapoToufu
{
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

	protected:

		Noodles::StructLayoutManager::Ptr manager;
		Config const config;

		Potato::Task::Context task_context;
	};

}
