module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{

	GameContext::GameContext()
	{
		if (std::thread::hardware_concurrency() >= 1)
		{
			Potato::Task::ThreadProperty thread_property;
			thread_property.acceptable_mask = ~static_cast<std::size_t>(ThreadMask::MainThread);
			task_context.CreateThreads(std::thread::hardware_concurrency() - 1, thread_property);
		}
	}

	void GameContext::RunMainThreadAndWait()
	{
		Potato::Task::ThreadProperty thread_property;
		thread_property.acceptable_mask = static_cast<std::size_t>(ThreadMask::MainThread);
		task_context.ExecuteContextThreadUntilNoExistTask(thread_property);
	}

	Instance::Ptr GameContext::CreatInstance(InstanceConfig config)
	{
		auto ptr = Instance::Create(config);
		return ptr;
	}

	bool GameContext::Launch(Instance& scene)
	{
		return scene.Commit(task_context);
	}
}



