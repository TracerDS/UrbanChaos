//
// 2d navigation around the city.
//

#pragma once


//
// Initiliases the navigation system from the current map.
// It invalidates all the current waypoints.
//

void NAV_init();


//
// The waypoints are given in a linked list. They don't have
// the start point but do have the end point.
//

#define NAV_WAYPOINT_FLAG_LAST		(1 << 0)
#define NAV_WAYPOINT_FLAG_PULLUP	(1 << 1)	// If you hit a col-vect going to this waypoint, then do a pull-up.

typedef struct
{
	std::uint16_t x;
	std::uint16_t z;
	std::uint8_t flag;
	std::uint8_t length;	// The number of waypoints after this one.
	std::uint16_t next;

} NAV_Waypoint;

#define NAV_MAX_WAYPOINTS 512

extern NAV_Waypoint NAV_waypoint[NAV_MAX_WAYPOINTS];

//
// Returns a pointer to the given waypoint.
//

#ifndef NDEBUG
void NAV_waypoint_check(std::uint16_t index);
#define NAV_WAYPOINT(index) (NAV_waypoint_check(index), &NAV_waypoint[index])
#else
#define NAV_WAYPOINT(index) (&NAV_waypoint[index])
#endif



//
// Returns a 2D navigation path. 
//

#define NAV_FLAG_PULLUP	(1 << 0)	// This person can do pull-ups.

std::uint16_t NAV_do(std::uint16_t x1, std::uint16_t z1, std::uint16_t x2, std::uint16_t z2, std::uint8_t flag);


//
// Gives back a waypoint.
//

void NAV_waypoint_give(std::uint16_t index);


//
// Gives back the whole nav path.
//

void NAV_path_give(std::uint16_t index);


//
// Draws a navigation path.
//

void NAV_path_draw(std::uint16_t startx, std::uint16_t startz, std::uint16_t path);



