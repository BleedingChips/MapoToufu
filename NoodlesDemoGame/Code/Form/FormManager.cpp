module;

#include <cassert>

module FormManager;

struct TopFormEventResponder : public Dumpling::FormEventCapture
{
	void AddFormEventCaptureRef() const override {}
	void SubFormEventCaptureRef() const override {}
	Dumpling::FormEvent::Category AcceptedCategory() const override { return Dumpling::FormEvent::Category::MODIFY; }
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

std::future<bool> InitForm(Dumpling::Form& target_format,  Potato::Task::TaskContext& context, std::thread::id require_thread_id, FormInitProperty property, std::pmr::memory_resource* resource)
{
	std::promise<bool> promise;
	auto future = promise.get_future();
	auto task = Potato::Task::Task::CreateLambdaTask([
		form = Dumpling::Form::Ptr{&target_format}, 
		pro = std::move(promise), 
		property
	](Potato::Task::ExecuteStatus& status, Potato::Task::Task& self) mutable {
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
		auto re = context.CommitTask(std::move(task), task_property);
		assert(re);
	}
	return future;
}

Noodles::SystemNode::Ptr RegisterFormMessageSystem(Scene& scene, std::thread::id require_thread_id, Noodles::Priority priority, MessageLoopInitProperty property, Noodles::OrderFunction fun, std::pmr::memory_resource* resource)
{
	if(require_thread_id != std::thread::id{})
	{
		Potato::Task::TaskFilter filter{property.priority, Potato::Task::Category::THREAD_TASK, 0, require_thread_id};
		Potato::Task::TaskProperty task_property{
			property.name.name,
			{},
			filter
		};
		auto sys = scene.CreateAutomaticSystem(
			[](SceneWrapper& context){
				while(
					Dumpling::Form::PeekMessageEventOnce([&](Dumpling::FormEvent::System event)
						{
							if(event.message == Dumpling::FormEvent::System::Message::QUIT)
							{
								context.Quit();
							}
						}
					)
					)
					{
					
					}
			}, {u8"FormMessageLoop"}, resource
		);
		auto re = scene.AddTickedSystemNode(*sys, {priority, fun, filter});
		assert(re);
		return sys;
	}
	return {};
}
