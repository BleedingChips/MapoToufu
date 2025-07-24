module;
#include <cassert>
module MapoToufuRenderModule;

import MapoToufuGameContext;


namespace MapoToufu
{

	struct RendererModuleDefaultImplement : 
		public RendererModule, 
		public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		virtual void AddModuleRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		virtual void SubModuleRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		virtual void AddTaskNodeRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		virtual void SubTaskNodeRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		RendererModuleDefaultImplement(Potato::IR::MemoryResourceRecord record, Config config) : RendererModule(config), MemoryResourceRecordIntrusiveInterface(record) {}
	};

	auto RendererModule::Create(Config config) -> Ptr
	{
		Dumpling::Device::InitDebugLayer();
		auto render = Dumpling::Device::Create();
		if (render)
		{
			auto re = Potato::IR::MemoryResourceRecord::Allocate<RendererModuleDefaultImplement>(config.resource);
			if (re)
			{
				return new(re.Get()) RendererModuleDefaultImplement{ re, config };
			}
		}
		return {};
	}

	RendererModule::RendererModule(Config config)
		: init_config(config)
	{
		Dumpling::Device::InitDebugLayer();
		renderer = Dumpling::Device::Create();
	}

	SystemNode::Ptr RendererFunction_Wait()
	{
		static auto wait_system = AutoSystemNodeStatic(
			[](
				Context& context,
				AutoSingletonQuery<FrameRenderer const> singleton
				)
			{
				auto [render] = singleton.Query(context)->GetPointerTuple();

				if (render == nullptr)
					return;

				render->WaitLastFrameFinish();

			});
		return &wait_system;
	}

	SystemNode::Ptr RendererFunction_Commited()
	{
		static auto commited_system = AutoSystemNodeStatic(
			[](
				Context& context,
				AutoComponentQuery<Form> component,
				AutoSingletonQuery<FrameRenderer, PassDistributor> singleton
				)
			{
				auto [render, distributor] = singleton.Query(context)->GetPointerTuple();

				if (render == nullptr || distributor == nullptr)
					return;
				
				component.Foreach(context, [&](decltype(component)::Data& data) -> bool {
					auto form = data.GetSpan<0>();
					for (auto& ite : form)
					{
						if (ite.form_wrapper)
						{
							if (ite.need_skip_first_present)
							{
								ite.need_skip_first_present = false;
							}
							else {
								ite.form_wrapper->Present();
							}
							ite.form_wrapper->LogicPresent();
						}

						if (ite.event_hook)
						{
							ite.event_hook->UpdateEventHook(context, ite);
						}
					}
					return true;
					});

				render->CommitFrame();
				distributor->CleanRequest();
				
			});
		return &commited_system;
	}

	void RendererModule::TaskExecute(Potato::Task::Context& context, Parameter& parameter)
	{
		while (true)
		{
			auto re = Dumpling::Form::PeekMessageEventOnce(
				[this](Dumpling::FormEvent& event) -> Dumpling::FormEvent::Respond
				{
					if(init_config.enable_imgui_context)
						return Dumpling::IGHeadUpDisplay::FormEventHook(event);
					return event.RespondMarkAsSkip();
				}
			);
			if (re.has_value())
			{
				if (*re)
				{
					continue;
				}
				else {
					parameter.trigger_time =
						std::chrono::steady_clock::now()
						+ std::chrono::milliseconds{ parameter.custom_data.data1 };
					context.Commit(*this, parameter);
					return;
				}
			}
			else {
				return;
			}
		}
	}

	void RendererModule::Load(Instance& instance)
	{
		if (renderer)
		{
			FrameRenderer f_renderer{ renderer->CreateFrameRenderer(), renderer };
			if (instance.AddSingleton(std::move(f_renderer)))
			{
				auto wait_sys_index = instance.PrepareSystemNode(RendererFunction_Wait());
				assert(wait_sys_index);
				auto commited_sys_index = instance.PrepareSystemNode(RendererFunction_Commited());
				assert(commited_sys_index);

				SystemNode::Parameter sys_parameter;
				sys_parameter.module_name = module_name;
				sys_parameter.layer = init_config.priority.layer;
				sys_parameter.priority.primary = init_config.priority.primary_priority;
				sys_parameter.priority.second = std::numeric_limits<decltype(sys_parameter.priority.second)>::max();
				sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();

				{
					sys_parameter.name = L"MapoToufuRenderer::Wait";
					sys_parameter.priority.third = std::numeric_limits<decltype(sys_parameter.priority.second)>::max() - 1;
					
					auto exe_index = instance.LoadSystemNode(SystemCategory::Tick, wait_sys_index, sys_parameter);
					assert(exe_index.has_value() && *exe_index);
				}

				{
					sys_parameter.name = L"MapoToufuRenderer::Commite";
					sys_parameter.priority.third = std::numeric_limits<decltype(sys_parameter.priority.second)>::max();

					auto exe_index = instance.LoadSystemNode(SystemCategory::Tick, commited_sys_index, sys_parameter);
					assert(exe_index.has_value() && *exe_index);
				}

				PassDistributor distributor;

				{
					Dumpling::PassScription scription;
					scription.pass_name = CleanViewTargetPass::GetPassName();
					scription.default_parameter = Potato::IR::StructLayoutObject::DefaultConstruct(
						Potato::IR::StructLayout::GetStatic<CleanViewTargetPass::Property>()
					);
					distributor.RegisterPass(std::move(scription));
				}

				{
					Dumpling::PassScription scription;
					scription.pass_name = IGHUDPass::GetPassName();
					scription.default_parameter = Potato::IR::StructLayoutObject::DefaultConstruct(
						Potato::IR::StructLayout::GetStatic<IGHUDPass::Property>()
					);
					distributor.RegisterPass(std::move(scription));
				}
				
				instance.AddSingleton(std::move(distributor));
				
				{
					auto clean_view_target = instance.PrepareSystemNode(CleanViewTargetPass::GetPassSystem());
					assert(clean_view_target);
					SystemNode::Parameter sys_parameter;
					sys_parameter.module_name = module_name;
					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;
					sys_parameter.name = CleanViewTargetPass::GetPassName();
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();
					instance.LoadSystemNode(SystemCategory::Tick, clean_view_target, sys_parameter);
				}

				{
					auto IGhud = instance.PrepareSystemNode(IGHUDPass::GetPassSystem());
					assert(IGhud);
					SystemNode::Parameter sys_parameter;
					sys_parameter.module_name = module_name;
					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;
					sys_parameter.name = IGHUDPass::GetPassName();
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();
					instance.LoadSystemNode(SystemCategory::Tick, IGhud, sys_parameter);
				}
			}
		}
	}

	void RendererModule::Init(GameContext& context)
	{
		Potato::Task::Node::Parameter parameter;
		parameter.acceptable_mask = static_cast<std::size_t>(ThreadMask::MainThread);
		parameter.custom_data.data1 = 1;
		parameter.node_name = L"MapoToufuRenderer::FormMessageLoop";
		parameter.trigger_time = Potato::Task::TimeT::now() + std::chrono::milliseconds{ parameter.custom_data.data1 };
		context.GetTaskContext().Commit(*this, parameter);
	}

	void RendererModule::UnLoad(Context& context)
	{

	}
}