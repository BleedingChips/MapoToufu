module;

#include <cassert>

module MapoToufuForm;
import std;

struct TopFormEventResponder : public Dumpling::FormEventCapture
{
	void AddFormEventCaptureRef() const override {}
	void SubFormEventCaptureRef() const override {}

	TopFormEventResponder() : FormEventCapture(Dumpling::FormEvent::Category::MODIFY) {}

	Dumpling::FormEvent::Respond Receive(Dumpling::Form& interface, Dumpling::FormEvent::Modify event) override
	{
		if(event.message == decltype(event.message)::DESTROY)
		{
			Dumpling::Form::PostQuitEvent();
		}
		return Dumpling::FormEvent::Respond::PASS;
	}
}DefaultRespond;

namespace MapoToufu
{
	std::future<FormTuple> CreateForm(Potato::Task::TaskContext& context, FormProperty property, Device& device,  std::thread::id thread_id, bool IsTopForm, std::size_t identity, std::pmr::memory_resource* resource)
	{
		if(thread_id != std::thread::id{})
		{
			auto form = Dumpling::Form::Create(identity, resource);
			if(form)
			{
				if(IsTopForm)
				{
					form->InsertCapture(&DefaultRespond);
				}
				form->InsertCapture(property.capture);
				std::promise<FormTuple> promise;
				auto future = promise.get_future();
				auto task = Potato::Task::Task::CreateLambdaTask([
					form, 
					pro = std::move(promise), 
					property = std::move(property),
					device_ptr = Device::Ptr{&device},
					resource
				](Potato::Task::TaskContextWrapper& status, Potato::Task::Task& self) mutable {
					if(form->Init(property, resource))
					{
						auto wrapper = device_ptr->CreateFormWrapper(*form);
						auto hud = Dumpling::Gui::CreateHUD(*form, *device_ptr, property.widget, resource);
						if(wrapper && hud)
						{
							pro.set_value(
								{
									std::move(form),
									std::move(wrapper),
									std::move(hud)
								}
							);
							return;
						}
					}
					pro.set_value({});
				}, resource);
				if(!task)
				{
					promise.set_value({});
				}else{
					Potato::Task::TaskFilter filter{Potato::Task::Priority::High, Potato::Task::Category::THREAD_TASK, 0, thread_id};
					Potato::Task::TaskProperty task_property{
						property.title,
						{},
						filter
					};
					auto re = context.CommitDelayTask(std::move(task), std::chrono::microseconds{10}, task_property);
					assert(re);
				}
				return future;
			}
		}
		return {};
	}

	bool CommitedFormMessageLoop(Scene::Ptr reference_scene, Potato::Task::TaskContext& context, std::thread::id id, MessageLoopInitProperty property, std::pmr::memory_resource* resource)
	{
		auto task = Potato::Task::Task::CreateLambdaTask([reference_scene, property](Potato::Task::TaskContextWrapper& wrapper, Potato::Task::Task& this_task)
		{
			bool NeedExit = false;
			while(
				Dumpling::Form::PeekMessageEventOnce([&](Dumpling::FormEvent::System event)
					{
						if(event.message == Dumpling::FormEvent::System::Message::QUIT)
						{
							if(reference_scene)
							{
								reference_scene->Quit();
								NeedExit = true;
							}
						}
					}
				)
			)
			{
			
			}
			if(!NeedExit)
			{
				wrapper.context.CommitTask(&this_task, wrapper.task_property);
			}
		}, resource);
		Potato::Task::TaskFilter filter{property.priority, Potato::Task::Category::THREAD_TASK, 0, id};
		Potato::Task::TaskProperty task_property{
			property.name.name,
			{},
			filter
		};
		return context.CommitTask(std::move(task), task_property);
	}
}



