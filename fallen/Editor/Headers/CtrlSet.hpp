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
		std::uint8_t				ControlCount,
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
		std::uint8_t				HandleControlSet(MFPoint *current_point);
		std::uint16_t				HandleControlSetClick(std::uint8_t flags,MFPoint *clicked_point);
		std::uint16_t				HandleControlSetKey(std::uint8_t the_key);

		inline Control		*GetControlList()					{	return ControlList;				}
		Control				*GetControlPtr(std::uint16_t id);
		void				SetControlState(std::uint16_t id,std::uint8_t state);
		std::uint8_t				GetControlState(std::uint16_t id);
		void				ToggleControlSelectedState(std::uint16_t id);
		void				ToggleControlActiveState(std::uint16_t id);
		void				SetMenuItemState(std::uint16_t id,std::uint16_t item,std::uint8_t state);
		void				SetPopUpItemState(CPopUp *the_popup,std::uint16_t item,std::uint8_t state);

		inline std::uint8_t		GetStateFlags()						{	return StateFlags;						}
		inline void			SetStateFlags(std::uint8_t flags)				{	StateFlags=flags;						}

		void				SetControlDrawArea();
		void				FillControlDrawArea(std::uint32_t colour);
		void				HiliteControlDrawArea(std::uint32_t hilite,std::uint32_t lolite);
		inline void			ControlSetBounds(EdRect *bounds_rect)	{	SetRect=*bounds_rect;					}
		inline EdRect		*ControlGetBounds()					{	return &SetRect;						}
		inline bool			PointInControlSet(MFPoint *the_point)	{	return SetRect.PointInRect(the_point);	}

		inline MFPoint		*GlobalToLocal(MFPoint *the_point)		{	the_point->X-=SetRect.GetLeft();the_point->Y-=SetRect.GetTop();return the_point;	}
		inline MFPoint		*LocalToGlobal(MFPoint *the_point)		{	the_point->X+=SetRect.GetLeft();the_point->Y+=SetRect.GetTop();return the_point;	}

		inline std::uint8_t		GetControlCount()					{	return ControlCount;																}
};


#endif
