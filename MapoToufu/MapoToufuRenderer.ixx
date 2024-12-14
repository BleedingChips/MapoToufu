module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import MapoToufuScene;

export namespace MapoToufu
{

	struct Form
	{
		struct Event : Dumpling::FormEvent
		{
			bool captured = false;
		};

	protected:

		Dumpling::Form  form;
		Dumpling::FormWrapper::Ptr form_wrapper;

		std::shared_mutex reading_mutex;
		std::pmr::vector<Event> captured_event;

		std::shared_mutex cached_event_mutex;
		std::pmr::vector<Event> cache_captured_event_event;
	};

	struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr renderer;
	};


	struct Renderer
	{
		bool Init(std::pmr::memory_resource* resource);

		struct SystemProperty
		{
			std::int32_t priority_layout = 0;
			std::int32_t priority_first = 0;
			std::u8string_view group_name;
		};

		void OnSceneCreated(Scene& scene, SystemProperty property);

	protected:

		static void UpdateFormFrame(SceneWrapper& wrapper, AtomicComponentFilter<Form> c_filter, AtomicSingletonFilter<FrameRenderer> c_renderer);


		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};

};