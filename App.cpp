#include"App.h"

App::App() :
	wnd(800, 600, L"KinhLup Window") {}

int App::Go()
{
	while (true)
	{
		if (const auto encode = Window::ProcessMessages())
		{
			return *encode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	wnd.Gfx().EndFrame();

}