#include<Windows.h>
#include "WindowsMessageMap.h"

#include <string>
#include <Windows.h>
#include <format>
#include "Window.h"
#include "Mouse.h"
#include <sstream>
#include "App.h"


int CALLBACK WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		return App{}.Go();
	}
	catch (const KinhLupException& e)
	{
		MessageBoxA(nullptr,  e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	
}