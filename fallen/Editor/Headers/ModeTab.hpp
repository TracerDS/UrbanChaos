// ModeTab.hpp
// Guy Simmons, 18th February 1997.

#ifndef	_MODETAB_HPP_
#define	_MODETAB_HPP_

#include	"Controls.hpp"
#include	"CtrlSet.hpp"
#include	"Primativ.hpp"

#define	TAB_ACTIVE		(1<<0)
#define	TAB_CLEANUP		(1<<1)


class	ModeTab	:	public	ControlSet
{
	private:
		CBYTE				*Title;
		UWORD				TabID;
		ULONG				*ExternalUpdate;
		ModeTab				*LastModeTab,
							*NextModeTab;
		EdRect				TitleRect;

	protected:
		EdRect				ContentRect;


		
	public:
							ModeTab();
		inline void			RequestUpdate()		{	if(ExternalUpdate)*ExternalUpdate=1;	}

		void				SetupModeTab(CBYTE *the_title,UWORD id,EdRect *bounding_rect,ULONG *update_ptr);
		void				SetTabArea(EdRect *bounding_rect);
		void				MoveTabArea(EdRect *bounding_rect);
		void				SetTabDrawArea();
		void				ClearTab();

		void				DrawTab();
		virtual void		DrawTabContent();
		virtual void		HandleTab(MFPoint *current_point);
		virtual UWORD		HandleTabClick(UBYTE flags,MFPoint *clicked_point);

		inline UWORD		GetTabID()						{	return TabID;					}
		inline void			SetTabID(UWORD id)					{	TabID=id;						}

		inline void			SetLastTabLink(ModeTab *last_tab)	{	LastModeTab=last_tab;			}
		inline void			SetNextTabLink(ModeTab *next_tab)	{	NextModeTab=next_tab;			}
		inline ModeTab		*GetLastTabLink()				{	return LastModeTab;				}
		inline ModeTab		*GetNextTabLink()				{	return NextModeTab;				}

		inline bool			PointInTabContent(MFPoint *the_point){	return ContentRect.PointInRect(the_point);	}
		inline SLONG		ContentLeft()					{	return ContentRect.GetLeft();				}
		inline SLONG		ContentTop()					{	return ContentRect.GetTop();				}
		inline SLONG		ContentRight()					{	return ContentRect.GetRight();				}
		inline SLONG		ContentBottom()					{	return ContentRect.GetBottom();				}
		inline SLONG		ContentWidth()					{	return ContentRect.GetWidth();				}
		inline SLONG		ContentHeight()					{	return ContentRect.GetHeight();				}

		inline bool			PointInTitle(MFPoint *the_point)		{	return TitleRect.PointInRect(the_point);	}
		inline SLONG		TitleLeft()						{	return TitleRect.GetLeft();					}
		inline SLONG		TitleRight()					{	return TitleRect.GetRight();				}
		inline SLONG		TitleTop()						{	return TitleRect.GetTop();					}
		inline SLONG		TitleWidth()					{	return TitleRect.GetWidth();				}
		inline SLONG		TitleHeight()					{	return TitleRect.GetHeight();				}

		inline MFPoint		*GlobalToLocal(MFPoint *the_point)	{	the_point->X-=ContentRect.GetLeft();the_point->Y-=ContentRect.GetTop();return the_point;	}
		inline MFPoint		*LocalToGlobal(MFPoint *the_point)	{	the_point->X+=ContentRect.GetLeft();the_point->Y+=ContentRect.GetTop();return the_point;	}

		inline void			SetExternalUpdatePtr(ULONG *ptr)	{	ExternalUpdate=ptr;		}
};

#endif

