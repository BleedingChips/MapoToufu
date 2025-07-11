module;

export module MapoToufuRenderer;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuInstance;
import MapoToufuDefine;
import MapoToufuRenderPass;

export namespace MapoToufu
{

	struct Form;

	struct FormConfig
	{

	};

	struct FormEventCapture : public Dumpling::FormEventCapture
	{
		void Swap() {};

		using Ptr = Potato::Pointer::IntrusivePtr<FormEventCapture, Wrapper>;

		static Ptr Create(FormConfig config = {}, std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		virtual void Update(Context& context, Entity& owner, Form& form) = 0;

	public:

	};

	using PassRenderer = Dumpling::PassRenderer;

	struct FrameRenderer
	{
		FrameRenderer(FrameRenderer const&) = default;
		FrameRenderer(FrameRenderer&&) = default;
		FrameRenderer(Dumpling::FrameRenderer::Ptr renderer) : frame_renderer(std::move(renderer)) {}
		
		bool CommitFrame() { return frame_renderer->CommitFrame().has_value(); }
		bool FlushFrame() { return frame_renderer->FlushToLastFrame(); }
		bool BeginPass(PassRenderer& pass_renderer, PassRequest const& request) const;
		bool EndPass(PassRenderer& pass_renderer) const;
	protected:
		Dumpling::FrameRenderer::Ptr frame_renderer;
	};

	struct Form
	{
		Dumpling::Form platform_form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		FormEventCapture::Ptr event;
	};
};