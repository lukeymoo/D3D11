#include "WindowHandler.h"

static int fpsCounter = 0;

WindowHandler::Exception::Exception(int li, std::string fi, HRESULT h)
	: ExceptionHandler(li, fi, h)
{
	type = "Window Handler Exception";
	return;
}

WindowHandler::Exception::Exception(std::string m, int li, const char* fi)
	: ExceptionHandler(li, fi, 0)
{
	type = "Window Handler Exception";
	errorDescription = m;
	return;
}

std::string WindowHandler::Exception::getType(void) const noexcept
{
	return type;
}

WindowHandler::WindowHandler(int w, int h, const char* t)
	:
	hWnd(nullptr), width(w), height(h), hr(0), running(true)
{
	fpsTimer.startTimer();
	timer.startTimer();

	WNDCLASSEXA windowClass;
	ZeroMemory(&windowClass, sizeof(WNDCLASSEXW));

	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
	windowClass.lpfnWndProc = configureProcessing;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = "moo_game_engine_class";
	windowClass.hIconSm = nullptr;

	RegisterClassEx(&windowClass);

	// Adjust window rect will ensure our client region is
	// the specified width/height and not just the window as whole

	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = width;
	r.bottom = height;
	AdjustWindowRect(
		&r,
		WS_MINIMIZEBOX | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
		false
	);

	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		"moo_game_engine_class",
		t,
		WS_MINIMIZEBOX | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
		10,
		10,
		r.right - r.left,
		r.bottom - r.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		this
	);

	// ensure window created succesfully
	if (hWnd == nullptr) {
		throw Exception(
			"CreateWindow() failed! HWND is nullptr",
			__LINE__,
			__FILE__
		);
	}

	// Initialize Graphics Handler now that we've got a
	// window handle
	gfx = std::make_unique<Graphics>(hWnd, width, height);

	// Show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	return;
}

WindowHandler::~WindowHandler(void)
{
	UnregisterClass("moo_game_engine_class", GetModuleHandle(nullptr));
	return;
}

std::string WindowHandler::getTitle(void) const noexcept
{
	return title;
}

HWND WindowHandler::getWindowHandle(void)
{
	return hWnd;
}

int WindowHandler::getWidth(void) const noexcept
{
	return width;
}

int WindowHandler::getHeight(void) const noexcept
{
	return height;
}


void WindowHandler::go(void)
{
	MSG msg;

	while (running) {
		double fpsdt = timer.getElaspedMS();
		timer.restart();

		// process messages and remove them from queue
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// catch quit message as peek message will do nothing with WM_QUIT
		if (msg.message == WM_QUIT) {
			running = false;
		}

		// Title bar displays player's X/Y/Z
		fpsCounter += 1;
		if (fpsTimer.getElaspedMS() > 1000) {
			std::ostringstream oss;
			DirectX::XMFLOAT3 rotation = gfx->camera.getRotationFloat3();
			oss << "Player Vector: <"
				<< gfx->camera.getX() << ", " << gfx->camera.getY()
				<< ", " << gfx->camera.getZ() << ">";
			oss << "Rotation Vector: <" << rotation.x
				<< ", " << rotation.y << ", " << rotation.z << "> ";
			oss << "FPS: " << fpsCounter;

			fpsCounter = 0;
			fpsTimer.restart();
			SetWindowText(hWnd, oss.str().c_str());
		}

		/*
			Input handling
		*/
		Mouse::Event mouseEvent = mouse.read();
		if (mouse.isLeftPressed()) {
			if (mouseEvent.getType() == Mouse::Event::Type::Move) {
				std::pair<int, int> md = mouse.getPosDelta();
				XMVECTOR r = XMVectorSet((md.second * 0.8f), (md.first * 0.8f), 0.0f, 0.0f);
				gfx->camera.rotate(r * 0.01f);
			}
		}
		if (kbd.isKeyPressed('W')) {
			gfx->camera.moveForward(fpsdt);
		}
		if (kbd.isKeyPressed('A')) {
			gfx->camera.moveLeft(fpsdt);
		}
		if (kbd.isKeyPressed('S')) {
			gfx->camera.moveBackward(fpsdt);
		}
		if (kbd.isKeyPressed('D')) {
			gfx->camera.moveRight(fpsdt);
		}

		// move camera UP/DOWN Y axis
		if (kbd.isKeyPressed(VK_SPACE)) {
			gfx->camera.moveUp(fpsdt);
		}
		if (kbd.isKeyPressed(VK_CONTROL)) {
			gfx->camera.moveDown(fpsdt);
		}

		gfx->camera.update();

		/*
			Rendering
		*/
		gfx->prepareScene();
		gfx->pSwap->Present(1, 0);
	}
	return;
}





LRESULT CALLBACK WindowHandler::configureProcessing(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// reassign window proc callback to our now fully constructed 
	// windowhandler obj's
	if (uMsg == WM_NCCREATE) {
		// extract ptr to WindowHandler from CREATESTRUCT*
		// We passed our WindowHandler* via CreateWindow() last arg `this`
		const CREATESTRUCTW* const ptrCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowHandler* const pWindowHandler = static_cast<WindowHandler*>(ptrCreateStruct->lpCreateParams);

		// store that ptr in the window's api `user data` section linking our windows API
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindowHandler));

		// change the message procedure now that we've finished modifications on windows api side
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&pWindowHandler->processMessageThunk));
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowHandler::processMessageThunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowHandler* wp = reinterpret_cast<WindowHandler*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	return wp->processMessage(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowHandler::processMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

		// Pass mouse events to our mouse handler
		case WM_MOUSEMOVE:
		{
			POINTS p = MAKEPOINTS(lParam);
			// if pointer in client region
			if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {

				mouse.onMouseMove(p.x, p.y);

				// ensure we've updated in/out window state
				if (!mouse.isInWindow()) {
					SetCapture(hWnd);
					mouse.onMouseEnter(); // update state
				}
			}
			else { // pointer not in client region
				// check if button is held down
				if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) {
					mouse.onMouseMove(p.x, p.y);
				}
				else { // no mouse drag event so release
					ReleaseCapture();
					mouse.onMouseLeave(); // log leave
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onLeftPress(p.x, p.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onLeftRelease(p.x, p.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onRightPress(p.x, p.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onRightRelease(p.x, p.y);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onMiddlePress(p.x, p.y);
			break;
		}
		case WM_MBUTTONUP:
		{
			const POINTS p = MAKEPOINTS(lParam);
			mouse.onMiddleRelease(p.x, p.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS p = MAKEPOINTS(lParam);
			const int wD = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.onWheelDelta(p.x, p.y, wD);
			break;
		}

		// Pass keyboard events to our keyboard handler
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			// filter os repeat keys unless we want them
			// windows uses bit 30 in lParam to notify if key is a repeat or not
			if (!(lParam & 0x40000000) || kbd.isAutorepeatEnabled()) {
				kbd.onKeyPress(static_cast<unsigned char>(wParam));
			}
			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			kbd.onKeyRelease(static_cast<unsigned char>(wParam));
			break;
		}
		case WM_CHAR:
		{
			kbd.onChar(static_cast<unsigned char>(wParam));
			break;
		}


		// General events
		case WM_KILLFOCUS:
		{
			kbd.clearState();
			mouse.clearState();
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}