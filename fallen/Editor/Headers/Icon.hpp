// Window.hpp
// Guy Simmons, 18th February 1997.

#pragma once

#include	"Primativ.hpp"


struct	AWindowIcon
{
	void	(*Function)(std::uint16_t id);
	std::uint8_t	Flag;
	std::uint16_t	ImageOn;
	std::int16_t	ImageOff;
};

class	WinBarIcon	:	public  EdRect
{
	private:
		struct	AWindowIcon	*WindowIcons;
	public:
	inline				WinBarIcon()   {}
	void				DrawIcons();
	void				HandleIconClick(std::uint8_t flags,MFPoint *clicked_point);
	void				InitIcons(struct AWindowIcon *p_icons);
};

