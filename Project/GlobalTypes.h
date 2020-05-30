#pragma once

// Contains enums/structs/etc that are needed across multiple files

enum class DIRECTION {
	FORWARD,	// from origin -> +z axis
	BACKWARD,	// from origin -> -z axis
	LEFT,		// from origin -> -x axis
	RIGHT,		// from origin -> +x axis
	UP,			// from origin -> +y axis
	DOWN		// from origin -> -y axis
};

enum class CAMERA_MODE {
	FREELOOK,
	FIRSTPERSON,
	THIRDPERSON
};