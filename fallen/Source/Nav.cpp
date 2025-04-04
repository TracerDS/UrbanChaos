//
// Navigation.
//

#include "game.h"
#include "nav.h"
#include "wallhug.h"

//
// The waypoints.
//

NAV_Waypoint NAV_waypoint[NAV_MAX_WAYPOINTS];
std::uint16_t NAV_waypoint_free; // The head of the free list.

void NAV_waypoint_check(std::uint16_t index) {
    ASSERT(WITHIN(index, 1, NAV_MAX_WAYPOINTS - 1));
}

std::uint16_t NAV_waypoint_get() {
    std::uint16_t ans = NAV_waypoint_free;

    if (ans == NULL) {
        //
        // Oh dear! Ran out of waypoints.
        //
    } else {
        NAV_waypoint_free = NAV_WAYPOINT(ans)->next;
    }

    return ans;
}

void NAV_waypoint_give(std::uint16_t index) {
    NAV_WAYPOINT(index)->next = NAV_waypoint_free;
    NAV_waypoint_free = index;
}

void NAV_path_give(std::uint16_t index) {
    std::uint16_t next;

    while (index != NULL) {
        next = NAV_WAYPOINT(index)->next;

        NAV_waypoint_give(index);

        index = next;
    }
}

void NAV_init() {
    std::int32_t i;

    //
    // Build the free list of navigation waypoints.
    //

    NAV_waypoint_free = 1;

    for (i = 1; i < NAV_MAX_WAYPOINTS - 1; i++) {
        NAV_waypoint[i].next = i + 1;
    }

    NAV_waypoint[NAV_MAX_WAYPOINTS - 1].next = 0;
}

std::int32_t NAV_wall_in_way(std::int32_t x, std::int32_t z, std::int32_t dirn) {
#ifdef TARGET_DC
    // Shouldn't be using this, apparently.
    ASSERT(false);
#endif

    std::int32_t mx;
    std::int32_t mz;

    static struct
    {
        std::int32_t dx;
        std::int32_t dz;

    } offset[4] =
        {
            {0,  -1},
            {1,  0 },
            {0,  1 },
            {-1, 0 }
    };

    ASSERT(WITHIN(dirn, 0, 3));

    mx = x + offset[dirn].dx;
    mz = z + offset[dirn].dz;

    if (!WITHIN(mx, 0, MAP_WIDTH - 1) ||
        !WITHIN(mz, 0, MAP_HEIGHT - 1)) {
        //
        // You can't navigate off the map.
        //

        return true;
    } else {
        if (MAP[MAP_INDEX(mx, mz)].Flags & FLOOR_HIDDEN) {
            return true;
        } else {
            return false;
        }
    }
}

std::uint16_t NAV_do(std::uint16_t x1, std::uint16_t z1, std::uint16_t x2, std::uint16_t z2, std::uint8_t flag) {
    std::int32_t i;

    std::int32_t sx;
    std::int32_t sz;
    std::int32_t ex;
    std::int32_t ez;

    std::uint32_t result;
    std::uint16_t ans;
    std::uint16_t next;
    std::uint16_t prev;

    struct wallhug_path wp;

    //
    // Use the middle of mapsquares.
    //

    sx = x1 >> ELE_SHIFT;
    sz = z1 >> ELE_SHIFT;

    ex = x2 >> ELE_SHIFT;
    ez = z2 >> ELE_SHIFT;

    //
    // Use Jan's wallhugger.
    //

    wp.start.x = sx;
    wp.start.y = sz;

    wp.end.x = ex;
    wp.end.y = ez;

    result = wallhug_tricky(&wp);

    if (result == WALLHUG_FAILED) {
        //
        // Did not succeed in getting all the way there.
        //
    }

    if (wp.length == 0) {
        //
        // This is wierd!
        //

        return NULL;
    } else {
        //
        // Build the linked list of waypoints.
        //

        ans = NAV_waypoint_get();

        if (ans == NULL) {
            //
            // No more waypoints!
            //

            TRACE("No more waypoints left.\n");

            return ans;
        } else {
            prev = ans;

            for (i = 0; i < wp.length; i++) {
                NAV_WAYPOINT(prev)->x = (wp.waypoints[i].x << ELE_SHIFT) + (1 << (ELE_SHIFT - 1));
                NAV_WAYPOINT(prev)->z = (wp.waypoints[i].y << ELE_SHIFT) + (1 << (ELE_SHIFT - 1));
                NAV_WAYPOINT(prev)->length = (wp.length - 1) - i;
                NAV_WAYPOINT(prev)->flag = 0;

                if (i == wp.length - 1) {
                    //
                    // The last waypoint.
                    //

                    NAV_WAYPOINT(prev)->next = NULL;
                } else {
                    //
                    // Allocate the next waypoint.
                    //

                    next = NAV_waypoint_get();

                    if (next == NULL) {
                        //
                        // Abandon the waypoints.
                        //

                        NAV_WAYPOINT(prev)->next = NULL;

                        break;
                    } else {
                        NAV_WAYPOINT(prev)->next = next;

                        //
                        // Fill the in next waypoint.
                        //

                        prev = next;
                    }
                }
            }
        }
    }

    return ans;
}

#ifndef TARGET_DC
void NAV_path_draw(std::uint16_t startx, std::uint16_t startz, std::uint16_t path) {
    std::uint16_t x1;
    std::uint16_t z1;
    std::uint16_t x2;
    std::uint16_t z2;

    x1 = startx;
    z1 = startz;

    while (path) {
        x2 = NAV_WAYPOINT(path)->x;
        z2 = NAV_WAYPOINT(path)->z;

        e_draw_3d_line_dir(x1, 0, z1, x2, 0, z2);

        x1 = x2;
        z1 = z2;

        path = NAV_WAYPOINT(path)->next;
    }
}
#endif