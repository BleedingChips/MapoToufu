#include <cassert>
#include "d3d12.h"

import std;
import Potato;
import Dumpling;
import Noodles;
import FormManager;
import RendererManager;
import Scene;
import DumplingImGui;


struct A {};
struct B {};


std::mutex P;

struct Printer
{
	char const* Str = nullptr;
	Printer(char const* Str) : Str(Str)
	{
		std::lock_guard lg(P);
		std::cout << Str << std::endl;
	}
	~Printer()
	{
		std::lock_guard lg(P);
		std::cout << Str << std::endl;
	}
};

int main()
{
	Dumpling::Device::InitDebugLayer();
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

	form_wrapper->LogicPresent();

	auto im_context = Dumpling::Gui::CreateHUD(*top_form, *hard_device, Dumpling::Gui::Widget::DemoWidget());

	CommitedFormMessageLoop(scene, context, *requireID);

	Dumpling::Color color {0.0f, 0.0f, 0.0f, 1.0f};

	auto Fun = [](float color, float speed, float time) -> float
	{
		return std::fmod(color + speed * time, 2.0f);
	};

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicUserModify<A>)
		{
			Printer P{"Pass"};
			color.R =  Fun(color.R, 0.2f, context.GetContext().GetFramedDurationInSecond());
			color.G =  Fun(color.G, 0.3f, context.GetContext().GetFramedDurationInSecond());
			color.B =  Fun(color.B, 0.4f, context.GetContext().GetFramedDurationInSecond());

			Dumpling::Color new_color {color};

			new_color.R = std::abs(new_color.R - 1.0f);
			new_color.G = std::abs(new_color.G - 1.0f);
			new_color.B = std::abs(new_color.B - 1.0f);

			Dumpling::PassRenderer render;
			frame_renderer->PopPassRenderer(render);

			Dumpling::RendererTargetCarrier carrier;
			carrier.AddRenderTarget(*form_wrapper);

			render.SetRenderTargets(carrier);
			render.ClearRendererTarget(carrier, 0, new_color);
			im_context->Commited(render);
			frame_renderer->FinishPassRenderer(render);
		},
		{u8"Pass"}, {0, 1, 0}
	);

	std::size_t frame_index = frame_renderer->GetCurrentFrame();

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicUserModify<B>)
		{
			Printer P{"Flush"};

			frame_renderer->FlushToLastFrame(std::chrono::microseconds{1});
			
			form_wrapper->Present();
		},
		{u8"Flush"}, {0, 1, 0}
	);

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicUserModify<A>, Noodles::AtomicUserModify<B>)
		{
			Printer P{"Commited"};
			frame_index = *frame_renderer->CommitFrame();
			form_wrapper->LogicPresent();
		},
		{u8"Commited"}, {0, 0, 0}
	);

	scene->Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	frame_renderer->FlushToLastFrame();

	return 0;
}