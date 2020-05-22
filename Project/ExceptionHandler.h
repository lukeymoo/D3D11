#pragma once

#include "winclean.h"
#include <sstream>
#include <string>

class ExceptionHandler
{		
	public:
		ExceptionHandler(int l, std::string f, HRESULT h); // HRESULT exception
		ExceptionHandler(void) = delete;
		~ExceptionHandler(void);

		int getLine(void) const noexcept;
		std::string getFile(void) const noexcept;
		int getCode(void) const noexcept;
		std::string getErrorDescription(void);

		virtual std::string getType(void) const noexcept;

	private:
		void translateErrorCode(void) noexcept; // turn HRESULT into windows description
	private:
		int line;
		HRESULT hr;
		std::string file;
	protected:
		std::string type; // changes based on class origin
		std::string errorDescription; // windows translated description
};