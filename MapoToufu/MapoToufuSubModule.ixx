module;

export module MapoToufuSubModule;

import std;
import Potato;
import Dumpling;
import MapoToufuDefine;

export namespace MapoToufu
{
	struct GameContext;
	struct SubModuleCollection;


	struct ModulePriority
	{
		std::int32_t layer = 0;
		std::int32_t primary_priority = 0;
	};


	struct SubModule
	{
		struct Wrapper
		{
			void AddRef(SubModule const* ptr) { return ptr->AddSubModuleRef(); }
			void SubRef(SubModule const* ptr) { return ptr->SubSubModuleRef(); }
		};

		using Ptr = IntrusivePtr<SubModule, Wrapper>;

		virtual void Init(struct GameContext& context) {}
		virtual void Destory(GameContext& context) {}
		virtual bool ShouldLoad(Instance const& target_instance, InstanceConfig const& config) const { return false; }
		virtual void Load(GameContext& context, Instance& instance, InstanceConfig const& config, SubModuleCollection const& Collection) {}
		virtual void UnLoad(Context& context, SubModuleCollection const& Collection) {}
		virtual StructLayout const& GetStructLayout() const = 0;

	protected:

		virtual void AddSubModuleRef() const = 0;
		virtual void SubSubModuleRef() const = 0;
		friend struct Wrapper;
	};

	struct SubModuleDescription
	{
		std::wstring_view name;
	};

	struct SubModuleCollection
	{
		template<typename Type>
		Potato::Pointer::ObserverPtr<SubModule> GetSubModule() const { return GetSubModule(*StructLayout::GetStatic<Type>()); }

		Potato::Pointer::ObserverPtr<SubModule> GetSubModule(StructLayout const& require_layuout) const;

		bool RegisterSubModule(SubModule& sub_module, SubModuleDescription const& description = {});

	protected:

		struct Element
		{
			std::size_t hash_id;
			StructLayout::Ptr layout;
			SubModule::Ptr sub_module;
		};

		std::vector<Element> sub_modules;

		friend struct GameContext;
	};
}
