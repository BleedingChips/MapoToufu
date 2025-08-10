module;

export module MapoToufuGameContext;

import std;
import Potato;
import Dumpling;
import MapoToufuDefine;
import MapoToufuSubModule;

export namespace MapoToufu
{

	struct GameContext
	{
		Instance::Ptr CreatInstance(InstanceConfig config = {});
		
		void RunMainThreadAndWait();

		bool Launch(Instance& scene);

		GameContext();

		auto& GetTaskContext() { return task_context; }

		bool RegisterModule(SubModule& sub_module) { return collection.RegisterSubModule(sub_module); }

	protected:

		SystemNode::Ptr EndSubModuleSystemNode();

		std::shared_mutex module_mutex;

		SubModuleCollection collection;

		Potato::Task::Context task_context;
	};

}
