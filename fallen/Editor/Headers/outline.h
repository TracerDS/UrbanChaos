//
// Functions for helping with the outline of shapes on grids.
//

#pragma once

typedef struct OUTLINE_Outline OUTLINE_Outline;


//
// Creates a new outline.
//

OUTLINE_Outline *OUTLINE_create(std::int32_t num_z_squares);

//
// Adds the given line to the outline.  Must be orthogonal. The shape
// can be concave or convex.  It must link up completely before it is used
// by OUTLINE_overlap().
//

void OUTLINE_add_line(
		OUTLINE_Outline *oo,
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);

//
// Frees up all the memory used in the given outline.
//

void OUTLINE_free(OUTLINE_Outline *oo);


//
// Returns true if the two outlines overlap.
//

std::int32_t OUTLINE_overlap(
		OUTLINE_Outline *oo1,
		OUTLINE_Outline *oo2);


//
// Returns true if the given line goes through an outline.
//

std::int32_t OUTLINE_intersects(
		OUTLINE_Outline* oo,
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);