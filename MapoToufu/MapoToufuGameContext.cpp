module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{


	GameContext::GameContext(Config in_config)
		: config(std::move(in_config))
	{
	}

	void GameContext::Loop()
	{
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
			task_context.ProcessTaskOnce({});
			std::this_thread::yield();
		}
	}

	Scene::Ptr GameContext::CreateScene()
	{
		auto ptr = Scene::Create(config.scene_resource);
		return ptr;
	}

	bool GameContext::Launch(Scene& scene)
	{
		return scene.Commited(task_context, {});
	}
}



