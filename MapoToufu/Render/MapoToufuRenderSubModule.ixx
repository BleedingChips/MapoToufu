module;

export module MapoToufuRenderSubModule;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;
import MapoToufuRenderPass;
import MapoToufuRenderer;
import MapoToufuSubModule;

export namespace MapoToufu
{

	enum class RenderPlatform
	{
		DX12,
		DoNotCare
	};

	struct RendererSubModule : public SubModule, public Potato::Task::Node
	{
		static constexpr std::string_view module_name = "MapoTouFuRenderer";

		using Ptr = IntrusivePtr<RendererSubModule, SubModule::Wrapper>;

		struct Config
		{
			RenderPlatform platform = RenderPlatform::DoNotCare;
			bool enable_imgui_context = true;
			ModulePriority priority;
			std::pmr::memory_resource* resource = std::pmr::get_default_resource();
		};

		static Ptr Create(Config config = {});

		//bool AddFormComponent(Instance& instance, Entity& target_entity, FormConfig config = {});
		//bool AddIGHUDComponent(Instance& instance, Entity& target_entity, Dumpling::IGWidget::Ptr weight = {});
		
		virtual void Init(GameContext& context) override;
		virtual void Destory(GameContext& context) override;
		virtual bool ShouldLoad(Instance const& target_instance, InstanceConfig const& config) const override;
		virtual void Load(GameContext& context,Instance& instance, InstanceConfig const& config, SubModuleCollection const& Collection) override;
		virtual void UnLoad(Context& context, SubModuleCollection const& Collection) override;
		virtual StructLayout const& GetStructLayout() const override;

		ThreadMask GetCreateWindowThreadMask() const { return ThreadMask::MainThread; }

	protected:

		virtual void TaskExecute(Potato::Task::Context& context, Parameter& parameter) override;

		RendererSubModule(Config config);

		Config init_config;
		Dumpling::Device::Ptr renderer;

		std::shared_mutex event_capture_mutex;
		std::pmr::vector<Dumpling::FormEventHook::Ptr> captures;

		Potato::Misc::AtomicRefCount instance_reference_count;
	};
};