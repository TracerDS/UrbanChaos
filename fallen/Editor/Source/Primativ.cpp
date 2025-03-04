// Primativ.cpp
// Guy Simmons, 26th October 1996.

#include	"Editor.hpp"
#ifdef	EDITOR


//****************************************************************************


void	EdRect::SetRect(std::int32_t left,std::int32_t top,std::int32_t width,std::int32_t height)
{
	Top		=	top;
	Left	=	left;
	Bottom	=	(std::int32_t)top+(height-1);
	Right	=	(std::int32_t)left+(width-1);
	Width	=	width;
	Height	=	height;
}
	
void	EdRect::NormalRect(void)
{
	if(Right<Left)
		SWAP(Left,Right);

	if(Bottom<Top)
		SWAP(Top,Bottom);

	Width=Right-Left;
	Height=Bottom-Top;

}	

//****************************************************************************

void	EdRect::MoveRect(std::int32_t left,std::int32_t top)
{
	Top		=	top;
	Left	=	left;
	Bottom	=	(std::int32_t)top+(Height-1);
	Right	=	(std::int32_t)left+(Width-1);
}

//****************************************************************************

void	EdRect::OffsetRect(std::int32_t dx,std::int32_t dy)
{
	Top		+=	dy;
	Left	+=	dx;
	Bottom	+=	dy;
	Right	+=	dx;
}

//****************************************************************************

void	EdRect::ShrinkRect(std::int32_t dx,std::int32_t dy)
{
	Top		+=	dy;
	Left	+=	dx;
	Bottom	-=	dy;
	Right	-=	dx;
	Width	-=	dx<<1;
	Height	-=	dy<<1;
}

//****************************************************************************

void	EdRect::OutlineRect(std::uint32_t colour)
{
	DrawVLineC(Right,Top,Bottom,colour);
	DrawHLineC(Left,Right,Bottom,colour);
	DrawVLineC(Left,Top,Bottom,colour);
	DrawHLineC(Left,Right,Top,colour);
}

//****************************************************************************

void	draw_h_xord_line(std::int32_t x1,std::int32_t x2,std::int32_t y1)
{
	std::uint32_t		c0;
	switch (WorkScreenDepth)
	{
		case 1:
		{
			
			std::uint8_t		*the_line;
			if(y1>=0 && y1<WorkWindowHeight)
			{
				if(x1 < 0)
					x1	=	0;
				if(x2 >= WorkWindowWidth)
					x2	=	WorkWindowWidth-1;

				the_line	=	WorkWindow+x1+(y1*WorkScreenWidth);
				for(c0=x1;c0<x2;c0+=2,the_line+=2)
					*the_line	=	(std::uint8_t)(*(the_line)^0xaa);
			}
		}

			break;
		case 2:
		{
			
			std::uint16_t		*the_line;
			if(y1>=0 && y1<WorkWindowHeight)
			{
				if(x1 < 0)
					x1	=	0;
				if(x2 >= WorkWindowWidth)
					x2	=	WorkWindowWidth-1;

				the_line	=	(std::uint16_t*)WorkWindow+x1+(y1*WorkScreenWidth>>1);
				for(c0=x1;c0<x2;c0+=2,the_line+=2)
					*the_line	=	(std::uint16_t)(*(the_line)^0xaaaa);
			}
		}

			break;
		case 4:
		{
			
			std::uint32_t		*the_line;
			if(y1>=0 && y1<WorkWindowHeight)
			{
				if(x1 < 0)
					x1	=	0;
				if(x2 >= WorkWindowWidth)
					x2	=	WorkWindowWidth-1;

				the_line	=	(std::uint32_t*)WorkWindow+x1+(y1*WorkScreenWidth>>2);
				for(c0=x1;c0<x2;c0+=2,the_line+=2)
					*the_line	=	(std::uint32_t)(*(the_line)^0xaaaaaa);
			}
		}

			break;
	}

}

