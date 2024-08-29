module;

export module FormManager;

export import Scene;
export import Potato;
export import Dumpling;


export{
	Dumpling::Form::Ptr CreateTopForm(std::size_t identity = 0, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
	
	struct FormInitProperty : Dumpling::FormProperty
	{
		Potato::Task::Priority priority = Potato::Task::Priority::Normal;
	};

	std::future<bool> InitForm(Dumpling::Form& target_format,  Potato::Task::TaskContext& context, std::thread::id require_thread_id, FormInitProperty property = {}, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

	struct MessageLoopInitProperty
	{
		Noodles::SystemName name = {u8"Form Message Loop"};
		Potato::Task::Priority priority = Potato::Task::Priority::Normal;
	};

	Noodles::SystemNode::Ptr RegisterFormMessageSystem(Scene& scene, std::thread::id id, Noodles::Priority priority, MessageLoopInitProperty property = {}, Noodles::OrderFunction fun = nullptr, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

};
