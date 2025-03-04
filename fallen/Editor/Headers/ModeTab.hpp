// ModeTab.hpp
// Guy Simmons, 18th February 1997.

#pragma once

#include	"Controls.hpp"
#include	"CtrlSet.hpp"
#include	"Primativ.hpp"

#define	TAB_ACTIVE		(1<<0)
#define	TAB_CLEANUP		(1<<1)


class	ModeTab	:	public	ControlSet
{
	private:
		char				*Title;
		std::uint16_t				TabID;
		std::uint32_t				*ExternalUpdate;
		ModeTab				*LastModeTab,
							*NextModeTab;
		EdRect				TitleRect;

	protected:
		EdRect				ContentRect;


		
	public:
							ModeTab();
		inline void			RequestUpdate()		{	if(ExternalUpdate)*ExternalUpdate=1;	}

		void				SetupModeTab(char* the_title,std::uint16_t id,EdRect *bounding_rect,std::uint32_t *update_ptr);
		void				SetTabArea(EdRect *bounding_rect);
		void				MoveTabArea(EdRect *bounding_rect);
		void				SetTabDrawArea();
		void				ClearTab();

		void				DrawTab();
		virtual void		DrawTabContent();
		virtual void		HandleTab(MFPoint *current_point);
		virtual std::uint16_t		HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);

		inline std::uint16_t		GetTabID()						{	return TabID;					}
		inline void			SetTabID(std::uint16_t id)					{	TabID=id;						}

		inline void			SetLastTabLink(ModeTab *last_tab)	{	LastModeTab=last_tab;			}
		inline void			SetNextTabLink(ModeTab *next_tab)	{	NextModeTab=next_tab;			}
		inline ModeTab		*GetLastTabLink()				{	return LastModeTab;				}
		inline ModeTab		*GetNextTabLink()				{	return NextModeTab;				}

		inline bool			PointInTabContent(MFPoint *the_point){	return ContentRect.PointInRect(the_point);	}
		inline std::int32_t		ContentLeft()					{	return ContentRect.GetLeft();				}
		inline std::int32_t		ContentTop()					{	return ContentRect.GetTop();				}
		inline std::int32_t		ContentRight()					{	return ContentRect.GetRight();				}
		inline std::int32_t		ContentBottom()					{	return ContentRect.GetBottom();				}
		inline std::int32_t		ContentWidth()					{	return ContentRect.GetWidth();				}
		inline std::int32_t		ContentHeight()					{	return ContentRect.GetHeight();				}

		inline bool			PointInTitle(MFPoint *the_point)		{	return TitleRect.PointInRect(the_point);	}
		inline std::int32_t		TitleLeft()						{	return TitleRect.GetLeft();					}
		inline std::int32_t		TitleRight()					{	return TitleRect.GetRight();				}
		inline std::int32_t		TitleTop()						{	return TitleRect.GetTop();					}
		inline std::int32_t		TitleWidth()					{	return TitleRect.GetWidth();				}
		inline std::int32_t		TitleHeight()					{	return TitleRect.GetHeight();				}

		inline MFPoint		*GlobalToLocal(MFPoint *the_point)	{	the_point->X-=ContentRect.GetLeft();the_point->Y-=ContentRect.GetTop();return the_point;	}
		inline MFPoint		*LocalToGlobal(MFPoint *the_point)	{	the_point->X+=ContentRect.GetLeft();the_point->Y+=ContentRect.GetTop();return the_point;	}

		inline void			SetExternalUpdatePtr(std::uint32_t *ptr)	{	ExternalUpdate=ptr;		}
};


