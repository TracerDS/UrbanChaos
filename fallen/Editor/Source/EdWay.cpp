// EdWay.cpp
// Guy Simmons, 6th February 1998

#include "Editor.hpp"
#include "EdWay.h"

//---------------------------------------------------------------

std::uint32_t ed_waypoint_count = 0;
EditWaypoint edit_waypoints[MAX_EDIT_WAYPOINTS];

//---------------------------------------------------------------

void init_ed_waypoints() {
    ed_waypoint_count = 0;
    memset(edit_waypoints, 0, sizeof(edit_waypoints));
}

//---------------------------------------------------------------

std::uint16_t alloc_ed_waypoint() {
    std::uint16_t c0;

    for (c0 = 1; c0 < MAX_EDIT_WAYPOINTS; c0++) {
        if (!edit_waypoints[c0].Used) {
            edit_waypoints[c0].Used = true;
            edit_waypoints[c0].Next = 0;
            edit_waypoints[c0].Prev = 0;

            ed_waypoint_count++;
            return c0;
        }
    }
    return 0;
}

//---------------------------------------------------------------

void free_ed_waypoint(std::uint16_t wp_index) {
    std::uint16_t next_index,
        prev_index;

    next_index = edit_waypoints[wp_index].Next;
    prev_index = edit_waypoints[wp_index].Prev;

    if (prev_index)
        edit_waypoints[prev_index].Next = next_index;

    if (next_index)
        edit_waypoints[next_index].Prev = prev_index;

    edit_waypoints[wp_index].Used = false;
    ed_waypoint_count--;
}

//---------------------------------------------------------------

void link_next_waypoint(std::uint16_t link_wp, std::uint16_t next_wp) {
    edit_waypoints[link_wp].Prev = edit_waypoints[next_wp].Prev;
    if (edit_waypoints[next_wp].Prev) {
        edit_waypoints[edit_waypoints[next_wp].Prev].Next = link_wp;
    }
    edit_waypoints[next_wp].Prev = link_wp;
    edit_waypoints[link_wp].Next = next_wp;
}

//---------------------------------------------------------------

void link_prev_waypoint(std::uint16_t link_wp, std::uint16_t prev_wp) {
    edit_waypoints[link_wp].Next = edit_waypoints[prev_wp].Next;
    if (edit_waypoints[prev_wp].Next) {
        edit_waypoints[edit_waypoints[prev_wp].Next].Prev = link_wp;
    }
    edit_waypoints[prev_wp].Next = link_wp;
    edit_waypoints[link_wp].Prev = prev_wp;
}

//---------------------------------------------------------------

void pack_waypoints(std::uint16_t *map_table) {
    std::uint16_t c0,
        temp_index;
    std::uint32_t count = 0;
    EditWaypoint *temp_waypoints;

    temp_waypoints = (EditWaypoint *) MemAlloc(sizeof(EditWaypoint) * MAX_EDIT_WAYPOINTS);
    if (temp_waypoints) {
        map_table[0] = 0;
        temp_index = 1;
        memcpy(temp_waypoints, edit_waypoints, sizeof(EditWaypoint) * MAX_EDIT_WAYPOINTS);
        memset(edit_waypoints, 0, sizeof(EditWaypoint) * MAX_EDIT_WAYPOINTS);

        // First of all get rid of all the gaps in the table.
        for (c0 = 1; c0 < MAX_EDIT_WAYPOINTS; c0++) {
            if (temp_waypoints[c0].Used) {
                map_table[c0] = temp_index;
                edit_waypoints[temp_index] = temp_waypoints[c0];
                temp_index++;

                count++;
                if (count == ed_waypoint_count)
                    break;
            }
        }

        // Now adjust all the links.
        for (c0 = 1; c0 < MAX_EDIT_WAYPOINTS; c0++) {
            if (edit_waypoints[c0].Used) {
                edit_waypoints[c0].Next = map_table[edit_waypoints[c0].Next];
                edit_waypoints[c0].Prev = map_table[edit_waypoints[c0].Prev];
            } else // As all the entries are now packed, the first gap is the end.
                break;
        }

        MemFree(temp_waypoints);
    }
}

//---------------------------------------------------------------
