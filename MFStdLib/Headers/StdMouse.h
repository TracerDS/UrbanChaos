// StdMouse.h
// Guy Simmons, 18th December 1997.

#pragma once

#include <cstdint>

//---------------------------------------------------------------

struct LastMouse
{
	std::int32_t ButtonState;
	std::int32_t MouseX;
	std::int32_t MouseY;
	MFPoint MousePoint;
};


extern volatile std::uint8_t MouseMoved;
extern volatile std::uint8_t LeftButton;
extern volatile std::uint8_t MiddleButton;
extern volatile std::uint8_t RightButton;
extern volatile std::int32_t MouseX, MouseY;
extern volatile LastMouse LeftMouse;
extern volatile LastMouse MiddleMouse;
extern volatile LastMouse RightMouse;
extern volatile MFPoint MousePoint;