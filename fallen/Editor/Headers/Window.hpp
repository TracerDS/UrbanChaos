// Window.hpp
// Guy Simmons, 18th February 1997.

#pragma once

#include	"ModeTab.hpp"
#include	"Primativ.hpp"
#include	"Icon.hpp"

#define	ACTIVE			(1<<0)

#define	HAS_TITLE		(1<<0)
#define	HAS_GROW		(1<<1)
#define	HAS_CLOSE		(1<<2)
#define	HAS_MAXIMIZE	(1<<3)
#define	HAS_MINIMIZE	(1<<4)
#define	HAS_VSCROLL		(1<<5)
#define	HAS_HSCROLL		(1<<6)
#define	HAS_CONTROLS	(1<<7)

#define	HAS_ICONS		(1<<8)

#define	OUTSIDE_WINDOW	0
#define	IN_WINDOW		1
#define	IN_TITLE		2
#define	IN_GROW			3
#define	IN_HSCROLL		4
#define	IN_VSCROLL		5
#define	IN_CONTENT		6
#define	IN_CONTROLS		7
#define	IN_ICONS		8


class	Window	:	public EdRect
{
	private:
		std::uint16_t			ContentColour;

		std::uint8_t			StateFlags,
						Update,
						*TopLeftPtr;
		std::uint32_t			Flags;
		char			*Title;
		std::uint16_t			ControlAreaHeight,
						ControlAreaWidth;
		EdRect			ContentRect,
						ControlRect,
						GrowRect,
						HScrollRect,
						TitleRect,
						IconRect,
						VScrollRect;
		ModeTab			*CurrentTab,
						*TabList;


		void			SetAreaSizes();

	protected:
		ControlSet		WindowControls;


	public:
		virtual inline	~Window()	{}

		void			SetupWindow(char* title,std::uint32_t flags,std::int32_t x,std::int32_t y,std::int32_t width,std::int32_t height);
		void			SetContentDrawArea();

		inline std::uint8_t	GetStateFlags()					{	return StateFlags;							}
		inline void		SetStateFlags(std::uint8_t flags)			{	StateFlags=flags;							}

		inline void		SetContentColour(std::uint32_t the_colour)	{	ContentColour=(std::uint16_t)the_colour;			}
		inline EdRect	*GetContentRect()				{	return &ContentRect;						}
		inline std::int32_t	ContentLeft()					{	return ContentRect.GetLeft();				}
		inline std::int32_t	ContentTop()					{	return ContentRect.GetTop();				}
		inline std::int32_t	ContentRight()					{	return ContentRect.GetRight();				}
		inline std::int32_t	ContentBottom()					{	return ContentRect.GetBottom();				}
		inline std::int32_t	ContentWidth()					{	return ContentRect.GetWidth();				}
		inline std::int32_t	ContentHeight()					{	return ContentRect.GetHeight();				}
		inline bool		PointInContent(MFPoint *the_point)	{	return ContentRect.PointInRect(the_point);	}
		void			FillContent(std::uint32_t the_colour);
		void			ClearContent();

		inline std::int32_t	ControlsLeft()					{	return ControlRect.GetLeft();				}
		inline std::int32_t	ControlsTop()					{	return ControlRect.GetTop();				}
		inline std::int32_t	ControlsWidth()					{	return ControlRect.GetWidth();				}
		inline std::int32_t	ControlsHeight()				{	return ControlRect.GetHeight();				}
		inline bool		PointInControls(MFPoint *the_point)	{	return ControlRect.PointInRect(the_point);	}
		inline void		SetControlsWidth(std::uint16_t width)		{	ControlAreaWidth=width;SetAreaSizes();		}
		inline void		SetControlsHeight(std::uint16_t height)		{	ControlAreaHeight=height;SetAreaSizes();	}

		void			ConstrainHeight(std::int32_t *new_height);
		void			MoveWindow(std::int32_t x,std::int32_t y);
		void			SizeWindow(std::int32_t dx,std::int32_t dy);

		inline MFPoint	*GlobalToLocal(MFPoint *the_point)	{	the_point->X-=ContentRect.GetLeft();the_point->Y-=ContentRect.GetTop();return the_point;	}
		inline MFPoint	*LocalToGlobal(MFPoint *the_point)	{	the_point->X+=ContentRect.GetLeft();the_point->Y+=ContentRect.GetTop();return the_point;	}

		void			DrawWindowFrame();
		void			DrawWindowContent();
		void			DrawWindow();
		void			DrawGrowBox();

		virtual void	DrawContent();
		virtual void	DrawControls();

		std::uint32_t			WhereInWindow(MFPoint *the_point);

		void			BringTabToFront(ModeTab *the_tab);
		void			BringTabIDToFront(std::uint16_t id);
		void			ActivateNextTab();
		void			ActivateLastTab();
		void			AddTab(ModeTab *the_tab);
		void			HandleTab(MFPoint *current_point);
		inline ModeTab	*CurrentModeTab()				{	return CurrentTab;							}

		WinBarIcon		TopIcons;
};

