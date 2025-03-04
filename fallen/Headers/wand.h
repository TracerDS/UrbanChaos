//
// A system for wandering people.
//

#pragma once


//
// Initialises the wander system. Looks for nice places to walk and
// sets the PAP_FLAG_WANDER flag.
//

void WAND_init();


//
// Returns the next place for a wandering person to wander to.
//

void WAND_get_next_place(
		Thing *p_person,
		std::int32_t *wand_world_x,
		std::int32_t *wand_world_z);


//
// Returns true if the given square is a good place to wander.
//

std::int32_t WAND_square_is_wander(std::int32_t map_x, std::int32_t map_z);


//
// Draws a cross over all the wander squares near the given place.
// 

void WAND_draw(std::int32_t map_x, std::int32_t map_z);


