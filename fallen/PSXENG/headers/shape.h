//
// Primitive shapes...
//

#ifndef _SHAPE_
#define _SHAPE_

#include "ob.h"

//
// Draws a semi-sphere whose edge is black and whose colour in the
// middle is given.  It is drawn with additive-alpha.
//

void SHAPE_semisphere(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t dx, // Gives the direction of the semi-sphere (256 long)
    std::int32_t dy,
    std::int32_t dz,
    std::int32_t radius,
    std::int32_t page,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue);

void SHAPE_semisphere_textured(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t dx, // Gives the direction of the semi-sphere (256 long)
    std::int32_t dy,
    std::int32_t dz,
    std::int32_t radius,
    float u_mid,
    float v_mid,
    float uv_radius,
    std::int32_t page,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue);

//
// Draws a sphere.
//

void SHAPE_sphere(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t radius,
    std::uint32_t colour);

//
// Draws a sparky line.
//

#define SHAPE_MAX_SPARKY_POINTS 8

void SHAPE_sparky_line(
    std::int32_t num_points,
    std::int32_t px[],
    std::int32_t py[],
    std::int32_t pz[],
    std::uint32_t colour,
    float width);

//
// Draws a bit of glitter.
//

void SHAPE_glitter(
    std::int32_t x1,
    std::int32_t y1,
    std::int32_t z1,
    std::int32_t x2,
    std::int32_t y2,
    std::int32_t z2,
    std::uint32_t colour);

//
// Draws a trip-wire.
//

void SHAPE_tripwire(
    std::int32_t x1,
    std::int32_t y1,
    std::int32_t z1,
    std::int32_t x2,
    std::int32_t y2,
    std::int32_t z2,
    std::int32_t width,
    std::uint32_t colour,
    std::uint16_t counter, // A frame animation counter.
    std::uint8_t along);   // How far along the line the tripwire goes.

//
// Draws a waterfall.
//

void SHAPE_waterfall(
    std::int32_t map_x,
    std::int32_t map_z,
    std::int32_t map_dx,
    std::int32_t map_dz,
    std::int32_t top,
    std::int32_t bot);

//
// Draws a droplet of water.
//

void SHAPE_droplet(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t dx,
    std::int32_t dy,
    std::int32_t dz,
    std::uint32_t colour);

//
// Draws a shadow for the given prim.
//

void SHAPE_prim_shadow(OB_Info *oi);

#endif
