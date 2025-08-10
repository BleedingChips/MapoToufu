module;

module MapoToufuSubModule;

namespace MapoToufu
{
	Potato::Pointer::ObserverPtr<SubModule> SubModuleCollection::GetSubModule(StructLayout const& require_layuout) const
	{
		for (auto& sub_module : sub_modules)
		{
			if (sub_module.hash_id == require_layuout.GetHashCode() && *sub_module.layout == require_layuout)
			{
				return sub_module.sub_module;
			}
		}
		return nullptr;
	}

	bool SubModuleCollection::RegisterSubModule(SubModule& sub_module, SubModuleDescription const& description)
	{
		Element new_sub_module;
		new_sub_module.layout = &sub_module.GetStructLayout();
		new_sub_module.hash_id = new_sub_module.layout->GetHashCode();
		new_sub_module.sub_module = &sub_module;
		sub_modules.emplace_back(std::move(new_sub_module));
		return true;
	}
}
