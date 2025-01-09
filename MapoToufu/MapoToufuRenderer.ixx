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
		struct Event : Dumpling::FormEvent
		{
			bool captured = false;
		};

		Dumpling::Form  form;
		Dumpling::FormWrapper::Ptr form_wrapper;
		FormEventStorage::Ptr event_storage;
	};

	struct FrameRenderer
	{
		Dumpling::FrameRenderer::Ptr frame_renderer;
		std::pmr::vector<SystemNode::Ptr> reference_node;
	};


	struct RendererModule : public Potato::IR::MemoryResourceRecordIntrusiveInterface
	{

		struct Config
		{
			std::int32_t priority_layout = 0;
			std::int32_t priority_first = 0;
		};

		using FormConfig = Dumpling::Form::Config;

		using Ptr = Potato::Pointer::IntrusivePtr<RendererModule>;

		static auto Create(Config config) -> Ptr;

		bool CreateForm(Entity& entity, Scene& scene);
		bool CreateRenderer(Scene& scene);

	protected:

		RendererModule(Potato::IR::MemoryResourceRecord record, Config config, Dumpling::Device::Ptr renderer);

		Config config;
		Dumpling::Device::Ptr renderer;
		operator bool() const { return renderer; }
	};

};