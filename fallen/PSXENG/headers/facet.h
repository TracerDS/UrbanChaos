//
// Facet drawing functions.
//

#pragma once



//
// Draws the given facet.
//

void FACET_draw(std::int32_t facet,std::uint8_t fade_alpha);


//
// Draws all the walkable faces for the given building.
//

void FACET_draw_walkable(std::int32_t building);
void FACET_draw_ware_walkable(std::int32_t build);


//
// NOT USED ANY MORE!
//

//
// Draws a sewer ladder. All coordinates and the height are
// in hi-res mapsquares
//

void FACET_draw_ns_ladder(
		std::int32_t x1,
		std::int32_t z1,
		std::int32_t x2,
		std::int32_t z2,
		std::int32_t height);



