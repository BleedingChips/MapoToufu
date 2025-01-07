module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{


	GameContext::GameContext(Config in_config)
		: config(std::move(in_config))
	{
		task_context.AddGroupThread({}, std::thread::hardware_concurrency());
		manager = Noodles::StructLayoutManager::Create();
	}

	void GameContext::Loop()
	{
		Potato::Task::Context::ThreadExecuteContext context;
		bool continue_loop = true;
		while(continue_loop)
		{
			while(true)
			{
				auto re = Dumpling::Form::PeekMessageEventOnce();
				if(re)
				{
					if(!*re)
						break;
				}else
				{
					continue_loop = false;
				}
			}
			task_context.ExecuteContextThreadOnce(context, std::chrono::steady_clock::now());
			std::this_thread::yield();
		}
	}

	Scene::Ptr GameContext::CreateScene()
	{
		auto ptr = Scene::Create(*manager, config.scene_resource);
		return ptr;
	}

	bool GameContext::Launch(Scene& scene)
	{
		return scene.Commited(task_context, {});
	}
}



