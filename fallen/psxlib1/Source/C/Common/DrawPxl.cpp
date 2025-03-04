// DrawPixel.c
// Guy Simmons, 7th October 1996.


#include	<MFHeader.h>


//---------------------------------------------------------------

void DrawPixel8(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	*(WorkWindow+x+(y*WorkScreenWidth))	=	(std::uint8_t)colour;
}

void DrawPixel16(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	std::uint16_t	*ptr;
	ptr  = (std::uint16_t*)WorkWindow+x+(y*WorkScreenWidth>>1);
	*ptr = (std::uint16_t)colour;
}

void DrawPixel32(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	std::uint32_t	*ptr;
	ptr  = (std::uint32_t*)WorkWindow+x+(y*WorkScreenWidth>>2);
	*ptr = (std::uint32_t)colour;
}

//---------------------------------------------------------------

void DrawPixelC8(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	if(x>=0 && x<WorkWindowWidth && y>=0 && y<WorkWindowHeight)
	{
		*(WorkWindow+x+(y*WorkScreenWidth))	=	(std::uint8_t)colour;
	}
}

void DrawPixelC16(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	if(x>=0 && x<WorkWindowWidth && y>=0 && y<WorkWindowHeight)
	{
		std::uint16_t	*ptr;
		ptr  = (std::uint16_t*)WorkWindow+x+(y*WorkScreenWidth>>1);
		*ptr = (std::uint16_t)colour;
	}
}

void DrawPixelC32(std::int32_t x,std::int32_t y,std::uint32_t colour)
{
	if(x>=0 && x<WorkWindowWidth && y>=0 && y<WorkWindowHeight)
	{
		std::uint32_t	*ptr;
		ptr  = (std::uint32_t*)WorkWindow+x+(y*WorkScreenWidth>>2);
		*ptr = (std::uint32_t)colour;
	}
}

//---------------------------------------------------------------
