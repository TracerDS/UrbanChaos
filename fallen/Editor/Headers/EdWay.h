// EdWay.h
// Guy Simmons, 6th February 1998.

#pragma once

//---------------------------------------------------------------
// Waypoint definition.

#define	MAX_EDIT_WAYPOINTS		1000

struct EditWaypoint
{
	bool		Used;
	std::uint16_t		Next,
				Prev;
	std::int32_t		X,Y,Z;
};

extern std::uint32_t				ed_waypoint_count;
extern EditWaypoint		edit_waypoints[MAX_EDIT_WAYPOINTS];

//---------------------------------------------------------------

void	init_ed_waypoints();
std::uint16_t	alloc_ed_waypoint();
void	free_ed_waypoint(std::uint16_t wp_index);
void	link_next_waypoint(std::uint16_t link_wp,std::uint16_t next_wp);
void	link_prev_waypoint(std::uint16_t link_wp,std::uint16_t prev_wp);
void	pack_waypoints(std::uint16_t *map_table);

//---------------------------------------------------------------

