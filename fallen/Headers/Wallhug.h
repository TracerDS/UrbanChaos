#ifndef WALLHUG_H
#define WALLHUG_H

/*****************************************************************************

                                a navigation routine based on wallhugging.


To integrate it into your stuff:

define WALLHUG_HEIGHT and WALLHUG_WIDTH to be your map height (y range)
and width (x range) respectively.

WALLHUG_MAX_ORD should be std::max(WALLHUG_HEIGHT, WALLHUG_WIDTH)

WALLHUG_WALL_IN_WAY should return non-zero if there's a wall blocking
movement from cell (x,y) in direction dirn.

WALLHUG_NORTH, WALLHUG_SOUTH, WALLHUG_EAST, WALLHUG_WEST are the four
directions: set these to anything you want, as long as they're not the
same as WALLHUG_DONE and WALLHUG_FAILED_DIRN, which you can also change.


To use:

Call wallhug_trivial/tricky with a path structure with .start and .end
filled in. It will fill the structure with waypoints. These waypoints will
not include the start point, but will include the end. The .length is the
number of waypoints. There will be a direct line of sight between each
waypoint, as defined by a line connecting the middle of one waypoint to
the middle of the other.

If the path is not possible, the path will get as close to the destination
as possible, and the function will return WALLHUG_FAILED, otherwise it
will return something else.

wallhug_trivial just does a normal wallhug
wallhug_tricky cleans the path up a bit so it looks less foolish.

*****************************************************************************/

std::int32_t NAV_wall_in_way(std::int32_t x, std::int32_t y, std::int32_t dirn);

#define WALLHUG_MAX_COUNT 20000 // most steps involved in a path ever
#define WALLHUG_HEIGHT 128
#define WALLHUG_WIDTH 128
#define WALLHUG_MAX_ORD 128     // must be less than 255
#define WALLHUG_INVALID_ORD 255 // don't change this
// #define	WALLHUG_WALL_IN_WAY(x, y, dirn)			bridgehg_internal_wall_in_way(x, y, dirn)
#define WALLHUG_WALL_IN_WAY(x, y, dirn) NAV_wall_in_way(x, y, dirn)
#define WALLHUG_MAX_PTS 252 // must be less than 253

#define WALLHUG_FAILED 0x70000000 // don't change this

#define WALLHUG_NORTH 0
#define WALLHUG_EAST 1
#define WALLHUG_SOUTH 2
#define WALLHUG_WEST 3
#define WALLHUG_DONE 50
#define WALLHUG_FAILED_DIRN 51

struct wallhug_waypoint {
    std::uint8_t x, y;
};

struct wallhug_path {
    std::uint8_t length, padb;
    wallhug_waypoint start, end, waypoints[WALLHUG_MAX_PTS];
};

struct wallhug_info {
    wallhug_path path;
    std::uint32_t dirn;
    std::uint32_t handed; // where -1 means left-handed and 1 right-handed
    wallhug_waypoint current, old;
};

std::uint32_t wallhug_trivial(wallhug_path *path); // simple wallhug
std::uint32_t wallhug_tricky(wallhug_path *path);  // more expensive, less silly

// this should be an array for the steps that each direction produces. eg wallhug_dirn_steps[WALLHUG_NORTH] == {0, -1}
// it's defined here so that you don't have to change wallhug.c

extern wallhug_waypoint wallhug_dirn_steps[4];

//////////////////////////////////////////////////////////////////////////////
// extra stuff providing backdoors into parts of the code
extern std::uint32_t wallhug_current_count;
std::uint32_t wallhug_cleanup(wallhug_path *path, std::uint32_t retval);
std::uint32_t wallhug_continue_trivial(wallhug_path *path, wallhug_waypoint current, std::uint32_t max_count);
void wallhug_hugstep(wallhug_info *hugger);
bool wallhug_add_huggers_path(wallhug_path *path, wallhug_info *successful_hugger);

#define WALLHUG_ADDMOD4(a, b) (((a) + (b)) & 3)
#define WALLHUG_INVALID_WAYPOINT ((std::uint8_t) 255)
extern std::uint8_t wallhug_last_hugstart;
extern std::uint8_t wallhug_last_handed;
extern std::uint8_t wallhug_last_dirn;
extern std::uint32_t wallhug_last_hug_count;
extern bool wallhug_looking_for_last;

#endif
