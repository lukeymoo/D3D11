#include "mousehandler.h"

/*
	Getter functions
*/
std::pair<int, int> Mouse::getPos(void) const noexcept
{
	return { x, y };
}

std::pair<int, int> Mouse::getPosDelta(void) const noexcept
{
	return { mouse_x_delta, mouse_y_delta };
}

int Mouse::getPosX(void) const noexcept
{
	return x;
}

int Mouse::getPosY(void) const noexcept
{
	return y;
}

bool Mouse::isLeftPressed(void) const noexcept
{
	return leftIsPressed;
}

bool Mouse::isMiddlePressed(void) const noexcept
{
	return middleIsPressed;
}

bool Mouse::isRightPressed(void) const noexcept
{
	return rightIsPressed;
}

bool Mouse::isInWindow(void) const noexcept
{
	return inWindow;
}

Mouse::Event Mouse::read(void) noexcept
{
	if (buffer.size() > 0) {
		Mouse::Event e;
		e = buffer.front();
		buffer.pop();
		return e;
	}
	else {
		Mouse::Event e;
		return e;
	}
}



/*
	On event handlers
*/

void Mouse::clearState(void) noexcept
{
	buffer = std::queue<Mouse::Event>();
	return;
}

void Mouse::onMouseEnter(void) noexcept
{
	inWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	trimBuffer();
	return;
}

void Mouse::onMouseLeave(void) noexcept
{
	inWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	trimBuffer();
	return;
}

void Mouse::onMouseMove(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	trimBuffer();
	return;
}

void Mouse::onLeftPress(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	leftIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::LDown, *this));
	trimBuffer();
	return;
}

void Mouse::onLeftRelease(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	leftIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	trimBuffer();
	return;
}

void Mouse::onRightPress(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	rightIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::RDown, *this));
	trimBuffer();
	return;
}

void Mouse::onRightRelease(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	rightIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	trimBuffer();
	return;
}

void Mouse::onMiddlePress(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	middleIsPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::MDown, *this));
	trimBuffer();
	return;
}

void Mouse::onMiddleRelease(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	middleIsPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	trimBuffer();
	return;
}

void Mouse::onWheelUp(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	deltatest += 1;

	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	trimBuffer();
	return;
}

void Mouse::onWheelDown(int nx, int ny) noexcept
{
	last_x = x;
	last_y = y;
	x = nx;
	y = ny;
	mouse_x_delta = x - last_x;
	mouse_y_delta = y - last_y;

	deltatest -= 1;

	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	trimBuffer();
	return;
}

void Mouse::onWheelDelta(int nx, int ny, int wDelta) noexcept
{
	wheelDeltaCarry += wDelta;
	// generate wheel events every WHEEL_DELTA (120) up or down
	while (wheelDeltaCarry >= WHEEL_DELTA) { // up
		wheelDeltaCarry -= WHEEL_DELTA;
		onWheelUp(nx, ny);
	}

	while (wheelDeltaCarry <= -WHEEL_DELTA) { // down
		wheelDeltaCarry += WHEEL_DELTA;
		onWheelDown(nx, ny);
	}
	return;
}

void Mouse::trimBuffer(void) noexcept
{
	while (buffer.size() > maxBufferSize) {
		buffer.pop();
	}
	return;
}