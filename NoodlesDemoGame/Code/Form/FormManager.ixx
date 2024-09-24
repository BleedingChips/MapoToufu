module;

export module FormManager;

import std;
export import Scene;
export import Potato;
export import Dumpling;


export{
	Dumpling::Form::Ptr CreateTopForm(std::size_t identity = 0, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
	
	struct FormInitProperty : Dumpling::FormProperty
	{
		Potato::Task::Priority priority = Potato::Task::Priority::Normal;
	};

	std::future<bool> InitFormInThread(Dumpling::Form& target_format,  Potato::Task::TaskContext& context, std::thread::id require_thread_id, FormInitProperty property = {}, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

	struct MessageLoopInitProperty
	{
		Noodles::SystemName name = {u8"Form Message Loop"};
		Potato::Task::Priority priority = Potato::Task::Priority::Normal;
		std::chrono::steady_clock::duration duration_time = std::chrono::microseconds{1};
	};

	bool CommitedFormMessageLoop(Scene::Ptr reference_scene, Potato::Task::TaskContext& context, std::thread::id id, MessageLoopInitProperty property = {}, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
};
