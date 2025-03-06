//
// Drawing rotating prims.
//

#pragma once

#include "c:\fallen\headers\light.h"

//
// Draws a mesh using the POLY module.
//
extern void CHOPPER_draw_chopper(Thing *p_chopper);

std::int32_t MESH_draw_poly(
    std::int32_t prim,
    MAPCO16 at_x,
    MAPCO16 at_y,
    MAPCO16 at_z,
    std::int32_t i_yaw,
    std::int32_t i_pitch,
    std::int32_t i_roll,
    LIGHT_Colour *lpc,
    std::uint8_t fade);
/*
void MESH_draw_poly(
                std::int32_t         prim,
                MAPCO16	      at_x,
                MAPCO16       at_y,
                MAPCO16	      at_z,
                std::int32_t         i_yaw,
                std::int32_t         i_pitch,
                std::int32_t         i_roll
                );
*/

