// DrawPoint.c
// Guy Simmons, 7th October 1996.


#include	"Editor.hpp"


//---------------------------------------------------------------

void	DrawPoint8(MFPoint *the_point,std::uint32_t colour)
{
	*(WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth))	=	(std::uint8_t)colour;
}

void	DrawPoint16(MFPoint *the_point,std::uint32_t colour)
{
	std::uint16_t	*ptr;
	ptr  = (std::uint16_t*)WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth>>1);
	*ptr = (std::uint16_t)colour;

}

void	DrawPoint32(MFPoint *the_point,std::uint32_t colour)
{
	std::uint32_t	*ptr;
	ptr  = (std::uint32_t*)WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth>>2);
	*ptr = (std::uint32_t)colour;
}

//---------------------------------------------------------------

void	DrawPointC8(MFPoint *the_point,std::uint32_t colour)
{
	if	(
			the_point->X>=0					&&
			the_point->X<WorkWindowWidth	&&
			the_point->Y>=0					&&
			the_point->Y<WorkWindowHeight
		)
	{
		*(WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth))	=	(std::uint8_t)colour;
	}
}

void	DrawPointC16(MFPoint *the_point,std::uint32_t colour)
{
	std::uint16_t	*ptr;
	if	(
			the_point->X>=0					&&
			the_point->X<WorkWindowWidth	&&
			the_point->Y>=0					&&
			the_point->Y<WorkWindowHeight
		)
	{
		ptr  = (std::uint16_t*)WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth>>1);
		*ptr = (std::uint16_t)colour;
	}
}

void	DrawPointC32(MFPoint *the_point,std::uint32_t colour)
{
	std::uint32_t	*ptr;
	if	(
			the_point->X>=0					&&
			the_point->X<WorkWindowWidth	&&
			the_point->Y>=0					&&
			the_point->Y<WorkWindowHeight
		)
	{
		ptr  = (std::uint32_t*)WorkWindow+the_point->X+(the_point->Y*WorkScreenWidth>>2);
		*ptr = (std::uint32_t)colour;
	}
}

//---------------------------------------------------------------
