#include"Window.h"
#include"sstream"
#include"resource.h"
// tạo 1 singleton

int Window::s_windowCount= 0;

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(wc);   
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0));
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 16, 16, 0));
	RegisterClassExW(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClassW(wndClassName, GetInstance());
}
const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width , int height , const wchar_t* name) :
	width(width), height(height)
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	// thằng này tính toán của kích thức của sổ thức, cái ta cần truyền vào của sổ của client
	if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, FALSE))
		//create window and get hwnd
		throw CHWND_LAST_EXCEPT();
	
	hWnd = CreateWindowExW(0,WindowClass::GetName(), name,
		WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,CW_USEDEFAULT,wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr,
		WindowClass::GetInstance(), this
	);
	//check error
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	// - gọi createWindow sẽ tạo 1 cấu trúc là createStruct chứa con trỏ this trỏ tới đối tượng Window này(trong cấu trúc này có con trỏ lCreatePrama chính là this )
	//khi gọi createWindow, windows sẽ gửi msg = WM_NCCREATE tới winproc khi mà window tạo sắp xong.
	// khi đó sẽ gửi WndProc thêm cả lParam chính là con trỏ tới CREATESTRUCT của đối tượng đó, cho nên sẽ lấy được con trỏ lCreatePrama hay chính là this ra
	ShowWindow(hWnd, SW_SHOW);
	// graphics object
	pGfx = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}
void Window::SetTitle(const std::wstring& title) const
{
	if (SetWindowTextW(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}
std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// while queues has mess, remove and dispatch them(but do not block when queue is empty)
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int
			return msg.wParam;
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	// empty optional when not quit app
	return {};
}
Graphics& Window::Gfx()
{
	return *pGfx;
}
LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		++s_windowCount;
	// ép kiểu con trở từ LPARAM về CREATESTRUCTW
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*> (lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// WinAPI sẽ lưu userdata 
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// khi setup lần đầu kết thức thì set window proc(đã đăng kí ở class) sang địa chỉ mới là handleMsgThunk
		SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}
// appdter, thằng trung gian, thằng điều hướng
 LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	 // lấy handle của cửa sổ có msg hiện tại
	 Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	 
	 return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
 }

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		if (--s_windowCount <= 0)        // chỉ khi là cửa sổ cuối
			PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
      // phá hủy của sổ hiện tại, ko cần destructor;
		DestroyWindow(hWnd);
		return 0;
		//clear state when wwindow loss focus
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	//***************** KEYBOARD MESSAGES*************
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	//*******************END**************************

	//****************MOUSE MESSAGES*******************
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, log enter, capture mouse
		if (pt.x > 0 && pt.x < width && pt.y>0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client regoin -> logmove . maintain captur if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up-> release capture/ log event for leaving
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	
	//****************END MOUSE***********************
	}
  //các sự kiện khác trả về mặc định
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

//Window Exception Stuff
Window::Exception::Exception(int line ,const char* file, HRESULT hr) noexcept
	:KinhLupException(line, file), hr(hr){ }
const char* Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code : " << GetErrorCode() << std::endl
		<< "Description : " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}
const char* Window::Exception::GetType() const noexcept
{
	return "Kinh Lup Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	if (nMsgLen == 0)
	{
		return " Meo biet loi cho nao luôn \n Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);

	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}
