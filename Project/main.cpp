#include "WindowHandler.h"
#include <iostream>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	try {
		WindowHandler wnd(0, 0, "Moo Game Engine");
		wnd.go();
	}
	catch (ExceptionHandler& e)
	{
		MessageBox(nullptr, e.getErrorDescription().c_str(), e.getType().c_str(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Library Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr,
			"Unhandled Exception -- No further information",
			"Unhandled Exception",
			MB_ICONEXCLAMATION | MB_OK
		);
	}
	return 0;
}