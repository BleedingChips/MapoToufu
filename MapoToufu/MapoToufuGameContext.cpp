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
		std::size_t count = 0;
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
			task_context.ExecuteContextThreadOnce(context, std::chrono::steady_clock::now(), GetMainLoopGroupID());
			std::this_thread::yield();
			count += 1;
			if (count >= 100)
			{
				if (task_context.CheckNodeSequencerEmpty())
					continue_loop = false;
			}
		}
		task_context.ExecuteContextThreadUntilNoExistTask(GetMainLoopGroupID());
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



