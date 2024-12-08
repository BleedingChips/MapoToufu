module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;

export namespace MapoToufu
{

	struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr renderer;
	};

	struct RendererModule : public ModuleInterface, protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule, ModuleInterface::Wrapper>;

		struct Config
		{
		};

		static Ptr Create(Config config, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		StructLayout::Ptr GetStructLayout() const override { return StructLayout::GetStatic<RendererModule>(); }

		FrameRenderer CreateFrameRenderer() const;

	protected:

		RendererModule(Potato::IR::MemoryResourceRecord record, Config config);

		void AddModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }

		friend struct Potato::Pointer::DefaultIntrusiveWrapper;
		Dumpling::DevicePtr device;
	};
};