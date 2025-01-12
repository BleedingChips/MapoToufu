module;

export module MapoToufuRenderModule;

import std;
import Potato;
import Dumpling;
import Noodles;
import MapoToufuScene;
import MapoToufuGameContext;
import MapoToufuRenderer;

export namespace MapoToufu
{

	export struct RendererModule;

	using Dumpling::FormEvent;

	struct FormEventStorage : public Dumpling::FormEventCapture, public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{

		using Ptr = Potato::Pointer::IntrusivePtr<FormEventStorage, Dumpling::FormEventCapture::Wrapper>;

		static Ptr Create(std::pmr::memory_resource* resource = std::pmr::get_default_resource());

		template<typename Type>
		void ForeachEvent(Type&& type) requires(std::is_invocable_r_v<FormEvent::Respond, Type, FormEvent&>)
		{
			std::lock_guard lg(respond_mutex);
			for (auto& ite : respond_events)
			{
				if (!ite.has_captured)
				{
					FormEvent::Respond re = type(ite.event);
					if (re == FormEvent::Respond::CAPTURED)
					{
						ite.has_captured = true;
					}
				}
			}
		}

		void SwapReceiveEvent();

	protected:

		FormEventStorage(Potato::IR::MemoryResourceRecord record) : MemoryResourceRecordIntrusiveInterface(record) {}

		void AddFormEventCaptureRef() const override { MemoryResourceRecordIntrusiveInterface::AddRef(); }
		void SubFormEventCaptureRef() const override { MemoryResourceRecordIntrusiveInterface::SubRef(); }
		FormEvent::Respond RespondEvent(FormEvent event) override;

		struct Event
		{
			FormEvent event;
			bool has_captured = false;
		};

		std::mutex respond_mutex;
		std::pmr::vector<Event> respond_events;

		std::mutex receive_mutex;
		std::pmr::vector<Event> receive_events;
	};



	struct Form
	{

		template<typename Type>
		void ForeachEvent(Type&& type) requires(std::is_invocable_r_v<FormEvent::Respond, Type, FormEvent&>)
		{
			event_storage->ForeachEvent(std::forward<Type>(type));
		}

		virtual Dumpling::RendererResource::Ptr GetResource() { return form_wrapper->GetAvailableRenderResource(); }

	protected:

		Dumpling::Form  form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		FormEventStorage::Ptr event_storage;
		bool is_primary = true;

		friend struct RendererModule;
	};


	export struct RendererModule : public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{

		struct Config
		{
			std::int32_t render_layer = 0;
			std::int32_t render_priority_first = 0;
			std::int32_t render_priority_second = 0;
		};

		struct FormConfig : Dumpling::Form::Config
		{
			bool is_primary = true;
		};

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule>;

		static auto Create(Config config) -> Ptr;

		bool CreateForm(Entity& entity, Scene& scene, FormConfig const& config = {});
		bool CreateRenderer(Scene& scene);

	protected:

		RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer);
		static void Renderer_CommitedFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_filter, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer> order);
		static void Renderer_FlushFrame(SceneWrapper& wrapper, AutoComponentQuery<Form> c_form, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const> order);
		static void Renderer_DispatchPass(SceneWrapper& wrapper, AutoSingletonQuery<FrameRenderer> c_renderer, AutoThreadOrderQuery<FrameRenderer const> order);

		Config config;
		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};

};