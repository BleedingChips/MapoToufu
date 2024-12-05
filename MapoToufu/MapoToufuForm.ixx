module;

export module MapoToufuForm;

import std;
import Scene;
import Potato;
import Dumpling;
import MapoToufuRenderer;


export namespace MapoToufu
{

	using Noodles::SystemName;

	export struct FormModule;

	struct Form : protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<Form>;

		using Property = Dumpling::FormProperty;
		
	protected:

		friend struct Potato::Pointer::DefaultIntrusiveWrapper;

		std::shared_mutex mutex;
		Dumpling::Form::Ptr form_ptr;
		Dumpling::FormWrapper::Ptr wrapper;
	};

	struct FormModule : protected Potato::Task::Task, protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<FormModule>;

		struct Config
		{
			std::chrono::steady_clock::duration duration = std::chrono::milliseconds{10};
			Potato::Task::Priority priority = Potato::Task::Priority::Normal;
			std::u8string_view task_name = u8"FormSystem";
		};

		static Ptr Create(RendererModule::Ptr system, Potato::Task::TaskContext& context, std::thread::id require_thread_id, Config config, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		bool RegisterSystem(Scene& scene, std::u8string_view group_name, Noodles::SystemNodeProperty base_property);
		Form::Ptr CreateForm(Form::Property init_property, std::size_t identity);

	protected:

		friend struct Potato::Pointer::DefaultIntrusiveWrapper;

		RendererModule::Ptr renderer;
		std::chrono::steady_clock::duration duration;

		std::mutex mutex;

		struct InitForm
		{
			Form::Ptr target_form;
			Form::Property init_property;
		};

		std::pmr::vector<InitForm> init_forms;
	};

	/*
	struct FormProperty : public Dumpling::FormProperty
	{
		Widget::Ptr widget;
		FormEventCapture::Ptr capture;
	};

	struct FormTuple
	{
		Form::Ptr form;
		FormWrapper::Ptr wrapper;
		HeadUpDisplay::Ptr hud;
	};

	std::future<FormTuple> CreateForm(Potato::Task::TaskContext& context, FormProperty property, Device& device,  std::thread::id thread_id, bool IsTopForm = true, std::size_t identity = 0, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

	struct MessageLoopInitProperty
	{
		Noodles::SystemName name = {u8"Form Message Loop"};
		Potato::Task::Priority priority = Potato::Task::Priority::Normal;
	};

	bool CommitedFormMessageLoop(Scene::Ptr reference_scene, Potato::Task::TaskContext& context, std::thread::id id, MessageLoopInitProperty property = {}, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
	*/

}
