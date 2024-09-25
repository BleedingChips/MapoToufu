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

	auto im_context = Dumpling::ImGuiContext::Create(*hard_device);
	auto im_form_wrapper = im_context->CreateFormWrapper(*top_form);

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
			render.ClearRendererTarget(*form_wrapper, new_color);
			im_context->StartFrame();
			im_form_wrapper->StartFrame();
			im_form_wrapper->EndFrame();
			im_context->EndFrame();
			

			auto desc = form_wrapper->GetDescription(D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET);
			if(desc.resource_ptr)
			{
				if(desc.default_state != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
				{
					D3D12_RESOURCE_BARRIER barrier{
						D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
						D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE,
						D3D12_RESOURCE_TRANSITION_BARRIER{
							desc.resource_ptr.Get(),
							D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
							desc.default_state,
							D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET
						}
					};
					render->ResourceBarrier(1, &barrier);
				}
				render->OMSetRenderTargets(1, &(desc.cpu_handle), false, nullptr);
				auto heap = im_context->GetHeap();
				render->SetDescriptorHeaps(1, &heap);
				im_context->Commited(render);


				if(desc.default_state != D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET)
				{
					D3D12_RESOURCE_BARRIER barrier{
						D3D12_RESOURCE_BARRIER_TYPE::D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
						D3D12_RESOURCE_BARRIER_FLAGS::D3D12_RESOURCE_BARRIER_FLAG_NONE,
						D3D12_RESOURCE_TRANSITION_BARRIER{
							desc.resource_ptr.Get(),
							D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
							D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET,
							desc.default_state
						}
					};
					render->ResourceBarrier(1, &barrier);
				}
			}
			frame_renderer->FinishPassRenderer(render);
		},
		{u8"Pass"}, {0, 1, 0}
	);

	std::size_t frame_index = frame_renderer->GetCurrentFrame();

	scene->CreateAndAddTickedAutomaticSystem(
		[&](SceneWrapper& context, Noodles::AtomicUserModify<B>)
		{
			Printer P{"Flush"};
			while(true)
			{
				auto flush_frame = frame_renderer->TryFlushFrame();
				if(flush_frame >= frame_index)
				{
					break;
				}else
				{
					std::this_thread::sleep_for(std::chrono::microseconds{1});
				}
			}
			
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
	
	

	return 0;
}