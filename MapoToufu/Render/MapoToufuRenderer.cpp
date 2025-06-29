module;
#include <cassert>
#include "windows.h"
module MapoToufuRenderer;

namespace MapoToufu
{
	struct FormEventCaptureDefault : public FormEventCapture, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		FormEventCaptureDefault(Potato::IR::MemoryResourceRecord record, FormConfig config)
			: MemoryResourceRecordIntrusiveInterface(record) {
		}
		virtual HRESULT RespondEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override
		{
			if (msg == WM_CLOSE)
			{
				need_quit = true;
				Dumpling::Form::PostQuitEvent();
			}
			return DefWindowProcW(hWnd, msg, wParam, lParam);
		}

		virtual void Update(Context& context, Entity& owner, Form& form)
		{
			if (need_quit)
			{
				context.GetInstance().RequireQuit();
			}
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

	bool FrameRenderer::BeginPass(Dumpling::PassRenderer& pass_renderer) const
	{
		return frame_renderer->PopPassRenderer(pass_renderer);
	}
	bool FrameRenderer::EndPass(Dumpling::PassRenderer& pass_renderer) const
	{
		return frame_renderer->FinishPassRenderer(pass_renderer);
	}
}