// Window.hpp
// Guy Simmons, 18th February 1997.

#ifndef	_WINDOW_HPP_
#define	_WINDOW_HPP_

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
		UWORD			ContentColour;

		UBYTE			StateFlags,
						Update,
						*TopLeftPtr;
		ULONG			Flags;
		CBYTE			*Title;
		UWORD			ControlAreaHeight,
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

		void			SetupWindow(CBYTE *title,ULONG flags,SLONG x,SLONG y,SLONG width,SLONG height);
		void			SetContentDrawArea();

		inline UBYTE	GetStateFlags()					{	return StateFlags;							}
		inline void		SetStateFlags(UBYTE flags)			{	StateFlags=flags;							}

		inline void		SetContentColour(ULONG the_colour)	{	ContentColour=(UWORD)the_colour;			}
		inline EdRect	*GetContentRect()				{	return &ContentRect;						}
		inline SLONG	ContentLeft()					{	return ContentRect.GetLeft();				}
		inline SLONG	ContentTop()					{	return ContentRect.GetTop();				}
		inline SLONG	ContentRight()					{	return ContentRect.GetRight();				}
		inline SLONG	ContentBottom()					{	return ContentRect.GetBottom();				}
		inline SLONG	ContentWidth()					{	return ContentRect.GetWidth();				}
		inline SLONG	ContentHeight()					{	return ContentRect.GetHeight();				}
		inline BOOL		PointInContent(MFPoint *the_point)	{	return ContentRect.PointInRect(the_point);	}
		void			FillContent(ULONG the_colour);
		void			ClearContent();

		inline SLONG	ControlsLeft()					{	return ControlRect.GetLeft();				}
		inline SLONG	ControlsTop()					{	return ControlRect.GetTop();				}
		inline SLONG	ControlsWidth()					{	return ControlRect.GetWidth();				}
		inline SLONG	ControlsHeight()				{	return ControlRect.GetHeight();				}
		inline BOOL		PointInControls(MFPoint *the_point)	{	return ControlRect.PointInRect(the_point);	}
		inline void		SetControlsWidth(UWORD width)		{	ControlAreaWidth=width;SetAreaSizes();		}
		inline void		SetControlsHeight(UWORD height)		{	ControlAreaHeight=height;SetAreaSizes();	}

		void			ConstrainHeight(SLONG *new_height);
		void			MoveWindow(SLONG x,SLONG y);
		void			SizeWindow(SLONG dx,SLONG dy);

		inline MFPoint	*GlobalToLocal(MFPoint *the_point)	{	the_point->X-=ContentRect.GetLeft();the_point->Y-=ContentRect.GetTop();return the_point;	}
		inline MFPoint	*LocalToGlobal(MFPoint *the_point)	{	the_point->X+=ContentRect.GetLeft();the_point->Y+=ContentRect.GetTop();return the_point;	}

		void			DrawWindowFrame();
		void			DrawWindowContent();
		void			DrawWindow();
		void			DrawGrowBox();

		virtual void	DrawContent();
		virtual void	DrawControls();

		ULONG			WhereInWindow(MFPoint *the_point);

		void			BringTabToFront(ModeTab *the_tab);
		void			BringTabIDToFront(UWORD id);
		void			ActivateNextTab();
		void			ActivateLastTab();
		void			AddTab(ModeTab *the_tab);
		void			HandleTab(MFPoint *current_point);
		inline ModeTab	*CurrentModeTab()				{	return CurrentTab;							}

		WinBarIcon		TopIcons;
};

#endif