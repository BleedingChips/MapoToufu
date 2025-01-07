module;

export module MapoToufuScene;

import std;
import Noodles;
import Potato;
import Dumpling;

export namespace MapoToufu
{

	using Noodles::AutoComponentQuery;
	using Noodles::AutoSingletonQuery;
	using Noodles::ThreadOrderQuery;

	using SceneWrapper = Noodles::ContextWrapper;
	using Entity = Noodles::Entity;
	using SystemNode = Noodles::SystemNode;

	struct Scene : public Noodles::Context, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<Scene>;

		static Ptr Create(Noodles::StructLayoutManager& manager, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		Scene(Noodles::StructLayoutManager& manager, Potato::IR::MemoryResourceRecord record);

		template<typename FuncT>
		SystemNode::Ptr CreateAutoSystem(FuncT&& func, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
		template<typename FuncT>
		SystemNode::Ptr CreateAndAddAutoSystem(FuncT&& func, Noodles::Property property = {}, std::pmr::memory_resource * resource = std::pmr::get_default_resource());

	protected:

		void AddContextRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubContextRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
	};

	template<typename FuncT>
	SystemNode::Ptr Scene::CreateAutoSystem(FuncT&& func, std::pmr::memory_resource* resource)
	{
		return Noodles::CreateAutomaticSystem(*manager, std::forward<FuncT>(func), resource);
	}

	template<typename FuncT>
	SystemNode::Ptr Scene::CreateAndAddAutoSystem(FuncT&& func, Noodles::Property property, std::pmr::memory_resource* resource)
	{
		return Noodles::CreateAndAddAutomaticSystem(*this, std::forward<FuncT>(func), std::move(property), resource);
	}

	
};

