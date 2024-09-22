#include <cassert>

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

	auto fu = InitFormInThread(*top_form, context, *requireID, pro);

	auto val = fu.get();

	auto hard_device = Dumpling::Device::Create();


	auto form_wrapper = hard_device->CreateFormWrapper(*top_form);
	auto frame_renderer = hard_device->CreateFrameRenderer();

	Dumpling::InitImGui(*hard_device);

	RegisterFormMessageSystemInThread(*scene, *requireID, {0, 1, 0});

	Dumpling::Color color {0.0f, 0.0f, 0.0f, 1.0f};
	std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context)
		{
			color.R +=  0.001f * context.GetContext().GetFramedDurationInSecond();
			color.G +=  0.0015f * context.GetContext().GetFramedDurationInSecond();
			color.B +=  0.002f * context.GetContext().GetFramedDurationInSecond();
			Dumpling::PassRenderer render;
			frame_renderer->PopPassRenderer(render);
			render.ClearRendererTarget(*form_wrapper, color);
			frame_renderer->FinishPassRenderer(render);
			auto new_frame = frame_renderer->CommitFrame();
			assert(new_frame.has_value());
			form_wrapper->LogicalNextFrame();
		},
		{u8"Clear"}
	);

	std::size_t frame_index = 0;

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, std::size_t current_frame)
		{
			while(true)
			{
				auto index = frame_renderer->TryFlushFrame();
				if(index >= frame_index)
				{
					if(frame_index != 0)
					{
						form_wrapper->Present();
					}
					++frame_index;
					break;
				}else
				{
					std::this_thread::sleep_for(std::chrono::microseconds{10});
				}
			}
		}, {u8"flush"}
	);

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