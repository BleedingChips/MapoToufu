#include <cassert>

import std;
import Potato;
import Dumpling;
import Noodles;
import FormManager;
import RendererManager;
import Scene;
import DumplingImGui;
import DumplingHLSLComplier;

struct A {};
struct B {};


std::mutex P;

struct Printer
{
	char const* Str = nullptr;
	Printer(char const* Str) : Str(Str)
	{
		//std::lock_guard lg(P);
		//std::cout << Str << std::endl;
	}
	~Printer()
	{
		//std::lock_guard lg(P);
		//std::cout << Str << std::endl;
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

	auto im_context = Dumpling::Gui::CreateHUD(*top_form, *hard_device, Dumpling::Gui::Widget::GetDemoWidget());

	CommitedFormMessageLoop(scene, context, *requireID);

	Dumpling::Color color {0.0f, 0.0f, 0.0f, 1.0f};

	auto Fun = [](float color, float speed, float time) -> float
	{
		return std::fmod(color + speed * time, 2.0f);
	};

	auto shader_context = Dumpling::HLSLCompiler::Context::Create();

	std::cout << std::filesystem::current_path() << std::endl;

	auto tar = Potato::Path::ReverseRecursiveSearchDirectory(L"Content");
	std::filesystem::current_path(tar);


	std::cout << std::filesystem::current_path() << std::endl;

	Potato::Document::BinaryStreamReader reader2(L"Material/TestShader.raw_shader");
	auto stream_size = reader2.GetStreamSize();
	std::vector<std::byte> temp{stream_size};
	reader2.Read(std::span(temp));
	auto ite_str = std::span(temp);
	Potato::Document::StringSerializer serializer;
	auto ite_str2 = serializer.ConsumeBom(ite_str);
	auto code = *serializer.TryDirectCastToStringView<char8_t>(ite_str2);

	Dumpling::HLSLCompiler::Target vs_target
	{
		Dumpling::HLSLCompiler::Target::VS,
		u8"VS"
	};

	Dumpling::HLSLCompiler::Target ps_target
	{
		Dumpling::HLSLCompiler::Target::VS,
		u8"PS"
	};

	auto i = shader_context->Compile(code, vs_target, u8"F11");
	auto error = i.GetErrorMessage();
	auto p = shader_context->Compile(code, ps_target, u8"F22");
	auto error2 = p.GetErrorMessage();

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

			Dumpling::RenderTargetSet carrier;
			carrier.AddRenderTarget(*form_wrapper);

			render.SetRenderTargets(carrier);
			render.ClearRendererTarget(0, new_color);
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

	auto tem = scene->CreateAutomaticSystemTemplate([]()
	{
		Printer P{"FuckYou"};
	});

	auto node = scene->CreateSystem(*tem, {u8"Fcck"});
	scene->AddTickedSystemNode(*node, { });

	scene->Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	frame_renderer->FlushToLastFrame();

	return 0;
}