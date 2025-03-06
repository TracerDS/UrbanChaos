//	WaypointSetup.h
//	Matthew Rosenfeld, 11th November 1998.

#ifndef _WAYPOINTSETUP_H_
#define _WAYPOINTSETUP_H_

#include "Mission.h"

//---------------------------------------------------------------

void do_waypoint_setup(EventPoint *ep);
char *get_waypoint_message(EventPoint *ep, char *msg);

//---------------------------------------------------------------

#endif
