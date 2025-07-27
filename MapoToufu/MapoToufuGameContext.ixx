module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuDefine;

export namespace MapoToufu
{
	struct GameContext
	{
		Instance::Ptr CreatInstance(Instance::Config config = {});
		void Loop();
		bool Launch(Instance& scene);

		GameContext();

		auto& GetTaskContext() { return task_context; }

	protected:

		Potato::Task::Context task_context;
	};

}
