module;

module MapoToufuScene;

namespace MapoToufu
{
	Scene::Ptr GameContext::CreateScene()
	{
		return Scene::Create(scene_resource);
	}

	ModuleInterface::Ptr GameContext::FindModule_AssumedLocked(StructLayout const& layout) const
	{
		for(auto& Ite : modules)
		{
			auto& [ptr, module] = Ite;
			if(ptr.GetPointer() == &layout || *ptr == layout)
			{
				return module;
			}
		}
		return {};
	}

	bool GameContext::RegisterModule(ModuleInterface::Ptr ptr)
	{
		if(ptr)
		{
			std::lock_guard lg(module_mutex);
			GameContextWrapper wrap{ *this };
			modules.emplace_back(ptr->GetStructLayout(), ptr);
			if(!ptr->PostRegister(wrap))
			{
				return true;
			}
			modules.pop_back();
		}
		return false;
	}
}

