// CtrlSet.hpp
// Guy Simmons, 25th November 1996.

#ifndef	_CTRLSET_HPP_
#define	_CTRLSET_HPP_

#include	"Controls.hpp"
#include	"Primativ.hpp"

#define	CS_ACTIVE		(1<<0)
#define	CS_CLEANUP		(1<<1)


class	ControlSet
{
	private:
		UBYTE				ControlCount,
							StateFlags;
		Control				*ControlList,
							*CurrentControl;
		EdRect				SetRect;

		void				AddControl(Control *the_control);

	protected:

		
	public:
							ControlSet(ControlDef *defs);
							ControlSet();
		virtual				~ControlSet();
		void				InitControlSet(ControlDef *defs);
		void				FiniControlSet();

		void				DrawControlSet();
		UBYTE				HandleControlSet(MFPoint *current_point);
		UWORD				HandleControlSetClick(UBYTE flags,MFPoint *clicked_point);
		UWORD				HandleControlSetKey(UBYTE the_key);

		inline Control		*GetControlList()					{	return ControlList;				}
		Control				*GetControlPtr(UWORD id);
		void				SetControlState(UWORD id,UBYTE state);
		UBYTE				GetControlState(UWORD id);
		void				ToggleControlSelectedState(UWORD id);
		void				ToggleControlActiveState(UWORD id);
		void				SetMenuItemState(UWORD id,UWORD item,UBYTE state);
		void				SetPopUpItemState(CPopUp *the_popup,UWORD item,UBYTE state);

		inline UBYTE		GetStateFlags()						{	return StateFlags;						}
		inline void			SetStateFlags(UBYTE flags)				{	StateFlags=flags;						}

		void				SetControlDrawArea();
		void				FillControlDrawArea(ULONG colour);
		void				HiliteControlDrawArea(ULONG hilite,ULONG lolite);
		inline void			ControlSetBounds(EdRect *bounds_rect)	{	SetRect=*bounds_rect;					}
		inline EdRect		*ControlGetBounds()					{	return &SetRect;						}
		inline bool			PointInControlSet(MFPoint *the_point)	{	return SetRect.PointInRect(the_point);	}

		inline MFPoint		*GlobalToLocal(MFPoint *the_point)		{	the_point->X-=SetRect.GetLeft();the_point->Y-=SetRect.GetTop();return the_point;	}
		inline MFPoint		*LocalToGlobal(MFPoint *the_point)		{	the_point->X+=SetRect.GetLeft();the_point->Y+=SetRect.GetTop();return the_point;	}

		inline UBYTE		GetControlCount()					{	return ControlCount;																}
};


#endif
