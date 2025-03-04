// ComTab.h
// Guy Simmons, 9th March 1998.

#pragma once

#include	"EdCom.h"

#define	COM_MODE_NONE				0
#define	COM_MODE_SELECT_THING		1
#define	COM_MODE_SELECT_WAYPOINT	2
#define	COM_MODE_SELECT_SWITCH		3

//---------------------------------------------------------------

class	CommandTab	:	public	ModeTab
{
	private:
		std::uint16_t			DataField,
						TabMode;
		std::int32_t			TabData;
		EditCommand		*DataCommand;
		EditComList		*CurrentComList;

	public:
						CommandTab();
						~CommandTab();

		void			DrawTabContent();
		void			UpdateTab(std::uint8_t update_level);
		std::uint16_t			HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleTab(MFPoint *current_point);
		void			HandleControl(std::uint16_t control_id);
		void			DoComListPopup(MFPoint *clicked_point,EditComList *the_comlist);
		void			DoCommandPopup(MFPoint *clicked_point,std::uint16_t select_pos);
		void			CommonCommandOptions(std::uint32_t id,EditCommand *the_command);
		EditCondList	*CommandTab::SelectConditionList();

		void			DrawListsBox();
		void			DrawCurrentList();

		std::uint16_t			ListsHilitePos(MFPoint *current_point);
		EditComList		*HilitetedList(std::uint16_t select_pos);
		std::uint16_t			CurrentListHilitePos(MFPoint *current_point);
		EditCommand		*HilitetedCommand(std::uint16_t select_pos);

		inline std::uint16_t	GetTabMode()				{	return	TabMode;		}
		inline void		SetTabMode(std::uint16_t mode)			{	TabMode=mode;			}
		inline void		SetTabData(std::int32_t data)			{	TabData=data;			}
};

//---------------------------------------------------------------

