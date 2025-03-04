#pragma once

//#include "c:\fallen\headers\collide.h"
//DEFINES

// STRUCTS


//data
//collision info


// FUNCTIONS

extern void	calc_collision_info(struct ColInfo *p_col);
//extern void	calc_collision_info(std::int32_t left,std::int32_t right,std::int32_t top,std::int32_t bottom,std::int32_t depth,std::int32_t flag);

extern void	clear_all_col_info();

extern void	interface_thing();
extern void	init_thing();
extern std::int32_t	calc_height_at(std::int32_t x,std::int32_t z);
extern calc_things_height(struct MapThing *p_thing);


//
// Line intersection. Line segments that share a point count as
// intersecting.
//

#define	DONT_INTERSECT    0
#define	DO_INTERSECT      1
#define COLLINEAR         2

std::int32_t lines_intersect(
			std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2,
			std::int32_t x3, std::int32_t y3, std::int32_t x4, std::int32_t y4,
			std::int32_t *x,
			std::int32_t *y);


