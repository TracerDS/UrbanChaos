//	MapView.h
//	Guy Simmons, 12th August 1998.

#ifndef	MAPVIEW_H
#define	MAPVIEW_H

#include	"Mission.h"


//---------------------------------------------------------------

extern std::int32_t			cam_x,
						cam_y,
						cam_z,
						cam_yaw,
						cam_pitch,
						cam_focus_x,
						cam_focus_y,
						cam_focus_z,
						cam_focus_dist,
						cam_matrix[9],
						cam_forward[3],
						cam_left[3];

//	The mouse.
extern std::int32_t			mouse_valid,
						mouse_over,
						mouse_world_x,
						mouse_world_y,
						mouse_world_z,
						mouse_waypoint;

extern EventPoint		*hilited_ep,
						*selected_ep,
						*link_start_ep;

extern std::uint8_t			link_mode;

extern std::int32_t			zone_colours[ZF_NUM];

//---------------------------------------------------------------

bool	init_map_view();
void	fini_map_view();
void	process_view_wind();

//---------------------------------------------------------------

void   WaypointCaption(EventPoint *ep);
char* WaypointTitle(EventPoint *ep, char* msg);
char* WaypointExtra(EventPoint *ep, char* msg);

//---------------------------------------------------------------

bool TypeHasProperties(std::int32_t type);
bool HasProperties(EventPoint *ep);
std::int32_t OpenProperties(EventPoint *ep);
void CleanProperties(EventPoint *ep);


#endif
