#pragma once
#include"ChiliWin.h"
#include "KinhLupException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>

class Window {
public:
	class Exception:public KinhLupException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};

private:
	//singleton manages registration/cleanup of window class
	class WindowClass {
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		// delete cấm dùng câu lệnh này;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator = (const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"Kinh lup Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const wchar_t* name) ;
	~Window();
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;
	void SetTitle(const std::wstring& title) const;
	static std::optional<int> ProcessMessages();
	Graphics& Gfx();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width ;
	int height ;
	HWND hWnd ;
	std::unique_ptr<Graphics> pGfx;
private:
	static int s_windowCount;
};

#define CHWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr); 
#define CHWND_LAST_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,GetLastError()); 