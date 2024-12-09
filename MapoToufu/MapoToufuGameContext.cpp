module;

#include <cassert>

module MapoToufuGameContext;
import std;


namespace MapoToufu
{

	Scene::Ptr GameContext::CreateScene()
	{
		return Scene::Create(scene_resource);
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



