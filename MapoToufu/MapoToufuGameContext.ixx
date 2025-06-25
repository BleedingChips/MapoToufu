module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuInstance;
import MapoToufuDefine;

export namespace MapoToufu
{
	struct GameContext
	{
		Instance::Ptr CreatInstance(Instance::Config config = {});
		
		//Scene::Ptr CreateScene();
		void Loop();
		bool Launch(Instance& scene);

		GameContext();

	protected:

		Potato::Task::Context task_context;
	};

}
