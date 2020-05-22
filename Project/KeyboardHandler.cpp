#include "keyboardhandler.h"

/*
	Key event handling
*/
bool Keyboard::isKeyPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::readKey(void) noexcept
{
	// ensure keybuffer not empty
	if (keybuffer.size() > 0) {
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else {
		return Keyboard::Event();
	}
}

bool Keyboard::isKeyEmpty(void) const noexcept
{
	return keybuffer.empty();
}

void Keyboard::clearKey(void) noexcept
{
	keybuffer = std::queue<Keyboard::Event>();
}

/*
	Char event handling
*/
char Keyboard::readChar(void) noexcept
{
	// ensure char buffer not empty
	if (charbuffer.size() > 0) {
		unsigned char c = charbuffer.front();
		charbuffer.pop();
		return c;
	}
	else {
		return 0;
	}
}

bool Keyboard::isCharEmpty(void) const noexcept
{
	return charbuffer.empty();
}

void Keyboard::clearChar(void) noexcept
{
	charbuffer = std::queue<char>();
	return;
}

// Clear both queue buffers
void Keyboard::clear(void) noexcept
{
	clearChar();
	clearKey();
	return;
}

/*
	Autorepeat handling
*/
void Keyboard::enableAutorepeat(void) noexcept
{
	autoRepeatEnabled = true;
	return;
}

void Keyboard::disableAutorepeat(void) noexcept
{
	autoRepeatEnabled = false;
	return;
}

bool Keyboard::isAutorepeatEnabled(void) noexcept
{
	return autoRepeatEnabled;
}


void Keyboard::onKeyPress(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	trimBuffer(keybuffer);
	return;
}

void Keyboard::onKeyRelease(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	trimBuffer(keybuffer);
	return;
}

void Keyboard::onChar(unsigned char key) noexcept
{
	charbuffer.push(key);
	trimBuffer(charbuffer);
	return;
}

void Keyboard::clearState(void) noexcept
{
	keystates.reset();
	return;
}

template<typename T>
static void Keyboard::trimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > maxBufferSize) {
		buffer.pop();
	}
	return;
}
