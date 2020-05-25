#pragma once

#include "winclean.h"
#include <sstream>

#include "ExceptionHandler.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "GraphicsHandler.h"
#include "TimerHandler.h"

class WindowHandler
{
	// Exceptions handler
	public:
		class Exception : public ExceptionHandler
		{
			public:
				Exception(int line, std::string file, HRESULT hr);
				Exception(std::string message, int line, const char* file);
				~Exception(void) = default;
				std::string getType(void) const noexcept override;
		};
	public:
		WindowHandler(void) = delete;
		WindowHandler(int width, int height, const char* title);
		~WindowHandler(void);
		
		HWND getWindowHandle(void);


		int getWidth(void) const noexcept;
		int getHeight(void) const noexcept;
		std::string getTitle(void) const noexcept;

		void go(void);
	/*
		INTERFACES
	*/
	public:
		Keyboard kbd;
		Mouse mouse;
		std::unique_ptr<Graphics> gfx; // initialized after CreateWindow()
	private:
		Timer timer;
		Timer fpsTimer;
		HWND hWnd;
		int width;
		int height;
		std::string title;
		HRESULT hr;
		bool running; // main loop control
		static LRESULT CALLBACK configureProcessing(HWND, UINT, WPARAM, LPARAM);
		static LRESULT CALLBACK processMessageThunk(HWND, UINT, WPARAM, LPARAM);
		LRESULT CALLBACK processMessage(HWND, UINT, WPARAM, LPARAM);
};

#define WH_EXCEPT(hr) throw WindowHandler::Exception::Exception(__LINE__, __FILE__, hr)
#define W_EXCEPT(str) throw WindowHandler::Exception::Exception(str, __LINE__, __FILE__)