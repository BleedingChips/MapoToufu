module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;

export namespace MapoToufu
{
	struct Renderer
	{
		bool Init(std::pmr::memory_resource* resource);
	protected:
		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};

};