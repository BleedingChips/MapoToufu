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
		using Config = Dumpling::Form::Config;
		static void PostQuit() { Dumpling::Form::PostQuitEvent(); }
	protected:
		Dumpling::Form form;
		Dumpling::FormWrapper::Ptr wrapper;
		operator bool() const { return form; }
	};

	struct Renderer : public ModuleInterface, protected Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		
		using Ptr = Potato::Pointer::IntrusivePtr<Renderer, ModuleInterface::Wrapper>;

		static auto Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource()) -> Ptr;
		Form CreateForm(Form::Config config);

	protected:

		Renderer(Potato::IR::MemoryResourceRecord record,  Dumpling::Device::Ptr device) : MemoryResourceRecordIntrusiveInterface(record), device(std::move(device)) {}

		Dumpling::Device::Ptr device;

		void AddModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubModuleInterfaceRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		bool MainThreadTick() override;

		friend struct ModuleInterface::Wrapper;
	};

};