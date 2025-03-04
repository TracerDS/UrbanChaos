// Mouse.h
// Guy Simmons, 19th February 1997.

#pragma once


struct LastMouse
{
	std::int32_t		ButtonState,
				MouseX,
				MouseY;
	MFPoint		MousePoint;
};



extern volatile std::uint8_t		MouseMoved,
							LeftButton,
							MiddleButton,
							RightButton;
extern volatile std::int32_t		MouseX,
							MouseY;
extern volatile LastMouse	LeftMouse,
							MiddleMouse,
							RightMouse;
extern volatile MFPoint		MousePoint;

extern void	RemoveMouse();
extern void	PlaceMouse();

