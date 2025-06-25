module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuScene;
import MapoToufuGameContext;

export namespace MapoToufu
{

	using FormEvent = Dumpling::FormEvent;

	struct FormEventCapture : public Dumpling::FormEventCapture
	{
		void Swap() {};
	public:
		std::pmr::vector<FormEvent> fronet_end_event;

		std::mutex form_event;
		std::pmr::vector<FormEvent> back_end_event;
	};

	export struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr frame_renderer;
		std::pmr::vector<SystemNode::Ptr> reference_node;
	};

	struct Form
	{
		Dumpling::Form platform_form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		Dumpling::FormEventCapture::Ptr event;
	};
};