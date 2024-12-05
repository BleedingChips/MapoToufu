module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;

export namespace MapoToufu
{

	struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr renderer;
	};

	struct RendererModule :  protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule>;

		struct Config
		{
		};

		static Ptr Create(Config config, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		FrameRenderer CreateFrameRenderer() const;

	protected:

		friend struct Potato::Pointer::DefaultIntrusiveWrapper;
		Dumpling::DevicePtr device;
	};
};