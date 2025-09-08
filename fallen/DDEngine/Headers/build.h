//
// Draws buildings.
//

#pragma once

//
// This function uses the POLY module, and assumes
// that all the camera stuff has already been set up.
//

void BUILD_draw(Entity* building);

//
// Draws a building you are inside.
//

void BUILD_draw_inside();