module;

export module MapoToufuRenderModule;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuRenderer;
import MapoToufuModule;

export namespace MapoToufu
{

	

	struct RendererModule : public Module, public Potato::Task::Node
	{
		static constexpr std::wstring_view module_name = L"MapoTouFuRenderer";

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule, Module::Wrapper>;

		struct Config
		{
			ModulePriority priority;
			std::pmr::memory_resource* resource = std::pmr::get_default_resource();
		};

		static Ptr Create(Config config = {});

		bool AddFormComponent(Instance& instance, Entity& target_entity);
		void AddPass(Instance& instance, Instance::SystemIndex index, SystemNode::Parameter parameter = {});

		virtual void Init(GameContext& context) override;
		virtual void Load(Instance& instance) override;
		virtual void UnLoad(Context& context) override;

	protected:

		virtual void TaskExecute(Potato::Task::Context& context, Parameter& parameter) override;

		RendererModule(Config config);

		ModulePriority priority;
		Dumpling::Device::Ptr renderer;

		std::shared_mutex event_capture_mutex;
		std::pmr::vector<Dumpling::FormEventCapture::Ptr> captures;

		friend struct RendererModule;
	};






	/*
	export struct RendererModule : public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{

		struct Config
		{
			std::int32_t render_layer = 0;
			std::int32_t render_priority_first = 0;
			std::int32_t render_priority_second = 0;
		};

		struct FormConfig : Dumpling::Form::Config
		{
			bool is_primary = true;
		};

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule>;

		static auto Create(Config config) -> Ptr;


		bool CreateForm(Entity& entity, Scene& scene, FormConfig const& config = {});
		bool CreateRenderer(Scene& scene);

	protected:

		RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer);
		static void Renderer_CommitedFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_filter, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer> order);
		static void Renderer_FlushFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_form, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const> order);
		static void Renderer_DispatchPass(SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const> order);

		Config config;
		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};
	*/
};