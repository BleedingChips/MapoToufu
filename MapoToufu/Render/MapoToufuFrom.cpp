module;
#include <cassert>
module MapoToufuForm;

namespace MapoToufu::Form
{
	Entity::Ptr CreateForm(Instance& instance, FormConfig const& config)
	{
		auto entity = instance.CreateEntity();
		if (entity)
		{

			instance.AddEntityComponent(*entity, Dumpling::Form{});

			/*
			auto system = CreateAutoSystemNode([entity, config = config](Context& context, AutoComponentQuery<Dumpling::Form> query, AutoSingletonQuery<Dumpling::>) {
				
			});
			*/

			//assert(system);

			//instance


			//instance.AddEntityComponent(*entity, Dumpling::Form);
		}
		return {};
	}



	/*
	void InitFormSystem(Instance& instance, FormInitConfig const& config)
	{
		
	}
	*/

	

};