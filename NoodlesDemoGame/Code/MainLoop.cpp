import std;
import Potato;
import Dumpling;
import Noodles;
import FormManager;
import RendererManager;
import Scene;



int main()
{

	auto scene = Scene::Create();
	auto top_form = CreateTopForm();

	FormInitProperty pro;
	pro.title = u8"NoodlesDemoGame";

	Potato::Task::TaskContext context;

	context.AddGroupThread({1}, 1);
	auto requireID = context.GetRandomThreadIDFromGroup(1);

	auto fu = InitForm(*top_form, context, *requireID, pro);

	auto val = fu.get();

	auto hard_device = Dumpling::HardDevice::Create();
	auto renderer = Dumpling::Renderer::Create();

	auto form_wrapper = renderer->CreateFormWrapper(*hard_device, *top_form);

	RegisterFormMessageSystem(*scene, *requireID, {0, 1, 0});

	scene->Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	

	return 0;
}