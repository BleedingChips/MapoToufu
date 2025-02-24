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

	/*
	auto ptr2 = scene->CreateAndAddAutoSystem([](SceneWrapper& wrapper, AutoComponentQuery<Form> comp_query)
		{
			if (comp_query.IterateComponent(wrapper, 0))
			{
				auto span = comp_query.AsSpan<Form>();
				for (auto& ite : span)
				{
					ite.event_storage->ForeachEvent([&](MapoToufu::FormEvent& event)-> MapoToufu::FormEvent::Respond
					{
						if (event.IsSystem())
						{
							auto sys = event.GetSystem();
							if (sys.message == decltype(sys.message)::QUIT)
							{
								wrapper.GetContext().Quit();
							}
						}else if (event.IsModify())
						{
							auto modify = event.GetModify();
							if (modify.message == decltype(modify.message)::DESTROY)
							{
								wrapper.GetContext().Quit();
							}
						}
						return MapoToufu::FormEvent::Respond::PASS;
					});
				}
			}
		}, {{0, 1, 2, 3}, {u8"event_func"}});
		*/

	auto ptr = scene->CreateAutoSystem([&](SceneWrapper& wrapper, AutoComponentQuery<Form> comp_query, AutoSingletonQuery<FrameRenderer> sing_query)
	{
			//auto k = context.GetContext().CreateAutomaticSystem([](AtomicComponentFilter<Form>) {});
			//context.AddTemporarySystemNode(*k, {});
		if (sing_query.GetSingletons(wrapper))
		{
			if (auto render_ptr = sing_query.Get<FrameRenderer>(); render_ptr != nullptr)
			{
				std::size_t index = 0;
				while (comp_query.IterateComponent(wrapper, index++))
				{
					auto span = comp_query.AsSpan<Form>();
					Dumpling::PassRenderer renderer;
					render_ptr->frame_renderer->PopPassRenderer(renderer);

					color.R = Fun(color.R, 0.2f, wrapper.GetContext().GetFramedDurationInSecond());
					color.G = Fun(color.G, 0.3f, wrapper.GetContext().GetFramedDurationInSecond());
					color.B = Fun(color.B, 0.4f, wrapper.GetContext().GetFramedDurationInSecond());

					Dumpling::Color new_color{ color };

					new_color.R = std::abs(new_color.R - 1.0f);
					new_color.G = std::abs(new_color.G - 1.0f);
					new_color.B = std::abs(new_color.B - 1.0f);

					for (auto& ite : span)
					{
						Dumpling::RenderTargetSet carrier;
						carrier.AddRenderTarget(*ite.GetResource());

						renderer.SetRenderTargets(carrier);
						renderer.ClearRendererTarget(0, new_color);
					}
					render_ptr->frame_renderer->FinishPassRenderer(renderer);
				}
			}
		}
	});

	scene->CreateAndAddAutoSystem([&](SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> s_renderer)
	{
		if (s_renderer.GetSingletons(wrapper))
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