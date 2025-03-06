//
// An engine for the qmap.
//

#ifndef _QENG_
#define _QENG_

#include "..\headers\qmap.h"

//
// Once at the start of the whole program.
//

void QENG_init();

//
// Debug messages drawn to the screen.
//

void MSG_add(char* message, ...);

//
// Where everything is drawn from.
//

void QENG_set_camera(
    float world_x,
    float world_y,
    float world_z,
    float yaw,
    float pitch,
    float roll);

//
// Clears the screen.
//

void QENG_clear_screen();

//
// Draws a line in the world. Sets QENG_mouse_over and QENG_mouse_pos if the
// mouse is over the line.
//

extern std::int32_t QENG_mouse_over;
extern float QENG_mouse_pos_x; // Position in the world
extern float QENG_mouse_pos_y;
extern float QENG_mouse_pos_z;

void QENG_world_line(
    std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t width1, std::uint32_t colour1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t width2, std::uint32_t colour2,
    bool sort_to_front);

//
// Draws a QMAP_Draw structure.
//

void QENG_draw(QMAP_Draw* qd);

//
// Draws everything.
//

void QENG_render();

//
// Flips in the backbuffer.
//

void QENG_flip();

//
// Once at the end of the whole program.
//

void QENG_fini();

#endif
