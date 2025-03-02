// Primativ.hpp
// Guy Simmons, 26th October 1996.


#ifndef	_PRIMATIV_HPP_
#define	_PRIMATIV_HPP_


class	EdRect
{
	private:

	public:
		SLONG		Top,
					Left,
					Bottom,
					Right,
					Width,
					Height;
		MFPoint		TopLeft,
					BottomRight;

		void		SetRect(SLONG left,SLONG top,SLONG width,SLONG height);
		void		MoveRect(SLONG left,SLONG top);
		void		NormalRect();
		void		OffsetRect(SLONG dx,SLONG dy);
		void		ShrinkRect(SLONG dx,SLONG dy);
		void 		OutlineRect(ULONG colour);
		void		OutlineInvertedRect();
		void		HiliteRect(ULONG hilite,ULONG lolite);
		void		FillRect(ULONG colour);
		void		IndentRect(ULONG hilite,ULONG lolite);

		BOOL		PointInRect(MFPoint *the_point);
		BOOL		IntersectRect(EdRect *the_rect);

		inline SLONG	GetTop()			{	return Top;			}
		inline SLONG	GetLeft()			{	return Left;		}
		inline SLONG	GetBottom()			{	return Bottom;		}
		inline SLONG	GetRight()			{	return Right;		}
		inline SLONG	GetWidth()			{	return Width;		}
		inline SLONG	GetHeight()			{	return Height;		}
		inline MFPoint	GetTopLeft()		{	return TopLeft;		}
		inline MFPoint	GetBottomRight()	{	return BottomRight;	}
};

#endif
