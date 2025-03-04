//
// Draws prims super-fast!
//

#pragma once

#include "night.h"


//
// Initialises memory.
//

void FASTPRIM_init();


//
// Draws a prim fastly. Return true if it did it okay.
//

std::int32_t FASTPRIM_draw(
		std::int32_t prim,
		float x,
		float y,
		float z,
		float matrix[9],
		NIGHT_Colour *lpc);


//
// Frees up memory.
//

void FASTPRIM_fini();












