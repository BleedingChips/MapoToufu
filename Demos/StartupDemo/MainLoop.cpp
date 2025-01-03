#include <cassert>

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufu;

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

using namespace MapoToufu;

int main()
{

	GameContext context;

	auto renderer_module = RendererModule::Create({});


	auto scene = context.CreateScene();

	renderer_module->CreateRenderer(*scene);

	auto ent = scene->CreateEntity();

	renderer_module->CreateForm(*ent, *scene);

	Dumpling::Color color{ 0.0f, 0.0f, 0.0f, 1.0f };

	auto Fun = [](float color, float speed, float time) -> float
		{
			return std::fmod(color + speed * time, 2.0f);
		};

	auto ptr = scene->CreateAutomaticSystem([&](SceneWrapper& context, AtomicComponentFilter<Form> c_form, AtomicSingletonFilter<FrameRenderer> s_renderer)
	{
			if (s_renderer.GetSingletons(context))
			{
				auto ptr = s_renderer.Get<FrameRenderer>();
				std::size_t index = 0;
				if (ptr != nullptr)
				{
					while ( c_form.IterateComponent(context, index++))
					{
						auto span = c_form.AsSpan<Form>();
						Dumpling::PassRenderer renderer;
						ptr->frame_renderer->PopPassRenderer(renderer);

						color.R = Fun(color.R, 0.2f, context.GetContext().GetFramedDurationInSecond());
						color.G = Fun(color.G, 0.3f, context.GetContext().GetFramedDurationInSecond());
						color.B = Fun(color.B, 0.4f, context.GetContext().GetFramedDurationInSecond());

						Dumpling::Color new_color{ color };

						new_color.R = std::abs(new_color.R - 1.0f);
						new_color.G = std::abs(new_color.G - 1.0f);
						new_color.B = std::abs(new_color.B - 1.0f);

						for (auto& ite : span)
						{
							Dumpling::RenderTargetSet carrier;
							carrier.AddRenderTarget(*ite.form_wrapper);

							renderer.SetRenderTargets(carrier);
							renderer.ClearRendererTarget(0, new_color);
						}
						ptr->frame_renderer->FinishPassRenderer(renderer);
					}
				}
			}
	});

	scene->CreateAndAddTickedAutomaticSystem([&](SceneWrapper& context, AtomicSingletonFilter<FrameRenderer> s_renderer)
	{
		if (s_renderer.GetSingletons(context))
		{
			auto s_ptr = s_renderer.Get<FrameRenderer>();
			if (s_ptr != nullptr)
			{
				s_ptr->reference_node.push_back(ptr);
			}
		}
	}, { {0, 1, 1, 1} });

	context.Launch(*scene);
	context.Loop();

	volatile int i = 0;


	/*
	Dumpling::Device::InitDebugLayer();


	Potato::Task::TaskContext context;
	context.AddGroupThread({ 1 }, 1);
	context.AddGroupThread({ 2 }, 6);
	auto requireID = *context.GetRandomThreadIDFromGroup(1);

	auto renderer_module = MapoToufu::RendererModule::Create({});
	auto form_module = MapoToufu::FormModule::Create(renderer_module, context, requireID, {});
	
	auto scene = Scene::Create();
	auto frame_renderer = renderer_module->CreateFrameRenderer();
	scene->AddSingleton(std::move(frame_renderer));

	auto from = form_module->CreateForm({}, 0);

	auto ent = scene->CreateEntity(std::move(from));

	
	auto hard_device = Dumpling::Device::Create();

	

	MapoToufu::CommitedFormMessageLoop(scene, context, requireID);

	MapoToufu::FormProperty property;
	property.title = u8"NoodlesDemoGame";
	property.widget = MapoToufu::Widget::GetDemoWidget();

	auto form_future = MapoToufu::CreateForm(context, property, *hard_device, requireID);
	auto form = form_future.get();

	auto frame_renderer = hard_device->CreateFrameRenderer();

	form.wrapper->LogicPresent();

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
		[&](SceneWrapper& context, Noodles::AtomicThreadOrder<A>)
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
			carrier.AddRenderTarget(*form.wrapper);

			render.SetRenderTargets(carrier);
			render.ClearRendererTarget(0, new_color);
			form.hud->Commited(render);
			frame_renderer->FinishPassRenderer(render);
		},
		{u8"Pass"}, {0, 1, 0}
	);

	std::size_t frame_index = frame_renderer->GetCurrentFrame();

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicThreadOrder<B>)
		{
			Printer P{"Flush"};

			frame_renderer->FlushToLastFrame(std::chrono::microseconds{1});
			
			form.wrapper->Present();
		},
		{u8"Flush"}, {0, 1, 0}
	);

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicThreadOrder<A, B>)
		{
			Printer P{"Commited"};
			frame_index = *frame_renderer->CommitFrame();
			form.wrapper->LogicPresent();
		},
		{u8"Commited"}, {0, 0, 0}
	);

	auto tem = scene->CreateAutomaticSystemTemplate([]()
	{
		Printer P{"FuckYou"};
	});

	auto node = scene->CreateSystem(*tem);
	scene->AddTickedSystemNode(node, { u8"Fuck" }, {});

	scene->Commited(context, {});

	context.ProcessTaskUntillNoExitsTask({});
	
	frame_renderer->FlushToLastFrame();

	return 0;
	*/
}