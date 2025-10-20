#include<Windows.h>
#include "WindowsMessageMap.h"

#include <string>
#include <Windows.h>
#include <format>
#include "Window.h"



int CALLBACK WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try {
		Window wmd(800, 300, L"Kinh lup Window");
		//create window instance


		//message pump
		// khi có sự kiện xảy ra, thì handler của window đó được truyền vào trong msg (msg có con trỏ tới handle)
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessageW(&msg, nullptr, 0, 0)) > 0)
		{
			// dịch từ thông điệp ảo sang thông điệp chuỗi kí tự để dispatch hiểu
			TranslateMessage(&msg);
			//dispacth gọi tới WndPro đăng kí trong class window
			DispatchMessageW(&msg);
			if (wmd.kbd.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, L"Something", L"Anuthing",MB_OK);
			}
		}
		if (gResult == -1)
		{
			return -1;
		}
		else {
			return msg.wParam;
		}
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