#include "ExceptionHandler.h"

ExceptionHandler::ExceptionHandler(int l, std::string f, HRESULT h)
	: line(l), file(f), hr(h), type("Base Exception Handler")
{
	return;
}

ExceptionHandler::~ExceptionHandler(void)
{
	return;
}

std::string ExceptionHandler::getType(void) const noexcept
{
	return type;
}

int ExceptionHandler::getLine(void) const noexcept
{
	return line;
}

std::string ExceptionHandler::getFile(void) const noexcept
{
	return file;
}

int ExceptionHandler::getCode(void) const noexcept
{
	return static_cast<int>(hr);
}

// if we received an HRESULT our errorDescription is empty
// if we did not receive an HRESULT our error description is populated
// by constructor with custom string
std::string ExceptionHandler::getErrorDescription(void)
{
	if (hr != S_OK) {
		translateErrorCode();
	}
	std::ostringstream oss;

	oss << errorDescription << std::endl
		<< "Line: " << line << std::endl
		<< "File: " << file << std::endl;
	errorDescription = oss.str();
	return errorDescription;
}

void ExceptionHandler::translateErrorCode(void) noexcept
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf,
		0,
		nullptr
	);

	// move message to our private std::string errorDescription
	errorDescription = (const char*)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return;
}