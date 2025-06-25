module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{

	GameContext::GameContext()
	{
		if (std::thread::hardware_concurrency() >= 2)
		{
			Potato::Task::ThreadProperty thread_property;
			thread_property.acceptable_mask = static_cast<std::size_t>(ThreadMask::PlatformThread);
			task_context.CreateThreads(1, thread_property);
		}

		if (std::thread::hardware_concurrency() >= 3)
		{
			Potato::Task::ThreadProperty thread_property;
			thread_property.acceptable_mask = ~static_cast<std::size_t>(ThreadMask::PlatformThread);
			task_context.CreateThreads(std::thread::hardware_concurrency() - 2, thread_property);
		}
		
	}

	void GameContext::Loop()
	{
		Potato::Task::ThreadProperty thread_property;
		thread_property.acceptable_mask = ~static_cast<std::size_t>(ThreadMask::PlatformThread);
		task_context.ExecuteContextThreadUntilNoExistTask(thread_property);
	}

	Instance::Ptr GameContext::CreatInstance(Instance::Config config)
	{
		auto ptr = Instance::Create(config);
		return ptr;
	}

	bool GameContext::Launch(Instance& scene)
	{
		return scene.Commit(task_context);
	}
}



