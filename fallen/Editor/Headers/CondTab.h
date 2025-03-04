// CondTab.h
// Guy Simmons, 16th March 1998.

#ifndef	CONDTAB_H
#define	CONDTAB_H

#include	"EdCom.h"

#define	COND_MODE_NONE			0
#define	COND_MODE_SELECT_THING	1
#define	COND_MODE_SELECT_SWITCH	2

//---------------------------------------------------------------

class	ConditionTab	:	public	ModeTab
{
	private:
		std::uint16_t			DataField,
						TabMode;
		std::int32_t			TabData;
		EditCondition	*DataCondition;
		EditCondList	*CurrentCList;


	public:
						ConditionTab();
						~ConditionTab();

		void			DrawTabContent();
		void			UpdateTab(std::uint8_t update_level);
		std::uint16_t			HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleTab(MFPoint *current_point);
		void			HandleControl(std::uint16_t control_id);
		void			DoCListPopup(MFPoint *clicked_point,EditCondList *the_clist);
		void			DoConditionPopup(MFPoint *clicked_point,std::uint16_t select_pos);
		void			CommonConditionOptions(std::uint32_t id,EditCondition *the_condition);
		EditCondList	*SelectConditionList();

		void			DrawListsBox();
		void			DrawCurrentList();

		std::uint16_t			ListsHilitePos(MFPoint *current_point);
		EditCondList	*HilitetedList(std::uint16_t select_pos);
		std::uint16_t			CurrentListHilitePos(MFPoint *current_point);
		EditCondition	*HilitetedCondition(std::uint16_t select_pos);

		inline std::uint16_t	GetTabMode()				{	return	TabMode;		}
		inline void		SetTabMode(std::uint16_t mode)			{	TabMode=mode;			}
		inline void		SetTabData(std::int32_t data)			{	TabData=data;			}
};

//---------------------------------------------------------------

#endif
