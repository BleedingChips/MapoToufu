module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuGameContext;

export namespace MapoToufu
{

	using FormEvent = Dumpling::FormEvent;

	struct Form;

	struct FormEventResponder
	{
		struct Wrapper
		{
			void AddRef(FormEventResponder const* ptr) { ptr->AddFormEventResponderRef(); }
			void SubRef(FormEventResponder const* ptr) { ptr->SubFormEventResponderRef(); }
		};

		using Ptr = Potato::Pointer::IntrusivePtr<FormEventResponder, Wrapper>;
		virtual FormEvent::Respond Respond(Context& context, Form& form, Entity const& entity, FormEvent event) { return FormEvent::Respond::PASS; }

	protected:
		virtual void AddFormEventResponderRef() const = 0;
		virtual void SubFormEventResponderRef() const = 0;
	};

	struct FormEventCapture : public Dumpling::FormEventCapture
	{
		void Swap() {};

		using Ptr = Potato::Pointer::IntrusivePtr<FormEventCapture, Wrapper>;

		static Ptr Create(bool top_windows = true, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		void ComsumeFormEvent(Context& context);

	public:

		std::mutex respond_mutex;
		std::pmr::vector<FormEventResponder::Ptr> responder;

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
		FormEventCapture::Ptr event;
	};
};