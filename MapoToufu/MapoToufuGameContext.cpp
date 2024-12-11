module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{


	GameContext::GameContext(Config in_config)
		: config(std::move(in_config)), current_thread_id(std::this_thread::get_id())
	{
		renderer.Init(config.renderer_resource);
	}

	void GameContext::Loop()
	{
		assert(current_thread_id == std::this_thread::get_id());
		bool need_loop = true;
		while (need_loop)
		{
			while(true)
			{
				auto re = Dumpling::Form::PeekMessageEventOnce();
				if (re.has_value())
				{
					if (!*re)
						break;
				}
				else
				{
					need_loop = false;
					break;
				}
			}
		}
	}


	Scene::Ptr GameContext::CreateScene()
	{
		return Scene::Create(config.scene_resource);
	}

	ModuleInterface::Ptr GameContext::FindModule(StructLayout const& layout) const
	{
		std::shared_lock lg(module_mutex);
		for (auto& Ite : modules)
		{
			auto& [ptr, module] = Ite;
			if (ptr.GetPointer() == &layout || *ptr == layout)
			{
				return module;
			}
		}
		return {};
	}

	bool GameContext::RegisterModule(ModuleInterface::Ptr ptr)
	{
		if (ptr)
		{
			{
				std::lock_guard lg(module_mutex);
				modules.emplace_back(ptr->GetStructLayout(), ptr);
			}
			ptr->PostRegister(*this);
		}
		return false;
	}
}



