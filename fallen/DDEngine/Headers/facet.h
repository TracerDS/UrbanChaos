//
// Facet drawing functions.
//

#ifndef _FACET_
#define _FACET_

//
// Draws the given facet.
//

void FACET_draw(std::int32_t facet, std::uint8_t alpha);

//
// Draws all the walkable faces for the given building.
//

void FACET_draw_walkable(std::int32_t building);

//
// Projects a shadow onto the crinkled facet. When it has made a
// properly crinkled polygon, it calls AENG_add_projected_shadow_poly().
//
// Only call this function on NORMAL facets!
//

void FACET_project_crinkled_shadow(std::int32_t facet);

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

#endif
