#include"Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keybuffer.size() > 0u)
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}
bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
	charbuffer = std::queue<char>();
}

void Keyboard::FlushKey() noexcept
{
	keybuffer = std::queue<Event>();
}
void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	autorepeatEnhable = true;
}
void Keyboard::DisableAutoRepeat() noexcept
{
	autorepeatEnhable = false;
}
bool Keyboard::AutorepeatIsEnabled() const noexcept
{
	return autorepeatEnhable;
}
void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}
void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}
void Keyboard::OnChar(char character) noexcept
{
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void Keyboard::ClearState() noexcept
{
	keystates.reset();
}
template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}