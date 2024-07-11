import std;
import Potato;
import Dumpling;
import Noodles;

using namespace Potato;
using namespace Dumpling;
using namespace Noodles;


struct DefaultNoodleContext : public Context
{
	void AddContextRef() const override {}
	void SubContextRef() const override {}
};

struct TopFormEventResponder : public Dumpling::FormEventCapture
{
	void AddFormEventCaptureRef() const override {}
	void SubFormEventCaptureRef() const override {}
	FormEvent::Category AcceptedCategory() const override { return FormEvent::Category::MODIFY; }
	FormEvent::Respond Receive(Form& interface, FormEvent::Modify event) override
	{
		if(event.message == decltype(event.message)::DESTROY)
		{
			Form::PostFormQuitEvent();
		}
		return FormEvent::Respond::PASS;
	}
};


int main()
{

	DefaultNoodleContext n_context;

	TopFormEventResponder respond;
	auto form = Form::Create();
	form->InsertCapture(&respond);

	FormProperty pro;
	pro.title = u8"NoodlesDemoGame";


	Task::TaskContext context;

	context.AddGroupThread({1}, 1);
	auto requireID = context.GetRandomThreadIDFromGroup(1);
	std::promise<void> pro1;

	auto fur1 = pro1.get_future();

	Task::TaskFilter filter{Task::Priority::High, Task::Category::THREAD_TASK, 0, *requireID};

	n_context.CreateAndAddAtomaticSystem(
		[](Noodles::ExecuteContext& ex_context)
		{
			while(
				Form::PeekMessageEventOnce([&](FormEvent::System event)
					{
						if(event.message == FormEvent::System::Message::QUIT)
						{
							ex_context.noodles_context.Quit();
						}
					}
				)
				)
				{
					
				}
		},
		SystemNodeProperty{
			Priority{},
			Property{u8"form_message", u8"gro"},
			nullptr,
			filter
		}
	);

	auto init_task = Task::Task::CreateLambdaTask([&](Task::ExecuteStatus& status, Task::Task::Ptr)
	{
		form->Init(pro);
		pro1.set_value();
	});

	context.CommitTask(init_task, Task::TaskProperty{
		u8"init form",
		{},
		filter
	});

	fur1.get();

	n_context.Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	

	return 0;



	return 0;
}