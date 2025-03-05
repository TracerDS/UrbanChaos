//
// Poly-modelled animating waterfalls.
//

#ifndef _FALL_
#define _FALL_

//
// Initialises all the waterfalls.
//

void FALL_init();

//
// Creates a new waterfall. nullptr => Couldn't create one.
//

std::uint8_t FALL_create(
    float x1, float z1, float u, float v,
    float x2, float z2, float u, float v,
    float dx,
    float dz,
    float du,
    float dv,
    float top_y,
    float bot_y);

//
// Destroys the given waterfall.
//

void FALL_destroy();

//
// Processes all the waterfalls.
//

void FALL_process();

//
// Draws them all.
//

void FALL_draw();

#endif
