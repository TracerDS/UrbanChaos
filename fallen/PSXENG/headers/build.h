//
// Draws buildings.
//

#pragma once


//
// This function uses the POLY module, and assumes
// that all the camera stuff has already been set up.
//

void BUILD_draw(Thing *building);

//
// bodge for now
//
void BUILD_draw_facet(Thing *p_thing,std::uint16_t facet);

//
// Draws a building you are inside.
//

void BUILD_draw_inside();



