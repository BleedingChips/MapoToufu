module;

export module MapoToufuInstance;

import std;
import Noodles;
import Potato;
import Dumpling;

export namespace MapoToufu
{

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
};

