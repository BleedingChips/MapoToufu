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

	bool GameContext::RegisterModule(SubModule& sub_module)
	{
		std::lock_guard lock(module_mutex);
		if (collection.RegisterSubModule(sub_module))
		{
			sub_module.Init(*this);
			return true;
		}
		return false;
	}

	SystemNode::Ptr GameContext::EndSubModuleSystemNode()
	{
		static auto node = AutoSystemNodeStatic(
			[](Context& context, AutoSingletonQuery<SubModuleCollection> instance) {
				auto [collection] = instance.Query(context)->GetPointerTuple();
				if (collection != nullptr)
				{
					for (auto& ite : collection->sub_modules)
					{
						ite.sub_module->UnLoad(context, *collection);
					}
				}
			}
		);
		return &node;
	}

	Instance::Ptr GameContext::CreatInstance(InstanceConfig config)
	{
		auto ptr = Instance::Create(config);
		if (ptr)
		{
			SubModuleCollection instance_collection;

			{
				std::shared_lock lock(module_mutex);
				for (auto& ite : collection.sub_modules)
				{
					if (ite.sub_module->ShouldLoad(*ptr, config))
					{
						instance_collection.RegisterSubModule(*ite.sub_module);
					}
				}
			}

			for (auto& ite : instance_collection.sub_modules)
			{
				ite.sub_module->Load(*this, *ptr, config, instance_collection);
			}

			ptr->AddSingleton(std::move(instance_collection));

			Instance::SystemInfo info;
			info.identity_name = L"MapoToufu::EndSubModuleSystemNode";

			auto system_index = ptr->PrepareSystemNode(EndSubModuleSystemNode(), info, false);
			assert(system_index);

			SystemNode::Parameter par;
			par.module_name = L"MapoToufu::GameContext::UnloadSubmodule";

			ptr->LoadDyingSystemNode(system_index, par);
		}
		return ptr;
	}

	bool GameContext::Launch(Instance& scene)
	{
		return scene.Commit(task_context);
	}
}



