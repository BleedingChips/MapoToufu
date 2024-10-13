module;

export module MapoToufuForm;

import std;
import Scene;
import Potato;
import Dumpling;
import DumplingImGui;


export namespace MapoToufu
{
	using Dumpling::Form;
	using Dumpling::FormWrapper;
	using Dumpling::FormEventCapture;
	using Dumpling::Device;
	using Dumpling::Gui::HeadUpDisplay;
	using Dumpling::Gui::Widget;

	using Noodles::SystemName;

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


}
