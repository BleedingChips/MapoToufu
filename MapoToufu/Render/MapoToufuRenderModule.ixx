module;

export module MapoToufuRenderModule;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuRenderPass;
import MapoToufuRenderer;
import MapoToufuModule;

export namespace MapoToufu
{

	enum class RenderPlatform
	{
		DX12,
		DoNotCare
	};

	struct RendererModule : public Module, public Potato::Task::Node
	{
		static constexpr std::wstring_view module_name = L"MapoTouFuRenderer";

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule, Module::Wrapper>;

		struct Config
		{
			RenderPlatform platform = RenderPlatform::DoNotCare;
			ModulePriority priority;
			std::pmr::memory_resource* resource = std::pmr::get_default_resource();
		};

		static Ptr Create(Config config = {});

		bool AddFormComponent(Instance& instance, Entity& target_entity);
		virtual void Init(GameContext& context) override;
		virtual void Load(Instance& instance) override;
		virtual void UnLoad(Context& context) override;

	protected:

		virtual void TaskExecute(Potato::Task::Context& context, Parameter& parameter) override;

		RendererModule(Config config);

		RenderPlatform platform;
		ModulePriority priority;
		Dumpling::Device::Ptr renderer;

		std::shared_mutex event_capture_mutex;
		std::pmr::vector<Dumpling::FormEventCapture::Ptr> captures;

		friend struct RendererModule;
	};
};