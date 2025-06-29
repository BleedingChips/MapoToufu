module;
#include <cassert>
module MapoToufuRenderModule;

import MapoToufuGameContext;
import MapoToufuDefine;

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
		: priority(config.priority)
	{
		Dumpling::Device::InitDebugLayer();
		renderer = Dumpling::Device::Create();
	}

	SystemNode::Ptr RendererFunction_Commited()
	{
		static auto commited_system = AutoSystemNodeStatic(
			[](
				Context& context,
				AutoComponentQuery<Form> comp_q,
				AutoSingletonQuery<FrameRenderer> single_q
				)
			{

				auto render_query = single_q.Query(context);
				if (render_query.has_value())
				{
					auto render = render_query->GetPointer<0>();
					if (render != nullptr )
					{
						render->CommitFrame();
					}
				}

				comp_q.Foreach(context,
					[](decltype(comp_q)::Data& output) -> bool {
						auto span = output.GetSpan<0>();
						for (auto& ite : span)
						{
							if (ite.form_wrapper)
							{
								ite.form_wrapper->LogicPresent();
							}
						}
						return false;
					}
				);
			});
		return &commited_system;
	}

	SystemNode::Ptr RendererFunction_Flush()
	{
		static auto commited_system = AutoSystemNodeStatic(
			[](
				Context& context,
				AutoComponentQuery<Form, EntityProperty> comp_q,
				AutoSingletonQuery<FrameRenderer> single_q
				)
			{
				comp_q.Foreach(context,
					[&](decltype(comp_q)::Data& output) -> bool {
						for (auto ite : output)
						{
							auto [form, property] = ite;
							if (form != nullptr)
							{
								if (form->form_wrapper)
									form->form_wrapper->Present();
								if (form->event)
									form->event->Update(context, *property->GetEntity(), *form);
							}
						}
						return true;
					}
				);

				auto render_query = single_q.Query(context);
				if (render_query.has_value())
				{
					auto render = render_query->GetPointer<0>();
					if (render != nullptr)
					{
						render->FlushFrame();
					}
				}
			});
		return &commited_system;
	}

	SystemNode::Ptr RendererFunction_Dispatch()
	{
		static auto dispatch_system = AutoSystemNodeStatic(
			[](Context& context, AutoSingletonQuery<FrameRenderer> single_q)
			{
				if (auto outputr = single_q.Query(context))
				{
					if (auto [frame_renderer] = outputr->GetPointerTuple(); frame_renderer != nullptr)
					{

					}
				}
			});
		return &dispatch_system;
	}

	void RendererModule::TaskExecute(Potato::Task::Context& context, Parameter& parameter)
	{
		while (true)
		{
			auto re = Dumpling::Form::PeekMessageEventOnce(
				[](void* data, HWND hwnd, UINT uint, WPARAM wpar, LPARAM lpar) -> HRESULT
				{
					auto ptr = static_cast<RendererModule*>(data);
					std::shared_lock sm(ptr->event_capture_mutex);
					for (auto& ite : ptr->captures)
					{
						HRESULT re = ite->RespondEvent(hwnd, uint, wpar, lpar);
						if (FormEventCapture::IsRespondMarkAsCaptured(re, hwnd, uint, wpar, lpar))
						{
							return re;
						}
					}
					return FormEventCapture::RespondMarkAsSkip(hwnd, uint, wpar, lpar);
				},
				this
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
			FrameRenderer f_renderer{ renderer->CreateFrameRenderer() };
			//f_renderer.frame_renderer = ;
			if (instance.AddSingleton(std::move(f_renderer)))
			{
				auto commited_sys_index = instance.PrepareSystemNode(RendererFunction_Commited());
				assert(commited_sys_index);
				auto flush_sys_index = instance.PrepareSystemNode(RendererFunction_Flush());
				assert(flush_sys_index);
				auto dispatch_sys_index = instance.PrepareSystemNode(RendererFunction_Dispatch());
				assert(dispatch_sys_index);

				SystemNode::Parameter sys_parameter;
				
				sys_parameter.module_name = module_name;
				
				sys_parameter.layer = priority.layer;
				sys_parameter.priority.primary = priority.primary_priority;

				sys_parameter.priority.second = 1;
				sys_parameter.name = L"MapoToufuRenderer::Commited";
				//sys_parameter.acceptable_mask = *ThreadMask::MainThread;

				instance.LoadSystemNode(SystemCategory::Tick, commited_sys_index, sys_parameter);

				/*
				sys_parameter.priority.second = 2;
				sys_parameter.name = L"MapoToufuRenderer::Dispatch";
				sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();

				instance.LoadSystemNode(SystemCategory::Tick, dispatch_sys_index, sys_parameter);
				*/

				sys_parameter.priority.second = 100;
				sys_parameter.name = L"MapoToufuRenderer::Flush";
				sys_parameter.acceptable_mask = std::numeric_limits<decltype(sys_parameter.acceptable_mask)>::max();
				
				instance.LoadSystemNode(SystemCategory::Tick, flush_sys_index, sys_parameter);
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

	bool RendererModule::AddFormComponent(Instance& instance, Entity& target_entity)
	{
		if (instance.AddEntityComponent(target_entity, Form{}))
		{
			auto sys_index = instance.PrepareSystemNode(
				CreateAutoSystemNode([entity = Entity::Ptr{ &target_entity }, renderer = renderer](Context& context, AutoComponentQuery<Form> comp) {
					auto data = comp.QueryEntity(context, *entity);
					auto form = data->GetPointer<0>();
					Dumpling::Form::Config config;
					form->event = FormEventCapture::Create({});
					config.event_capture = form->event;
					form->platform_form = Dumpling::Form::Create(config);
					form->form_wrapper = renderer->CreateFormWrapper(form->platform_form);
					form->form_wrapper->LogicPresent();
					})
				, { true });

			SystemNode::Parameter para;
			para.acceptable_mask = *ThreadMask::MainThread;
			instance.LoadSystemNode(SystemCategory::OnceNextFrame, sys_index, para);
			return true;
		}
		return false;
	}

	void RendererModule::AddPass(Instance& instance, Instance::SystemIndex index, SystemNode::Parameter parameter)
	{
		parameter.layer = priority.layer;
		parameter.priority.primary = priority.primary_priority;
		parameter.priority.second = 5;

		instance.LoadSystemNode(SystemCategory::Tick, index, std::move(parameter));
	}

	/*
	FormEventStorage::Ptr FormEventStorage::Create(std::pmr::memory_resource* resource)
	{
		auto ir = Potato::IR::MemoryResourceRecord::Allocate<FormEventStorage>(resource);
		if (ir)
		{
			return new(ir.Get()) FormEventStorage{ir};
		}
		return {};
	}

	FormEvent::Respond FormEventStorage::RespondEvent(FormEvent event)
	{
		std::lock_guard lg(receive_mutex);
		receive_events.emplace_back(std::move(event), false);
		return FormEvent::Respond::PASS;
	}

	void FormEventStorage::SwapReceiveEvent()
	{
		std::lock(respond_mutex, receive_mutex);
		std::lock_guard lg1(respond_mutex, std::adopt_lock);
		std::lock_guard lg2(receive_mutex, std::adopt_lock);
		std::swap(respond_events, receive_events);
		receive_events.clear();
	}


	

	RendererModule::RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer)
		: MemoryResourceRecordIntrusiveInterface(record), config(config), renderer(std::move(renderer))
	{
		
	}

	void RendererModule::Renderer_FlushFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_form, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const>)
	{
		if (c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				ptr->frame_renderer->FlushToLastFrame();
			}
		}

		std::size_t ite = 0;
		while (c_form.IterateComponent(wrapper, ite++))
		{
			auto span = c_form.AsSpan<Form>();
			for (auto& ite : span)
			{
				ite.form_wrapper->Present();
			}
		}
		
	}

	void RendererModule::Renderer_DispatchPass(SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const>)
	{
		if(c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				for (auto& ite : ptr->reference_node)
				{
					wrapper.AddTemporarySystemNode(*ite, {});
				}
				ptr->reference_node.clear();
			}
		}
	}

	void RendererModule::Renderer_CommitedFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_filter, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer>)
	{
		if (c_renderer.GetSingletons(wrapper))
		{
			auto ptr = c_renderer.Get<FrameRenderer>();
			if (ptr != nullptr)
			{
				ptr->frame_renderer->CommitFrame();
			}
		}

		std::size_t ite = 0;
		while (c_filter.IterateComponent(wrapper, ite++))
		{
			auto span = c_filter.AsSpan<Form>();
			for (auto& ite : span)
			{
				ite.form_wrapper->LogicPresent();
				if(ite.is_primary)
				{
					ite.ForeachEvent([&](FormEvent const& event) ->FormEvent::Respond
						{
							if(event.IsModify())
							{
								auto modify = event.GetModify();
								if(modify.message == decltype(modify.message)::DESTROY)
								{
									wrapper.GetContext().Quit();
								}
							}
							return FormEvent::Respond::PASS;
						}
						);
				}
				ite.event_storage->SwapReceiveEvent();
			}
		}
	}

	bool RendererModule::CreateRenderer(Scene& scene)
	{
		FrameRenderer f_renderer;
		f_renderer.frame_renderer = renderer->CreateFrameRenderer();
		if (scene.AddSingleton(std::move(f_renderer)))
		{
			scene.CreateAndAddAutoSystem(
				Renderer_CommitedFrame,
				{
					{ config.render_layer, config.render_priority_first, config.render_priority_second, 0 },
					u8"renderer_module%commit_frame"
				}
			);


			scene.CreateAndAddAutoSystem(
				Renderer_DispatchPass,
				{
				{ config.render_layer, config.render_priority_first, config.render_priority_second, 1 },
				u8"renderer_module%dispatch_pass"
				});


			scene.CreateAndAddAutoSystem(
				Renderer_FlushFrame,
				{
					{config.render_layer, config.render_priority_first, config.render_priority_second, 2 },
					u8"renderer_module%flush_frame"
				}
			);

			return true;
		}
		return false;
	}

	bool RendererModule::CreateForm(Entity& entity, Scene& scene, FormConfig const& config)
	{

		Dumpling::Form::Config real_config = config;
		auto storage = FormEventStorage::Create();
		real_config.event_capture = storage;

		auto form = Dumpling::Form::Create(std::move(real_config));
		if(form)
		{
			auto wrapper = renderer->CreateFormWrapper(form, {});
			if(wrapper)
			{
				wrapper->LogicPresent();
				Form real_form;
				real_form.form = std::move(form);
				real_form.is_primary = config.is_primary;
				real_form.event_storage = std::move(storage);
				real_form.form_wrapper = wrapper;
				if (scene.AddEntityComponent(entity, std::move(real_form)))
				{
					return true;
				}
			}
			form.DestroyForm();
		}
		return false;
	}
	*/
}