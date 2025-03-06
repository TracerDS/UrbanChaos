//
// The new map screen
//

#ifndef _MAP_
#define _MAP_

//
// Initialises the map (gets rid of nav beacons... sets up view)
//

void MAP_init();

//
// Draws the map.
//

void MAP_draw();

//
// Adds a NAV beacon to the map.
// Removes a beacon from the map.
//

std::uint8_t MAP_beacon_create(std::int32_t x, std::int32_t z, std::int32_t index, THING_INDEX track_my_position);
void MAP_beacon_remove(std::uint8_t beacon);

//
// Processes the map.
//

void MAP_process();

//
// Draws the quick on-screen map beacon direction indicator.
//

void MAP_draw_onscreen_beacons();

#endif
