import std;
import Potato;
import Dumpling;
import Noodles;
import FormManager;
import RendererManager;
import Scene;
import DumplingImGui;


int main()
{

	auto scene = Scene::Create();
	auto top_form = CreateTopForm();

	FormInitProperty pro;
	pro.title = u8"NoodlesDemoGame";

	Potato::Task::TaskContext context;

	context.AddGroupThread({1}, 1);
	context.AddGroupThread({2}, 6);
	auto requireID = context.GetRandomThreadIDFromGroup(1);

	auto fu = InitForm(*top_form, context, *requireID, pro);

	auto val = fu.get();

	auto hard_device = Dumpling::Device::Create();


	auto form_wrapper = hard_device->CreateFormWrapper(*top_form);

	Dumpling::InitImGui(*hard_device);

	RegisterFormMessageSystem(*scene, *requireID, {0, 1, 0});

	scene->CreateAndAddTickedAutomaticSystem(
		[=](SceneWrapper& context){
			std::cout << "sdasdasd" << std::endl;
		},
		{u8"123"}
	);

	scene->Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	

	return 0;
}