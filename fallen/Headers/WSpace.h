//	WSpace.h
//	Guy Simmons, 11th August 1998.

#pragma once

//---------------------------------------------------------------

#define	ET_NONE			0
#define	ET_ROOT			1
#define	ET_MAP			2
#define	ET_MISSION		3
#define	ET_LMAP			4
#define	ET_WAYPOINT		5
#define	ET_BRIEF		6

#define	IM_ROOT			0
#define	IM_MAP			6
#define	IM_MISSION		1
#define	IM_LMAP			7
#define	IM_WAYPOINT		3
#define	IM_BRIEF		4
#define IM_SEWER		5
/*
#define	IM_ROOT			5
#define	IM_MAP			4
#define	IM_MISSION		1
#define	IM_LMAP			0
#define	IM_WAYPOINT		0
#define	IM_BRIEF		0
#define IM_SEWER		3
*/
#define IM_MESSAGE		9
#define IM_ITEM			10
#define IM_CAMERA		11
#define IM_PLAYER		12
#define IM_TRAP			13
#define	IM_SOUND		14
#define IM_MAPEXIT		15
#define	IM_COP			16
#define IM_BOMB			17
#define IM_BROKEN		2

struct WSElement
{
	std::uint8_t			ElementType;
	std::uint16_t			EventPointRef,
					MapRef,
					MissionRef;
	HTREEITEM		TreeItem;
};

//---------------------------------------------------------------

extern bool			workspace_changed;
extern HWND			ws_tree,wpt_tree;

bool init_workspace(HWND parent);
void fini_workspace();
void handle_ws_context(POINT *click_point);
bool handle_ws_dblclk(POINT *click_point);
void handle_ws_select(WSElement *the_element);
bool get_element_at_point(POINT *click_point,WSElement **the_element);
void ws_add_map();
void ws_new_mission();
void ws_del_mission();
void ws_add_light_map();
void ws_add_citsez_map();
bool create_workspace();
bool close_workspace();
bool load_workspace(bool load_default_workspace);
bool save_workspace();

//---------------------------------------------------------------
// waypointy stuff...
bool init_wptlist(HWND parent);
void fini_wptlist();
void reset_wptlist();
void fill_wptlist(Mission *mish);
HTREEITEM ws_root_waypoint(char* msg, std::int32_t type, LPARAM param);
void ws_add_waypoint(EventPoint *ep);
void ws_set_waypoint(EventPoint *ep, char ndx);
void ws_sel_waypoint(EventPoint *ep);
void ws_del_waypoint(EventPoint *ep);


//---------------------------------------------------------------

