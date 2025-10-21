#pragma once
#include"Window.h"
#include "KinhLupTimer.h"

class App
{
public:
	App();
	int Go();
private:
	void DoFrame();
private:
	Window wnd;
	KinhLupTimer timer;
};