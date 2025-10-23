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
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(c, c, 1.0f);
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}