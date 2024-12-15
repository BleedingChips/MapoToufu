module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;
import MapoToufuGameContext;

export namespace MapoToufu
{
	struct Form
	{
		struct Event : Dumpling::FormEvent
		{
			bool captured = false;
		};

		Dumpling::Form  form;
		Dumpling::FormWrapper::Ptr form_wrapper;
	};

	struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr frame_renderer;
	};


	struct RendererModule : public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{

		struct Config
		{
			std::int32_t priority_layout = 0;
			std::int32_t priority_first = 0;
			std::u8string_view group_name = u8"renderer_module";
		};

		using FormConfig = Dumpling::Form::Config;

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule>;

		static auto Create(Config config) -> Ptr;

		Form CreateForm();
		bool CreateRenderer(Scene& scene);

	protected:

		RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer);

		Config config;
		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};

};