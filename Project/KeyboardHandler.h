#pragma once

#include <queue>
#include <bitset>

class Keyboard
{
	friend class WindowHandler;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press, Release, Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept
			: type(Event::Type::Invalid), code(0)
		{}
		Event(Event::Type t, unsigned char c) noexcept
			: type(t), code(c)
		{}
		Keyboard::Event::Type getType(void) const noexcept
		{
			return type;
		}
		bool isPress(void) const noexcept
		{
			return type == Type::Press;
		}
		bool isRelease(void) const noexcept
		{
			return type == Type::Release;
		}
		bool isValid(void) const noexcept
		{
			return type == Type::Invalid;
		}
		unsigned char getCode(void) const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;

	// Key event handling
	bool isKeyPressed(unsigned char keycode) const noexcept;
	Keyboard::Event readKey(void) noexcept;
	bool isKeyEmpty(void) const noexcept;
	void clearKey(void) noexcept;

	// Char event handling
	char readChar(void) noexcept;
	bool isCharEmpty(void) const noexcept;
	void clearChar(void) noexcept;
	void clear(void) noexcept;

	// Autorepeat handling
	void enableAutorepeat(void) noexcept;
	void disableAutorepeat(void) noexcept;
	bool isAutorepeatEnabled(void) noexcept;
private:
	void onKeyPress(unsigned char keycode) noexcept;
	void onKeyRelease(unsigned char keycode) noexcept;
	void onChar(unsigned char key) noexcept;
	void clearState(void) noexcept;

	template<typename T>
	static void trimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int maxBufferSize = 16u;
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Keyboard::Event> keybuffer;
	std::queue<char> charbuffer;
};