// Primativ.hpp
// Guy Simmons, 26th October 1996.


#ifndef	_PRIMATIV_HPP_
#define	_PRIMATIV_HPP_


class	EdRect
{
	private:

	public:
		std::int32_t		Top,
					Left,
					Bottom,
					Right,
					Width,
					Height;
		MFPoint		TopLeft,
					BottomRight;

		void		SetRect(std::int32_t left,std::int32_t top,std::int32_t width,std::int32_t height);
		void		MoveRect(std::int32_t left,std::int32_t top);
		void		NormalRect();
		void		OffsetRect(std::int32_t dx,std::int32_t dy);
		void		ShrinkRect(std::int32_t dx,std::int32_t dy);
		void 		OutlineRect(std::uint32_t colour);
		void		OutlineInvertedRect();
		void		HiliteRect(std::uint32_t hilite,std::uint32_t lolite);
		void		FillRect(std::uint32_t colour);
		void		IndentRect(std::uint32_t hilite,std::uint32_t lolite);

		bool		PointInRect(MFPoint *the_point);
		bool		IntersectRect(EdRect *the_rect);

		inline std::int32_t	GetTop()			{	return Top;			}
		inline std::int32_t	GetLeft()			{	return Left;		}
		inline std::int32_t	GetBottom()			{	return Bottom;		}
		inline std::int32_t	GetRight()			{	return Right;		}
		inline std::int32_t	GetWidth()			{	return Width;		}
		inline std::int32_t	GetHeight()			{	return Height;		}
		inline MFPoint	GetTopLeft()		{	return TopLeft;		}
		inline MFPoint	GetBottomRight()	{	return BottomRight;	}
};

#endif
