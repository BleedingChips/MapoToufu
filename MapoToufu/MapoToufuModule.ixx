module;

export module MapoToufuModule;

import std;
import Potato;
import Dumpling;
import MapoToufuDefine;

export namespace MapoToufu
{

	struct ModulePriority
	{
		std::int32_t layer = 0;
		std::int32_t primary_priority = 0;
	};


	struct Module
	{
		struct Wrapper
		{
			void AddRef(Module const* ptr) { return ptr->AddModuleRef(); }
			void SubRef(Module const* ptr) { return ptr->SubModuleRef(); }
		};

		using Ptr = Potato::Pointer::IntrusivePtr<Module, Wrapper>;

		virtual void Init(struct GameContext& context) {}
		virtual void Destory(GameContext& context) {}
		virtual void Load(Instance& instance) {}
		virtual void UnLoad(Context& context) {}

	protected:

		virtual void AddModuleRef() const = 0;
		virtual void SubModuleRef() const = 0;
		friend struct Wrapper;
	};
}
