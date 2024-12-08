module;

export module MapoToufuForm;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;
import MapoToufuRenderer;


export namespace MapoToufu
{

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

	struct FormModule : public ModuleInterface, protected Potato::Task::Task, protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<FormModule, ModuleInterface::Wrapper>;

		struct Config
		{
			std::chrono::steady_clock::duration duration = std::chrono::milliseconds{10};
			Potato::Task::Priority priority = Potato::Task::Priority::Normal;
			std::u8string_view task_name = u8"FormSystem";
		};

		StructLayout::Ptr GetStructLayout() const override { return StructLayout::GetStatic<FormModule>(); }

		static Ptr Create(Config config, std::pmr::memory_resource* resource = std::pmr::get_default_resource());
		bool PostRegister(GameContextWrapper& context) override;

		Form::Ptr CreateForm(Form::Property init_property, std::size_t identity);

	protected:

		FormModule(Potato::IR::MemoryResourceRecord record, Config config);

		void AddModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		void AddTaskRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubTaskRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		void TaskExecute(Potato::Task::TaskContextWrapper& status) override;

		friend struct Potato::Pointer::DefaultIntrusiveWrapper;

		std::shared_mutex property_mutex;
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
