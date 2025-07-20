module;
#include <cassert>
#include "windows.h"
module MapoToufuRenderer;

namespace MapoToufu
{

	struct TopFormEventHook : public Dumpling::FormEventHook, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		virtual FormEvent::Respond Hook(FormEvent& event) override
		{
			if (event.IsFormDestory())
			{
				Dumpling::Form::PostQuitEvent();
			}
			return event.RespondMarkAsSkip();
		}

	protected:

		std::atomic_bool need_quit = false;

		virtual void AddFormEventCaptureRef() const { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		virtual void SubFormEventCaptureRef() const { MemoryResourceRecordIntrusiveInterface::SubRef(); }
	};
	

	FormEventCapture::Ptr FormEventCapture::Create(FormConfig config, std::pmr::memory_resource* resource)
	{
		auto re = Potato::IR::MemoryResourceRecord::Allocate<FormEventCaptureDefault>(resource);
		if (re)
		{
			return new (re.Get()) FormEventCaptureDefault{ re, std::move(config) };
		}
		return {};
	}
	*/

	bool FrameRenderer::BeginPass(PassRenderer& pass_renderer, PassRequest const& request) const
	{
		return frame_renderer->PopPassRenderer(pass_renderer, request);
	}

	bool FrameRenderer::EndPass(PassRenderer& pass_renderer) const
	{
		return frame_renderer->FinishPassRenderer(pass_renderer);
	}
}