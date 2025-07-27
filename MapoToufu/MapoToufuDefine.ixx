module;

export module MapoToufuDefine;

import std;
import Potato;
import Noodles;
import Dumpling;

export namespace MapoToufu
{
	enum class ThreadMask : std::size_t
	{
		MainThread = 1,
	};

	constexpr std::size_t operator*(ThreadMask mask) { return static_cast<std::size_t>(mask); }

	using Noodles::AutoComponentQuery;
	using Noodles::AutoSingletonQuery;

	using Instance = Noodles::Instance;
	using Context = Noodles::Context;
	using Entity = Noodles::Entity;
	using SystemNode = Noodles::SystemNode;
	using Noodles::CreateAutoSystemNode;
	using SystemCategory = Noodles::SystemCategory;
	using SystemInitializer = Noodles::SystemInitializer;
	using Noodles::AutoSystemNodeStatic;
	using AutoComponentQuertIterator = Noodles::AutoComponentQueryIterator;
	using EntityProperty = Noodles::EntityProperty;

	using Dumpling::FormEvent;
	using Dumpling::PassSequencer;
	using Dumpling::PassRequest;
	using Dumpling::PassIndex;
	using Dumpling::PassDistributor;
	using Dumpling::IGWidget;
	using Dumpling::RendererResource;

	using Potato::Pointer::IntrusivePtr;

};

