module;

#include <cassert>

module FormManager;
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

Dumpling::Form::Ptr CreateTopForm(std::size_t identity, std::pmr::memory_resource* resource)
{
	auto ptr = Dumpling::Form::Create(identity);
	if(ptr)
	{
		ptr->InsertCapture(&DefaultRespond);
	}
	return ptr;
}

std::future<bool> InitFormInThread(Dumpling::Form& target_format,  Potato::Task::TaskContext& context, std::thread::id require_thread_id, FormInitProperty property, std::pmr::memory_resource* resource)
{
	std::promise<bool> promise;
	auto future = promise.get_future();
	auto task = Potato::Task::Task::CreateLambdaTask([
		form = Dumpling::Form::Ptr{&target_format}, 
		pro = std::move(promise), 
		property
	](Potato::Task::TaskContextWrapper& status, Potato::Task::Task& self) mutable {
		pro.set_value(form->Init(property));
	}, resource);
	if(!task)
	{
		promise.set_value(false);
	}else{
		Potato::Task::TaskFilter filter{property.priority, Potato::Task::Category::THREAD_TASK, 0, require_thread_id};
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
			wrapper.context.CommitDelayTask(&this_task, property.duration_time, wrapper.task_property);
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
