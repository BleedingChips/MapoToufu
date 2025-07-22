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

	SystemNode::Ptr RendererFunction_Flush()
	{
		static auto commited_system = AutoSystemNodeStatic(
			[](
				Context& context,
				AutoComponentQuery<Form> comp_q,
				AutoSingletonQuery<FrameRenderer, PassDistributor> single_q
				)
			{
				auto render_query = single_q.Query(context);

				/*
				comp_q.Foreach(context,
					[&](decltype(comp_q)::Data& output) -> bool {
						auto span = output.GetSpan<0>();
						for (auto& ite : span)
						{
							if (ite.form_wrapper)
							{
								//ite.form_wrapper->LogicPresent();
								//ite.form_wrapper->Present();
							}
							if (ite.event_hook)
								ite.event_hook->UpdateEventHook(context, ite);
						}
						return true;
					}
				);
				*/
				
				if (render_query.has_value())
				{
					auto render = render_query->GetPointer<0>();
					if (render != nullptr )
					{
						render->CommitFrame();
					}
				}

				if (render_query.has_value())
				{
					auto render = render_query->GetPointer<0>();
					if (render != nullptr)
					{
						render->FlushFrame();
					}
					auto pass = render_query->GetPointer<1>();
					if (pass != nullptr)
					{
						pass->CleanRequest();
					}
				}

				
				comp_q.Foreach(context,
					[&](decltype(comp_q)::Data& output) -> bool {
						auto span = output.GetSpan<0>();
						for (auto& ite : span)
						{
							if (ite.form_wrapper)
							{
								ite.form_wrapper->LogicPresent();
								ite.form_wrapper->Present();
							}
							if (ite.event_hook)
								ite.event_hook->UpdateEventHook(context, ite);
						}
						return true;
					}
				);
				

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
			//f_renderer.frame_renderer = ;
			if (instance.AddSingleton(std::move(f_renderer)))
			{

				{
					auto flush_sys_index = instance.PrepareSystemNode(RendererFunction_Flush());
					assert(flush_sys_index);
					SystemNode::Parameter sys_parameter;
					sys_parameter.module_name = module_name;
					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;
					sys_parameter.priority.second = std::numeric_limits<decltype(sys_parameter.priority.second)>::max();
					sys_parameter.priority.third = std::numeric_limits<decltype(sys_parameter.priority.second)>::max();
					sys_parameter.name = L"MapoToufuRenderer::Flush";
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();
					auto exe_index = instance.LoadSystemNode(SystemCategory::Tick, flush_sys_index, sys_parameter);
					assert(exe_index.has_value() && *exe_index);
				}

				/*
				{
					auto flush_sys_index = instance.PrepareSystemNode(IGHUDPass::GetPassSystem());
					assert(flush_sys_index);
					SystemNode::Parameter sys_parameter;
					sys_parameter.module_name = module_name;
					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;
					sys_parameter.priority.second = std::numeric_limits<decltype(sys_parameter.priority.second)>::max();
					sys_parameter.priority.third = std::numeric_limits<decltype(sys_parameter.priority.second)>::max() - 1;
					sys_parameter.name = IGHUDPass::GetPassName();
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();
					auto exe_index = instance.LoadSystemNode(SystemCategory::Tick, flush_sys_index, sys_parameter);
					assert(exe_index.has_value() && *exe_index);
				}
				*/

				PassDistributor distributor;
				Dumpling::PassScription scription;
				scription.pass_name = CleanViewTargetPass::GetPassName();
				scription.default_parameter = Potato::IR::StructLayoutObject::DefaultConstruct(
					Potato::IR::StructLayout::GetStatic<CleanViewTargetPass::Property>()
				);
				distributor.RegisterPass(std::move(scription));
				instance.AddSingleton(std::move(distributor));
				
				{
					auto flush_sys_index = instance.PrepareSystemNode(CleanViewTargetPass::GetPassSystem());
					assert(flush_sys_index);

					SystemNode::Parameter sys_parameter;

					sys_parameter.module_name = module_name;

					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;

					sys_parameter.priority.second = 1;
					sys_parameter.name = CleanViewTargetPass::GetPassName();
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();

					instance.LoadSystemNode(SystemCategory::Tick, flush_sys_index, sys_parameter);
				}

				{
					auto flush_sys_index = instance.PrepareSystemNode(IGHUDPass::GetPassSystem());
					assert(flush_sys_index);

					SystemNode::Parameter sys_parameter;

					sys_parameter.module_name = module_name;

					sys_parameter.layer = init_config.priority.layer;
					sys_parameter.priority.primary = init_config.priority.primary_priority;

					sys_parameter.priority.second = 1;
					sys_parameter.name = IGHUDPass::GetPassName();
					sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();

					instance.LoadSystemNode(SystemCategory::Tick, flush_sys_index, sys_parameter);
				}

			}
		}
	}

	void RendererModule::Init(GameContext& context)
	{
		Potato::Task::Node::Parameter parameter;
		parameter.acceptable_mask = static_cast<std::size_t>(ThreadMask::MainThread);
		parameter.custom_data.data1 = 10;
		parameter.node_name = L"MapoToufuRenderer::FormMessageLoop";
		parameter.trigger_time = Potato::Task::TimeT::now() + std::chrono::milliseconds{ parameter.custom_data.data1 };
		context.GetTaskContext().Commit(*this, parameter);
	}

	void RendererModule::UnLoad(Context& context)
	{

	}

	/*
	bool RendererModule::AddFormComponent(Instance& instance, Entity& target_entity, FormConfig form_config)
	{
		if (instance.AddEntityComponent(target_entity, Form{}))
		{
			SystemNode::Parameter para;
			para.acceptable_mask = *ThreadMask::MainThread;
			para.layer = init_config.priority.layer;
			para.priority.primary = init_config.priority.primary_priority;
			para.priority.second = std::numeric_limits<decltype(para.priority.second)>::min();

			auto index = instance.LoadOnceSystemNode(
				CreateAutoSystemNode([entity = Entity::Ptr{ &target_entity }, renderer = renderer, form_config = std::move(form_config)](Context& context, AutoComponentQuery<Form> comp) {
					auto data = comp.QueryEntity(context, *entity);
					auto form = data->GetPointer<0>();
					Dumpling::Form::Config config;
					config.title = form_config.title;
					config.style = form_config.style;
					config.rectangle = form_config.rectangle;
					config.event_hook = form_config.event_hook;
					if (!config.event_hook)
					{
						auto hook = DefaultFormEventHoot::Create();
						config.event_hook = hook;
						form->event_hook = hook;
					}
					form->platform_form = Dumpling::Form::Create(config);
					form->form_wrapper = renderer->CreateFormWrapper(form->platform_form);
					//form->form_wrapper->LogicPresent();
					}),
				para
			);
			assert(index);

			return true;
		}
		return false;
	}

	bool RendererModule::AddIGHUDComponent(Instance& instance, Entity& target_entity, Dumpling::IGWidget::Ptr weight)
	{
		if (instance.AddEntityComponent(target_entity, IGHud{}))
		{
			SystemNode::Parameter para;
			para.acceptable_mask = *ThreadMask::MainThread;
			para.layer = init_config.priority.layer + 1;
			para.priority.primary = init_config.priority.primary_priority;
			para.priority.second = std::numeric_limits<decltype(para.priority.second)>::min() + 1;

			auto index = instance.LoadOnceSystemNode(
				CreateAutoSystemNode([entity = Entity::Ptr{ &target_entity }, renderer = renderer, weight = std::move(weight)](Context& context, AutoComponentQuery<Form, IGHud> comp) {
					auto data = comp.QueryEntity(context, *entity);
					auto hud = data->GetPointer<1>();
					auto form = data->GetPointer<0>();
					hud->hud = Dumpling::IGHeadUpDisplay::Create(*form, std::move(weight));
					//form->form_wrapper->LogicPresent();
					}),
				para
			);
			assert(index);

			return true;
		}
		return false;
	}
	*/
}