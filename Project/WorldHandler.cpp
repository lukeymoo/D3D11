#include "WorldHandler.h"

WorldHandler::WorldHandler(void)
{
	return;
}

WorldHandler::~WorldHandler(void)
{
	return;
}

bool WorldHandler::loadMap(std::string filename)
{
	// read in file
	std::ifstream file;
	file.open(filename, std::ios::binary | std::ios::in);

	if (!file.is_open()) {
		WORLD_EXCEPT("Failed to open file -> " + filename);
	}
	return true;
}










WorldHandler::Exception::Exception(std::string m, int l, const char* f)
	: ExceptionHandler(l, f, 0)
{
	type = "World Handler Exception";
	errorDescription = m;
	return;
}

WorldHandler::Exception::~Exception(void)
{
	return;
}