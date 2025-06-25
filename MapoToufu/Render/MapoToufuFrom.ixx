module;

export module MapoToufuForm;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;

export namespace MapoToufu::Form
{
	struct FormContext
	{

	};


	struct FormConfig
	{

	};

	struct FormInitConfig
	{
		std::int32_t layer = 0;
		std::int32_t primary_priority = 0;
	};

	bool InitFormSystem(Instance& instance, FormInitConfig const& config = {});


	Entity::Ptr CreateForm(Instance& instance, FormConfig const& config = {});


};