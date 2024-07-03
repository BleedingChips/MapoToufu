import std;
import Potato;
import Dumpling;
import Noodles;

using namespace Potato;
using namespace Dumpling;
using namespace Noodles;


struct DefaultNoodleContext : public Context
{
	void AddContextRef() const override {}
	void SubContextRef() const override {}
};

int main()
{

	auto form = Form::Create();

	FormProperty pro;
	pro.title = u8"NoodlesDemoGame";

	form->Init(pro);

	while(true)
	{
		bool need_quit = false;
		while(
			Form::PeekMessageEventOnce([&](Form*, FormEvent event)->FormEventRespond
		{
			if(event.message == FormEventEnum::DESTORYED)
			{
				need_quit = true;
			}
				return FormEventRespond::Default;
		})
			)
		{
			
		}
		if(need_quit)
			break;
	}
	

	return 0;



	return 0;
}