void	draw_v_xord_line(std::int32_t x1,std::int32_t y1,std::int32_t y2)
{
	std::uint32_t		c0;

	switch (WorkScreenDepth)
	{
		case 1:
		{
			
			std::uint8_t		*the_line;

			if(x1>=0 && x1<WorkWindowWidth)
			{
				if(y1 < 0)
					y1	=	0;
				if(y2 >= WorkWindowHeight)
					y2	=	WorkWindowHeight-1;

				the_line	=	WorkWindow+x1+(y1*WorkScreenWidth);
				for(c0=y1;c0<y2;c0+=2,the_line+=(WorkScreenWidth<<1))
					*the_line	=	(std::uint8_t)(*(the_line)^0xaa);
			}
			break;
		}
		case 2:
		{
			
			std::uint16_t		*the_line;

			if(x1>=0 && x1<WorkWindowWidth)
			{
				if(y1 < 0)
					y1	=	0;
				if(y2 >= WorkWindowHeight)
					y2	=	WorkWindowHeight-1;

				the_line	=	(std::uint16_t*)WorkWindow+x1+(y1*WorkScreenWidth>>1);
				for(c0=y1;c0<y2;c0+=2,the_line+=(WorkScreenWidth))
					*the_line	=	(std::uint16_t)(*(the_line)^0xaaaa);
			}
			break;
		}
		case 4:
		{
			
			std::uint32_t		*the_line;

			if(x1>=0 && x1<WorkWindowWidth)
			{
				if(y1 < 0)
					y1	=	0;
				if(y2 >= WorkWindowHeight)
					y2	=	WorkWindowHeight-1;

				the_line	=	(std::uint32_t*)WorkWindow+x1+(y1*WorkScreenWidth>>2);
				for(c0=y1;c0<y2;c0+=2,the_line+=(WorkScreenWidth>>1))
					*the_line	=	(std::uint32_t)(*(the_line)^0xaaaaaa);
			}
			break;
		}
	}
}

void	EdRect::OutlineInvertedRect(void)
{
	draw_h_xord_line(Left,Right,Top);
	draw_h_xord_line(Left,Right,Bottom);
	draw_v_xord_line(Left,Top,Bottom);
	draw_v_xord_line(Right,Top,Bottom);
}

//****************************************************************************

void	EdRect::HiliteRect(std::uint32_t hilite,std::uint32_t lolite)
{
	DrawVLineC(Right,Top,Bottom,lolite);
	DrawHLineC(Left,Right,Bottom,lolite);
	DrawVLineC(Left,Top,Bottom,hilite);
	DrawHLineC(Left,Right,Top,hilite);
}

//****************************************************************************

void	EdRect::FillRect(std::uint32_t colour)
{
	DrawBoxC(Left,Top,Width,Height,colour);
}


//****************************************************************************

void	EdRect::IndentRect(std::uint32_t hilite,std::uint32_t lolite)
{
	DrawVLineC(Right,Top,Bottom,lolite);
	DrawHLineC(Left,Right,Bottom,lolite);
	DrawVLineC(Left,Top,Bottom,hilite);
	DrawHLineC(Left,Right,Top,hilite);

	DrawVLineC(Right-2,Top+2,Bottom-2,hilite);
	DrawHLineC(Left+2,Right-2,Bottom-2,lolite);
	DrawVLineC(Left+2,Top+2,Bottom-2,hilite);
	DrawHLineC(Left+2,Right-2,Top+2,hilite);
}

//****************************************************************************

bool	EdRect::PointInRect(MFPoint *the_point)
{
	if(the_point->X >= Left && the_point->X <= Right)
	{
		if(the_point->Y >= Top && the_point->Y <= Bottom)
			return 1;
	}
	return 0;
}

//****************************************************************************

//assumes rectangles are normal (i.e right is on the right

bool	EdRect::IntersectRect(EdRect *the_rect)
{
	std::int32_t	flags=0;
	MFPoint	p;

	if(the_rect->Right<this->Left)
		return(0);

	if(the_rect->Left>this->Right)
		return(0);

	if(the_rect->Top>this->Bottom)
		return(0);

	if(the_rect->Bottom<this->Top)
		return(0);

	return(1);
}

//****************************************************************************

#endif
