module;

export module MapoToufuImGuiModule;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuRenderer;
import MapoToufuModule;
import MapoToufuRenderModule;

export namespace MapoToufu
{

	struct ImGuiModule : public Module
	{
		static constexpr std::wstring_view module_name = L"MapoTouFuImGui";

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule, Module::Wrapper>;

		static Ptr Create(RendererModule& renderer_module, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

	protected:


	};
};