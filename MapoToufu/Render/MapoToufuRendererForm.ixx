module;

export module MapoToufuRendererForm;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuDefine;

export namespace MapoToufu
{
	struct Form;

	struct FormEventHook : public Dumpling::FormEventHook
	{
		using Ptr = Potato::Pointer::IntrusivePtr<FormEventHook, Dumpling::FormEventHook::Wrapper>;
		virtual void UpdateEventHook(Context& context, Form& form) {}
	};

	struct DefaultFormEventHoot : public FormEventHook, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{
		using Ptr = FormEventHook::Ptr;

		static Ptr Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource());
		virtual FormEvent::Respond Hook(FormEvent& event) override;
		virtual void UpdateEventHook(Context& context, Form& form) override;
		DefaultFormEventHoot(Potato::IR::MemoryResourceRecord record) : MemoryResourceRecordIntrusiveInterface(record) {}
		virtual void AddFormEventHookRef() const { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		virtual void SubFormEventHookRef() const { MemoryResourceRecordIntrusiveInterface::SubRef(); }
	protected:
		std::atomic_bool need_quit = false;
	};

	struct FormConfig
	{
		Dumpling::Form::Rectangle rectangle;
		Dumpling::FormStyle::Ptr style = Dumpling::FormStyle::GetFixedStyle();
		FormEventHook::Ptr event_hook;
		wchar_t const* title = L"MapoToufu Form";
	};

	struct Form
	{
		RendererResource::Ptr GetRenderTarget() const { return form_wrapper->GetAvailableRenderResource(); }
		operator bool() const { return platform_form; }
		Dumpling::Form platform_form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		FormEventHook::Ptr event_hook;
		bool need_skip_first_present = true;
	};
};