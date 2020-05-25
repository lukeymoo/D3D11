#pragma once

#include <sstream>
#include <fstream>

#include "ExceptionHandler.h"

/*
	Creates a flat grid in world
	Reads in height map file & then raises vertices based on height map

	Each vertex is on per pixel basis --
	so 1 pixel in height map is 1 square unit(foot) in world
	
*/

class WorldHandler
{
	class Exception : public ExceptionHandler
	{
		public:
			Exception(std::string msg, int line, const char* file);
			~Exception(void);
	};
	public:
		WorldHandler(void);
		~WorldHandler(void);

		bool loadMap(std::string file);
};

#define WORLD_EXCEPT(str) throw WorldHandler::Exception::Exception(str, __LINE__, __FILE__)