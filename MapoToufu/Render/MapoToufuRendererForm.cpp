module;
#include <cassert>
#include "windows.h"
module MapoToufuRendererForm;

namespace MapoToufu
{
	FormEvent::Respond DefaultFormEventHoot::Hook(FormEvent& event)
	{
		if (event.IsMessage(FormMessage::DESTORY))
		{
			need_quit = true;
		}
		/*
		if (event.IsMessage(FormMessage::CLOSE))
		{
			//need_quit = true;
			return event.RespondMarkAsHooked();
		}
		*/
		return event.RespondMarkAsSkip();
	}
	void DefaultFormEventHoot::UpdateEventHook(Context& context, Form& form)
	{
		if (need_quit)
		{
			context.GetInstance().RequireQuit();
		}
	}

	DefaultFormEventHoot::Ptr DefaultFormEventHoot::Create(std::pmr::memory_resource* resource)
	{
		auto re = Potato::IR::MemoryResourceRecord::Allocate<DefaultFormEventHoot>(resource);
		if (re)
		{
			return new (re.Get()) DefaultFormEventHoot{ re };
		}
		return {};
	}
}