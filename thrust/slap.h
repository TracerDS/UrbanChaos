//
// Creates a shadowed bitmap for a light map, given the silhoutte.
// Credit to Eddie Edwards for the outline rendering idea.
//

#pragma once




//
// Initialises the SLAPPER with a new bitmap. It does not clear the bitmap!
//

#define SLAP_MAX_BITMAP_SIZE 256

void SLAP_init(
		std::uint8_t *bitmap,
		std::int32_t  bitmap_size);	// Power of 2 max of SLAP_MAX_BITMAP_SIZE


//
// Adds an outline edge given in 8-bit fixed point.
//

void SLAP_add_edge(
		std::int32_t x1, std::int32_t y1,
		std::int32_t x2, std::int32_t y2);


//
// Renders the bitmap. The image is clipped to the bitmap.
//

void SLAP_render();




