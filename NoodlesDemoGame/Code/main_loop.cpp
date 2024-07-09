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

struct TopFormEventResponder : public Dumpling::FormEventResponder
{
	void AddFormEventResponderRef() const override {}
	void SubFormEventResponderRef() const override {}
	FormEventRespond Respond(Form& interface, FormEvent event) override
	{
		if(event.message == FormEventEnum::DESTROY)
		{
			Form::PostFormQuitEvent();
		}
		return FormEventRespond::Default;
	}
};


int main()
{

	DefaultNoodleContext n_context;

	TopFormEventResponder respond;
	auto form = Form::Create(&respond);

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
				Form::PeekMessageEventOnce([&](Form*, FormEvent event, FormEventRespond)
					{
						if(event.message == FormEventEnum::QUIT)
						{
							ex_context.noodles_context.Quit();
						}
						return FormEventRespond::Default;
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