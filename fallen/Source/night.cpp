//
// Cached lighting.
//

#include "game.h"
#include "heap.h"
#include "night.h"
#include "ob.h"
#include "pap.h"
#include "fmatrix.h"
#include "heap.h"
#include "supermap.h"
#include "..\ledit\headers\ed.h"
#include "memory.h"
#include "ware.h"
#include "mav.h"
#include <algorithm>

#define NIGHT_FLAG_INSIDE (1 << 0)

NIGHT_Slight *NIGHT_slight; //[NIGHT_MAX_SLIGHTS];
std::int32_t NIGHT_slight_upto;

//
// The mapwho for static lights.
//

NIGHT_Smap_2d *NIGHT_smap; //[PAP_SIZE_LO][PAP_SIZE_LO];

NIGHT_Dlight *NIGHT_dlight; //[NIGHT_MAX_DLIGHTS];
std::uint8_t NIGHT_dlight_free;
std::uint8_t NIGHT_dlight_used;

//
// The cached lighting.
//

NIGHT_Square NIGHT_square[NIGHT_MAX_SQUARES];
std::uint8_t NIGHT_square_free;
std::int32_t NIGHT_square_num_used;
std::uint8_t NIGHT_cache[PAP_SIZE_LO][PAP_SIZE_LO];

NIGHT_Dfcache NIGHT_dfcache[NIGHT_MAX_DFCACHES];
std::uint8_t NIGHT_dfcache_free;
std::uint8_t NIGHT_dfcache_used;

std::uint32_t NIGHT_flag;
std::uint8_t NIGHT_lampost_radius;
std::int8_t NIGHT_lampost_red;
std::int8_t NIGHT_lampost_green;
std::int8_t NIGHT_lampost_blue;
NIGHT_Colour NIGHT_sky_colour;

// ========================================================
//
// AMBIENT LIGHT
//
// ========================================================

D3DCOLOR NIGHT_amb_d3d_colour;
D3DCOLOR NIGHT_amb_d3d_specular;
std::int32_t NIGHT_amb_red;
std::int32_t NIGHT_amb_green;
std::int32_t NIGHT_amb_blue;
std::int32_t NIGHT_amb_norm_x;
std::int32_t NIGHT_amb_norm_y;
std::int32_t NIGHT_amb_norm_z;

//
// The normal should be normalised to 256.
//

void NIGHT_ambient(
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    std::int32_t norm_x,
    std::int32_t norm_y,
    std::int32_t norm_z) {
    NIGHT_Colour amb_colour;

    NIGHT_amb_red = red * 80 >> 8;
    NIGHT_amb_green = green * 80 >> 8;
    NIGHT_amb_blue = blue * 80 >> 8;
    NIGHT_amb_norm_x = norm_x;
    NIGHT_amb_norm_y = norm_y;
    NIGHT_amb_norm_z = norm_z;

    amb_colour.red = NIGHT_amb_red;
    amb_colour.green = NIGHT_amb_green;
    amb_colour.blue = NIGHT_amb_blue;

    NIGHT_get_d3d_colour(
        amb_colour,
        &NIGHT_amb_d3d_colour,
        &NIGHT_amb_d3d_specular);
}

NIGHT_Colour NIGHT_ambient_at_point(
    std::int32_t norm_x,
    std::int32_t norm_y,
    std::int32_t norm_z) {
    NIGHT_Colour ans;

    std::int32_t dprod;

    dprod =
        norm_x * NIGHT_amb_norm_x +
            norm_y * NIGHT_amb_norm_y +
            norm_z * NIGHT_amb_norm_z >>
        10;

    dprod += 192;

    ans.red = (NIGHT_amb_red * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;
    ans.green = (NIGHT_amb_green * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;
    ans.blue = (NIGHT_amb_blue * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

    return ans;
}

// ========================================================
//
// STATIC LIGHTS
//
// ========================================================

void NIGHT_slight_compress() {
    std::int32_t x;
    std::int32_t z;

    NIGHT_Smap *ns;

    NIGHT_Slight *comp;
    std::int32_t comp_upto;

    comp = (NIGHT_Slight *) MemAlloc(sizeof(NIGHT_Slight) * NIGHT_MAX_SLIGHTS);
    comp_upto = 0;

    if (comp) {
        for (x = 0; x < PAP_SIZE_LO; x++)
            for (z = 0; z < PAP_SIZE_LO; z++) {
                ns = &NIGHT_smap[x][z];

                ASSERT(comp_upto + ns->number <= NIGHT_MAX_SLIGHTS);

                memcpy(&comp[comp_upto], &NIGHT_slight[ns->index], sizeof(NIGHT_Slight) * ns->number);

                ns->index = comp_upto;
                comp_upto += ns->number;
            }

        //
        // Copy the compressed array over the old array.
        //

        memcpy(NIGHT_slight, comp, sizeof(NIGHT_Slight) * NIGHT_MAX_SLIGHTS);
        NIGHT_slight_upto = comp_upto;

        MemFree(comp);
    }
}

void NIGHT_slight_init() {
    memset((std::uint8_t *) NIGHT_smap, 0, sizeof(std::uint16_t) * PAP_SIZE_LO * PAP_SIZE_LO);

    NIGHT_slight_upto = 0;
}
#ifndef PSX
std::int32_t NIGHT_slight_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::uint8_t radius,
    std::int8_t red,
    std::int8_t green,
    std::int8_t blue) {
    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    std::int32_t map_x = x >> PAP_SHIFT_LO;
    std::int32_t map_z = z >> PAP_SHIFT_LO;

    ASSERT(WITHIN(map_x, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(map_z, 0, PAP_SIZE_LO - 1));

    ns = &NIGHT_smap[map_x][map_z];

    if (ns->index + ns->number == NIGHT_slight_upto && NIGHT_slight_upto < NIGHT_MAX_SLIGHTS) {
        //
        // This squares static lights are already at the end of the array.
        // And there enough memory to add another one...
        //
    } else {
        //
        // We must copy this squares lights to the end of the array. Is
        // there enough room?
        //

        if (NIGHT_slight_upto + ns->number + 1 > NIGHT_MAX_SLIGHTS) {
            //
            // Not enough room- try compressing the static lights.
            //

            NIGHT_slight_compress();

            if (NIGHT_slight_upto + ns->number + 1 > NIGHT_MAX_SLIGHTS) {
                //
                // Still not enough room!
                //

                return false;
            }
        }

        //
        // Copy the squares to the end of the array.
        //

        memcpy(&NIGHT_slight[NIGHT_slight_upto], &NIGHT_slight[ns->index], sizeof(NIGHT_Slight) * ns->number);

        ns->index = NIGHT_slight_upto;
        NIGHT_slight_upto += ns->number;
    }

    //
    // Put the new light at the end of the array.
    //

    ASSERT(WITHIN(NIGHT_slight_upto, 0, NIGHT_MAX_SLIGHTS - 1));

    nsl = &NIGHT_slight[NIGHT_slight_upto];

    //
    // (x,z) of the light is relative to the mapsquare the light is in.
    //

    nsl->x = x >> 2;
    nsl->y = y;
    nsl->z = z >> 2;
    nsl->red = red >> 1;
    nsl->green = green >> 1;
    nsl->blue = blue >> 1;
    nsl->radius = radius;

    //
    // Set the bottom bit of blue depending on whether this light is inside a
    // building or not.
    //

    nsl->blue &= 0xfe;

    {
        std::uint16_t calc_inside_for_xyz(std::int32_t x, std::int32_t y, std::int32_t z, std::uint16_t * room);

        std::uint16_t room;

        /*		if (calc_inside_for_xyz(x,y,z,&room))
                        {
                                nsl->blue|=1;
                        }
                        else*/
        {
            //
            // The light might also be inside if it is in a warehouse.
            //

            if (y < PAP_calc_map_height_at(x, z)) {
                nsl->blue |= 1;
            }
        }
    }

    ns->number += 1;
    NIGHT_slight_upto += 1;

    return true;
}
void NIGHT_slight_delete(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::uint8_t radius,
    std::int8_t red,
    std::int8_t green,
    std::int8_t blue) {
    std::int32_t i;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t lx;
    std::int32_t lz;

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    mx = x >> PAP_SHIFT_LO;
    mz = z >> PAP_SHIFT_LO;

    ASSERT(WITHIN(mx, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(mz, 0, PAP_SIZE_LO - 1));

    ns = &NIGHT_smap[mx][mz];

    map_x = mx << PAP_SHIFT_LO;
    map_z = mz << PAP_SHIFT_LO;

    for (i = 0; i < ns->number; i++) {
        ASSERT(WITHIN(ns->index + i, 0, NIGHT_MAX_SLIGHTS - 1));

        nsl = &NIGHT_slight[ns->index + i];

        if ((nsl->x << 2) + map_x == (x & ~3) &&
            (nsl->z << 2) + map_z == (z & ~3) &&
            nsl->y == y &&
            nsl->red == (red >> 1) &&
            nsl->green == (green >> 1) &&
            (nsl->blue >> 1) == (blue >> 2) &&
            nsl->radius == radius) {
            //
            // This is the light we need to delete.
            //

            NIGHT_slight[ns->index + i] = NIGHT_slight[ns->index + ns->number - 1];

            ns->number -= 1;

            return;
        }
    }

    //
    // Trying to delete a light that doesn't exist.
    //

    ASSERT(0);

    return;
}

void NIGHT_slight_delete_all() {
    memset((std::uint8_t *) NIGHT_smap, 0, sizeof(std::uint8_t) * PAP_SIZE_LO * PAP_SIZE_LO);

    NIGHT_slight_upto = 0;
}
#endif

//
// Lampost lights
//

typedef struct
{
    std::uint16_t x;
    std::int16_t y;
    std::uint16_t z;

} NIGHT_Llight;

#define NIGHT_MAX_LLIGHTS 16

NIGHT_Llight NIGHT_llight[NIGHT_MAX_LLIGHTS];
std::int32_t NIGHT_llight_upto;

//
// Lights the given lo-res mapsquare.
//

typedef struct
{
    std::int32_t nx;
    std::int32_t ny;
    std::int32_t nz;
    std::int32_t height;

} NIGHT_Precalc;

typedef NIGHT_Precalc NIGHT_Preblock[PAP_BLOCKS];

void NIGHT_light_mapsquare(std::int32_t lo_map_x, std::int32_t lo_map_z, NIGHT_Colour *colour, std::int32_t floor_y, std::int32_t inside) {
    std::int32_t i;

    std::int32_t x;
    std::int32_t z;

    std::int32_t mx1;
    std::int32_t mz1;
    std::int32_t mx2;
    std::int32_t mz2;

    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t hi_map_x;
    std::int32_t hi_map_z;

    std::int32_t nx;
    std::int32_t ny;
    std::int32_t nz;

    std::int32_t h0;
    std::int32_t h1;
    std::int32_t h2;

    std::int32_t px;
    std::int32_t py;
    std::int32_t pz;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t dist;

    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;
    std::int32_t lradius;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    std::int32_t dprod;
    std::int32_t bright;

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;
    NIGHT_Colour *col_upto;

    if (WARE_in) {
        //
        // We only want to consider inside lights. The condition that the light
        // is on our floor is ignored if we are in a warehouse.
        //

        inside = true;
    }

    //
    // Allocate a 2d array on the scratchpad.
    //

    NIGHT_Preblock *precalc;

    ASSERT(HEAP_PAD_SIZE >= sizeof(NIGHT_Preblock) * PAP_BLOCKS);

    precalc = (NIGHT_Preblock *) HEAP_pad;

    //
    // Random lighting fluctuations over the map...
    //

    std::uint32_t seed = lo_map_x + (lo_map_z << 5);

    seed *= 328573;
    seed += 123456789;

    ASSERT(WITHIN(lo_map_x, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(lo_map_z, 0, PAP_SIZE_LO - 1));

    hi_map_x = lo_map_x << 2;
    hi_map_z = lo_map_z << 2;

    col_upto = colour;

    for (z = 0; z < PAP_BLOCKS; z++)
        for (x = 0; x < PAP_BLOCKS; x++) {
            mx = hi_map_x + x;
            mz = hi_map_z + z;

            //
            // Work out the normal at this point.
            //

            h0 = PAP_calc_height_at_point(mx, mz);
            h1 = PAP_calc_height_at_point(mx + 1, mz);
            h2 = PAP_calc_height_at_point(mx - 1, mz);

            nx = h2 - h0;
            nx -= h1 - h0;
            nx <<= 2;

            h1 = PAP_calc_height_at_point(mx, mz + 1);
            h2 = PAP_calc_height_at_point(mx, mz - 1);

            nz = h2 - h0;
            nz -= h1 - h0;
            nz <<= 2;

            if (abs(nx) > 120) {
                nx = (nx > 0) ? 100 : -100;
            }
            if (abs(nz) > 120) {
                nz = (nz > 0) ? 100 : -100;
            }

            ny = 256 - QDIST2(abs(nx), abs(nz));

            //
            // Store the info.
            //

            precalc[x][z].nx = nx;
            precalc[x][z].ny = ny;
            precalc[x][z].nz = nz;
            precalc[x][z].height = h0;

            //
            // Light the point from the ambient light.
            //

            dprod =
                NIGHT_amb_norm_x * nx +
                NIGHT_amb_norm_y * ny +
                NIGHT_amb_norm_z * nz;

            dprod = -dprod;

            SATURATE(dprod, 0, 65536);

            dprod >>= 9;
            dprod += 128;

            dprod *= NIGHT_LIGHT_MULTIPLIER;

            col_upto->red = NIGHT_amb_red * dprod >> 8;
            col_upto->green = NIGHT_amb_green * dprod >> 8;
            col_upto->blue = NIGHT_amb_blue * dprod >> 8;

            /*

            if (NIGHT_flag & NIGHT_FLAG_DARKEN_BUILDING_POINTS)
            {
                    //
                    // Darken points that lie on the edges of buildings.
                    //

                    for (dx = -1; dx <= 0; dx++)
                    for (dz = -1; dz <= 0; dz++)
                    {
                            px = mx + dx;
                            pz = mz + dz;

                            if (px >= 0 && pz >= 0)
                            {
                                    ASSERT(WITHIN(px, 0, PAP_SIZE_HI - 1));
                                    ASSERT(WITHIN(pz, 0, PAP_SIZE_HI - 1));

                                    if (PAP_2HI(px,pz).Flags & PAP_FLAG_HIDDEN)
                                    {
                                            #define NIGHT_DARKEN_GROUND 16

                                            if (col_upto->red   <= NIGHT_DARKEN_GROUND) {col_upto->red   = 0;} else {col_upto->red   -= NIGHT_DARKEN_GROUND;}
                                            if (col_upto->green <= NIGHT_DARKEN_GROUND) {col_upto->green = 0;} else {col_upto->green -= NIGHT_DARKEN_GROUND;}
                                            if (col_upto->blue  <= NIGHT_DARKEN_GROUND) {col_upto->blue  = 0;} else {col_upto->blue  -= NIGHT_DARKEN_GROUND;}

                                            goto darkened_this_point_once;
                                    }
                            }
                    }
            }

      darkened_this_point_once:;

            */

            col_upto += 1;
        }

    //
    // Light the map from the static lights and the lamposts.
    //

    mx1 = lo_map_x - 1;
    mz1 = lo_map_z - 1;

    mx2 = lo_map_x + 1;
    mz2 = lo_map_z + 1;

    if (mx1 < 0) {
        mx1 = 0;
    }
    if (mz1 < 0) {
        mz1 = 0;
    }

    if (mx2 >= PAP_SIZE_LO - 1) {
        mx2 = PAP_SIZE_LO - 1;
    }
    if (mz2 >= PAP_SIZE_LO - 1) {
        mz2 = PAP_SIZE_LO - 1;
    }

    for (mx = mx1; mx <= mx2; mx++)
        for (mz = mz1; mz <= mz2; mz++) {
            map_x = mx << PAP_SHIFT_LO;
            map_z = mz << PAP_SHIFT_LO;

            ASSERT(WITHIN(mx, 0, PAP_SIZE_LO - 1));
            ASSERT(WITHIN(mz, 0, PAP_SIZE_LO - 1));

            ns = &NIGHT_smap[mx][mz];

            for (i = 0; i < ns->number; i++) {
                ASSERT(WITHIN(ns->index + i, 0, NIGHT_slight_upto - 1));

                nsl = &NIGHT_slight[ns->index + i];

                //
                // If we are inside and light is inside, or we are outside and the light
                // is outside.
                //

                if (inside == (nsl->blue & 1)) {
                    lx = (nsl->x << 2) + map_x;
                    ly = nsl->y;
                    lz = (nsl->z << 2) + map_z;

                    lradius = nsl->radius << 2;

                    //
                    // if not inside or if inside then light has to be above floor level but
                    // low enough to reach floor
                    //

                    if (inside == 0 || WARE_in || (ly > floor_y && ly < floor_y + 256 + lradius)) {
                        //
                        // Work out the range of the map affected by the light
                        //

                        x1 = lx - lradius >> 8;
                        z1 = lz - lradius >> 8;

                        x2 = lx + lradius >> 8;
                        z2 = lz + lradius >> 8;

                        x1 -= hi_map_x;
                        z1 -= hi_map_z;

                        x2 -= hi_map_x;
                        z2 -= hi_map_z;

                        //
                        // If the light does not affect any of our map points,
                        // then ignore it.
                        //

                        if (x1 > PAP_BLOCKS - 1) {
                            continue;
                        }
                        if (z1 > PAP_BLOCKS - 1) {
                            continue;
                        }

                        if (x2 < 0) {
                            continue;
                        }
                        if (z2 < 0) {
                            continue;
                        }

                        //
                        // Clip to our map points.
                        //

                        if (x1 < 0) {
                            x1 = 0;
                        }
                        if (z1 < 0) {
                            z1 = 0;
                        }

                        if (x2 > PAP_BLOCKS - 1) {
                            x2 = PAP_BLOCKS - 1;
                        }
                        if (z2 > PAP_BLOCKS - 1) {
                            z2 = PAP_BLOCKS - 1;
                        }

                        //
                        // Light the affected region.
                        //

                        for (x = x1; x <= x2; x++)
                            for (z = z1; z <= z2; z++) {
                                //
                                // The position of this point.
                                //

                                px = x + hi_map_x << 8;
                                pz = z + hi_map_z << 8;

                                py = precalc[x][z].height;

                                //
                                // How far is this point from the light?
                                //

                                dx = lx - px;
                                dy = ly - py;
                                dz = lz - pz;

                                dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                                if (dist >= lradius) {
                                    //
                                    // Out of range.
                                    //
                                } else {
                                    //
                                    // The normal of this point.
                                    //

                                    nx = precalc[x][z].nx;
                                    ny = precalc[x][z].ny;
                                    nz = precalc[x][z].nz;

                                    dprod = dx * nx + dy * ny + dz * nx;

                                    if (dprod <= 0) {
                                        //
                                        // Facing away from the light.
                                        //
                                    } else {
                                        dprod /= dist;
                                        bright = 256 - (dist << 8) / lradius;
                                        bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                                        red = colour[x + z * PAP_BLOCKS].red;
                                        green = colour[x + z * PAP_BLOCKS].green;
                                        blue = colour[x + z * PAP_BLOCKS].blue;

                                        red += nsl->red * bright >> 8;
                                        green += nsl->green * bright >> 8;
                                        blue += nsl->blue * bright >> 8;

                                        SATURATE(red, 0, 255);
                                        SATURATE(green, 0, 255);
                                        SATURATE(blue, 0, 255);

                                        colour[x + z * PAP_BLOCKS].red = red;
                                        colour[x + z * PAP_BLOCKS].green = green;
                                        colour[x + z * PAP_BLOCKS].blue = blue;
                                    }
                                }
                            }
                    }
                }
            }
        }

    for (i = 0; i < NIGHT_llight_upto; i++) {
        //
        // There is a light under this prim... but where?
        //

        lx = NIGHT_llight[i].x;
        ly = NIGHT_llight[i].y;
        lz = NIGHT_llight[i].z;

        lradius = NIGHT_lampost_radius << 2;

        //
        // Work out the range of the map affected by the light
        //

        x1 = lx - lradius >> 8;
        z1 = lz - lradius >> 8;

        x2 = lx + lradius >> 8;
        z2 = lz + lradius >> 8;

        x1 -= hi_map_x;
        z1 -= hi_map_z;

        x2 -= hi_map_x;
        z2 -= hi_map_z;

        //
        // If the light does not affect any of our map points,
        // then ignore it.
        //

        if (x1 > PAP_BLOCKS - 1) {
            continue;
        }
        if (z1 > PAP_BLOCKS - 1) {
            continue;
        }

        if (x2 < 0) {
            continue;
        }
        if (z2 < 0) {
            continue;
        }

        //
        // Clip to our map points.
        //

        if (x1 < 0) {
            x1 = 0;
        }
        if (z1 < 0) {
            z1 = 0;
        }

        if (x2 > PAP_BLOCKS - 1) {
            x2 = PAP_BLOCKS - 1;
        }
        if (z2 > PAP_BLOCKS - 1) {
            z2 = PAP_BLOCKS - 1;
        }

        //
        // Light the affected region.
        //

        for (x = x1; x <= x2; x++)
            for (z = z1; z <= z2; z++) {
                //
                // The position of this point.
                //

                px = x + hi_map_x << 8;
                pz = z + hi_map_z << 8;

                py = precalc[x][z].height;

                //
                // How far is this point from the light?
                //

                dx = lx - px;
                dy = ly - py;
                dz = lz - pz;

                dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                if (dist >= lradius) {
                    //
                    // Out of range.
                    //
                } else {
                    //
                    // The normal of this point.
                    //

                    nx = precalc[x][z].nx;
                    ny = precalc[x][z].ny;
                    nz = precalc[x][z].nz;

                    dprod = dx * nx + dy * ny + dz * nx;

                    if (dprod <= 0) {
                        //
                        // Facing away from the light.
                        //
                    } else {
                        dprod /= dist;
                        bright = 256 - (dist << 8) / lradius;
                        bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                        red = colour[x + z * PAP_BLOCKS].red;
                        green = colour[x + z * PAP_BLOCKS].green;
                        blue = colour[x + z * PAP_BLOCKS].blue;

                        red += NIGHT_lampost_red * bright >> 8;
                        green += NIGHT_lampost_green * bright >> 8;
                        blue += NIGHT_lampost_blue * bright >> 8;

                        SATURATE(red, 0, 255);
                        SATURATE(green, 0, 255);
                        SATURATE(blue, 0, 255);

                        colour[x + z * PAP_BLOCKS].red = red;
                        colour[x + z * PAP_BLOCKS].green = green;
                        colour[x + z * PAP_BLOCKS].blue = blue;
                    }
                }
            }
    }

    if (lo_map_x == 0 || lo_map_x == PAP_SIZE_LO - 1 ||
        lo_map_z == 0 || lo_map_z == PAP_SIZE_LO - 1) {
        //
        // Fadeout this mapsquare towards the edge of the map.
        //

        col_upto = colour;

        for (z = 0; z < PAP_BLOCKS; z++)
            for (x = 0; x < PAP_BLOCKS; x++) {
                mx = hi_map_x + x;
                mz = hi_map_z + z;

                //
                // How far is this square from the edge of the map?
                //

                dx = (lo_map_x == 0) ? mx : ((PAP_SIZE_HI - 1) - mx);
                dz = (lo_map_z == 0) ? mz : ((PAP_SIZE_HI - 1) - mz);

                dist = std::min(dx, dz);

                col_upto->red = col_upto->red * dist >> 2;
                col_upto->green = col_upto->green * dist >> 2;
                col_upto->blue = col_upto->blue * dist >> 2;

                col_upto += 1;
            }
    }
}

//
// Gives back facet info.
//

void NIGHT_get_facet_info(
    std::uint16_t dfacet_index,
    std::int32_t *length,
    std::int32_t *height,
    std::int32_t *dx,
    std::int32_t *dy,
    std::int32_t *dz,
    std::uint32_t *flags) {
    DFacet *df;
    std::uint32_t flag = 0;

    ASSERT(WITHIN(dfacet_index, 1, next_dfacet - 1));

    df = &dfacets[dfacet_index];

    *dx = df->x[1] - df->x[0] << 8;
    *dz = df->z[1] - df->z[0] << 8;

    if (*dx && *dz) {
        //
        // We can't handle diagonal facets.
        //

        if (df->FacetType == STOREY_TYPE_NORMAL) {
            ASSERT(0);
        }
    }

    if (*dx) {
        *length = abs(*dx >> 8);
        *length += 1;

        *dx = (*dx > 0) ? +256 : -256;
        *dz = 0;
    } else {
        ASSERT(*dz);

        *length = abs(*dz >> 8);
        *length += 1;

        *dx = 0;
        *dz = (*dz > 0) ? +256 : -256;
    }

    if (df->FacetType == STOREY_TYPE_FENCE ||
        df->FacetType == STOREY_TYPE_FENCE_FLAT) {
        *height = 2;
        *dy = df->Height * 64;
    } else {
        *height = df->Height >> 2;
        *height += 1;
        *dy = 256;
    }
    if (df->FacetType == STOREY_TYPE_INSIDE ||
        df->FacetType == STOREY_TYPE_OINSIDE ||
        df->FacetType == STOREY_TYPE_INSIDE_DOOR)
        flag |= NIGHT_FLAG_INSIDE;

    *flags = flag;
}

//
// Lights the given prim.
//

typedef struct
{
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::int32_t nx;
    std::int32_t ny;
    std::int32_t nz;

} NIGHT_Point;

#define NIGHT_MAX_POINTS (HEAP_PAD_SIZE / sizeof(NIGHT_Point))

void NIGHT_light_prim(
    std::int32_t prim,
    std::int32_t prim_x,
    std::int32_t prim_y,
    std::int32_t prim_z,
    std::int32_t prim_yaw,
    std::int32_t prim_pitch,
    std::int32_t prim_roll,
    std::int32_t prim_inside,
    NIGHT_Colour *colour) {
    std::int32_t i;
    std::int32_t j;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t xmid;
    std::int32_t zmid;

    std::int32_t xmin, xmax;
    std::int32_t zmin, zmax;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t amb_x;
    std::int32_t amb_y;
    std::int32_t amb_z;

    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;
    std::int32_t lradius;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t dist;
    std::int32_t dprod;
    std::int32_t bright;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    std::int32_t num_points;

    std::int32_t point;
    std::int32_t matrix[9];
    std::int32_t rotate;

    PrimObject *p_obj = &prim_objects[prim];
    PrimInfo *p_info = get_prim_info(prim);

    NIGHT_Point *np;
    NIGHT_Point *np_base = (NIGHT_Point *) HEAP_pad;

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    //
    // Quite a few prims won't have any rotation to them... I expect.
    //

    if (prim_yaw == 0 &&
        prim_pitch == 0 &&
        prim_roll == 0) {
        rotate = false;
    } else {
        rotate = true;

        FMATRIX_calc(
            matrix,
            prim_yaw,
            prim_pitch,
            prim_roll);
    }

    //
    // Rotate the ambient light into the space of the prim.
    //

    amb_x = NIGHT_amb_norm_x;
    amb_y = NIGHT_amb_norm_y;
    amb_z = NIGHT_amb_norm_z;

    if (rotate) {
        FMATRIX_MUL(
            matrix,
            amb_x,
            amb_y,
            amb_z);
    }

    //
    // Initialise all the point colours to the ambient light.
    //

    num_points = p_obj->EndPoint - p_obj->StartPoint;

    for (i = 0; i < num_points; i++) {
        dprod =
            amb_x * prim_normal[p_obj->StartPoint + i].X +
            amb_y * prim_normal[p_obj->StartPoint + i].Y +
            amb_z * prim_normal[p_obj->StartPoint + i].Z;

        dprod >>= 9;
        dprod += 128;
        dprod *= NIGHT_LIGHT_MULTIPLIER;

        colour[i].red = NIGHT_amb_red * dprod >> 8;
        colour[i].green = NIGHT_amb_green * dprod >> 8;
        colour[i].blue = NIGHT_amb_blue * dprod >> 8;
    }

    //
    // The bounding box we search for lights effecting this prim.
    //

    xmid = prim_x >> PAP_SHIFT_LO;
    zmid = prim_z >> PAP_SHIFT_LO;

    ASSERT(WITHIN(xmid, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(zmid, 0, PAP_SIZE_LO - 1));

    xmin = xmid - 1;
    zmin = zmid - 1;

    xmax = xmid + 1;
    zmax = zmid + 1;

    if (xmin < 0) {
        xmin = 0;
    }
    if (zmin < 0) {
        zmin = 0;
    }

    if (xmax > PAP_SIZE_LO - 1) {
        xmax = PAP_SIZE_LO - 1;
    }
    if (zmax > PAP_SIZE_LO - 1) {
        zmax = PAP_SIZE_LO - 1;
    }

    for (mz = zmin; mz <= zmax; mz++) {
        for (mx = xmin; mx <= xmax; mx++) {
            ns = &NIGHT_smap[mx][mz];

            map_x = mx << PAP_SHIFT_LO;
            map_z = mz << PAP_SHIFT_LO;

            for (i = 0; i < ns->number; i++) {
                nsl = &NIGHT_slight[ns->index + i];
                //
                // make sure lights inside status is compatible with prims inside status
                //
                if ((prim_inside == 0 && (nsl->blue & 1) == 0) || (prim_inside && ((nsl->blue & 1) == 1))) {
                    //
                    // Rotate the light into the space of the prim.
                    //

                    lx = (nsl->x << 2) + map_x;
                    ly = nsl->y;
                    lz = (nsl->z << 2) + map_z;

                    lx -= prim_x;
                    ly -= prim_y;
                    lz -= prim_z;

                    if (rotate) {
                        FMATRIX_MUL(
                            matrix,
                            lx,
                            ly,
                            lz);
                    }

                    lradius = nsl->radius << 2;

                    //
                    // Is the light in range of the prim?
                    //

                    if (lx - lradius > p_info->maxx ||
                        lx + lradius < p_info->minx ||
                        ly - lradius > p_info->maxy ||
                        ly + lradius < p_info->miny ||
                        lz - lradius > p_info->maxz ||
                        lz + lradius < p_info->minz) {
                        //
                        // We can ignore the light.
                        //
                    } else {
                        for (j = 0; j < num_points; j++) {
                            point = p_obj->StartPoint + j;

                            dx = prim_points[point].X - lx;
                            dy = prim_points[point].Y - ly;
                            dz = prim_points[point].Z - lz;

                            dprod =
                                dx * prim_normal[point].X +
                                dy * prim_normal[point].Y +
                                dz * prim_normal[point].Z;

                            if (dprod <= 0) {
                                //
                                // The point is facing away from the light.
                                //
                            } else {
                                //
                                // Distance from the light.
                                //

                                dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                                if (dist < lradius) {
                                    dprod /= dist;
                                    bright = 256 - (dist << 8) / lradius;
                                    bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;
                                    ;

                                    red = colour[j].red;
                                    green = colour[j].green;
                                    blue = colour[j].blue;

                                    red += nsl->red * bright >> 8;
                                    green += nsl->green * bright >> 8;
                                    blue += nsl->blue * bright >> 8;

                                    SATURATE(red, 0, 255);
                                    SATURATE(green, 0, 255);
                                    SATURATE(blue, 0, 255);

                                    colour[j].red = red;
                                    colour[j].green = green;
                                    colour[j].blue = blue;
                                }
                            }
                        }
                    }
                }
            }
        }

        //
        // Dynamic lights?
        //
    }

    //
    // Lampost lights.
    //

    for (i = 0; i < NIGHT_llight_upto; i++) {
        //
        // Rotate the light into the space of the prim.
        //

        lx = NIGHT_llight[i].x;
        ly = NIGHT_llight[i].y;
        lz = NIGHT_llight[i].z;

        lx -= prim_x;
        ly -= prim_y;
        lz -= prim_z;

        if (rotate) {
            FMATRIX_MUL(
                matrix,
                lx,
                ly,
                lz);
        }

        lradius = NIGHT_lampost_radius << 2;

        //
        // Is the light in range of the prim?
        //

        if (lx - lradius > p_info->maxx ||
            lx + lradius < p_info->minx ||
            ly - lradius > p_info->maxy ||
            ly + lradius < p_info->miny ||
            lz - lradius > p_info->maxz ||
            lz + lradius < p_info->minz) {
            //
            // We can ignore the light.
            //
        } else {
            for (j = 0; j < num_points; j++) {
                point = p_obj->StartPoint + j;

                dx = prim_points[point].X - lx;
                dy = prim_points[point].Y - ly;
                dz = prim_points[point].Z - lz;

                dprod =
                    dx * prim_normal[point].X +
                    dy * prim_normal[point].Y +
                    dz * prim_normal[point].Z;

                if (dprod <= 0) {
                    //
                    // The point is facing away from the light.
                    //
                } else {
                    //
                    // Distance from the light.
                    //

                    dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                    if (dist < lradius) {
                        dprod /= dist;
                        bright = 256 - (dist << 8) / lradius;
                        bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                        red = colour[j].red;
                        green = colour[j].green;
                        blue = colour[j].blue;

                        red += NIGHT_lampost_red * bright >> 8;
                        green += NIGHT_lampost_green * bright >> 8;
                        blue += NIGHT_lampost_blue * bright >> 8;

                        SATURATE(red, 0, 255);
                        SATURATE(green, 0, 255);
                        SATURATE(blue, 0, 255);

                        colour[j].red = red;
                        colour[j].green = green;
                        colour[j].blue = blue;
                    }
                }
            }
        }
    }
}

// ========================================================
//
// DYNAMIC LIGHTS
//
// ========================================================

void NIGHT_dlight_init() {
    std::int32_t i;

    //
    // Create a linked list of free dlights.
    //

    memset(NIGHT_dlight, 0, sizeof(NIGHT_Dlight) * NIGHT_MAX_DLIGHTS);

    for (i = 1; i < NIGHT_MAX_DLIGHTS - 1; i++) {
        NIGHT_dlight[i].next = i + 1;
    }

    NIGHT_dlight[NIGHT_MAX_DLIGHTS - 1].next = NULL;
    NIGHT_dlight_free = 1;
    NIGHT_dlight_used = NULL;
}

std::uint8_t NIGHT_dlight_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::uint8_t radius,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue) {
    NIGHT_Dlight *ndl;

    std::uint8_t ans;

    if (NIGHT_dlight_free == NULL) {
        //
        // No more dlights available.
        //

        return NULL;
    }

    //
    // Take this light out of the free list.
    //

    ASSERT(WITHIN(NIGHT_dlight_free, 1, NIGHT_MAX_DLIGHTS - 1));

    ndl = &NIGHT_dlight[NIGHT_dlight_free];
    ans = NIGHT_dlight_free;
    NIGHT_dlight_free = ndl->next;

    //
    // Initialise the light.
    //

    ndl->x = x;
    ndl->y = y;
    ndl->z = z;
    ndl->radius = radius;
    ndl->red = red;
    ndl->green = green;
    ndl->blue = blue;
    ndl->next = NULL;
    ndl->flag = NIGHT_DLIGHT_FLAG_USED;

    //
    // Add to the used list.
    //

    ndl->next = NIGHT_dlight_used;
    NIGHT_dlight_used = ans;

    return ans;
}

void NIGHT_dlight_destroy(std::uint8_t dlight_index) {
    std::int32_t map;
    std::uint8_t next;
    std::uint8_t *prev;

    NIGHT_Dlight *ndl;

    ASSERT(WITHIN(dlight_index, 1, NIGHT_MAX_DLIGHTS - 1));

    //
    // Take this light out of the used list. I hate doubley linked
    // lists.  I'd rather do this!
    //

    next = NIGHT_dlight_used;
    prev = &NIGHT_dlight_used;

    while (1) {
        if (next == NULL) {
            //
            // The light wasn't in the linked list!
            //

            ASSERT(0);
        }

        ASSERT(WITHIN(next, 1, NIGHT_MAX_DLIGHTS - 1));

        ndl = &NIGHT_dlight[next];

        if (next == dlight_index) {
            //
            // This is the light to take out of the list.
            //

            *prev = ndl->next;

            break;
        }

        prev = &ndl->next;
        next = ndl->next;
    }

    //
    // Add the light to the free list.
    //

    ndl->flag = 0;
    ndl->next = NIGHT_dlight_free;
    NIGHT_dlight_free = dlight_index;
}

void NIGHT_dlight_move(std::uint8_t dlight_index, std::int32_t x, std::int32_t y, std::int32_t z) {
    NIGHT_Dlight *ndl;

    ASSERT(WITHIN(dlight_index, 1, NIGHT_MAX_DLIGHTS - 1));

    ndl = &NIGHT_dlight[dlight_index];

    ndl->x = x;
    ndl->y = y;
    ndl->z = z;
}

void NIGHT_dlight_colour(std::uint8_t dlight_index, std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    NIGHT_Dlight *ndl;

    ASSERT(WITHIN(dlight_index, 1, NIGHT_MAX_DLIGHTS - 1));

    ndl = &NIGHT_dlight[dlight_index];

    ndl->red = red;
    ndl->green = green;
    ndl->blue = blue;
}

//
// Either adds or subtracts dynamic light to the cached lighting.
//

std::uint8_t NIGHT_dfcache_counter;

void NIGHT_dlight_squares_do(std::int32_t subtract) {
    std::int32_t i;
    std::int32_t j;
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;
    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t px;
    std::int32_t py;
    std::int32_t pz;
    std::int32_t mx;
    std::int32_t mz;
    std::int32_t dfx;
    std::int32_t dfy;
    std::int32_t dfz;
    std::int32_t mx1;
    std::int32_t mz1;
    std::int32_t mx2;
    std::int32_t mz2;
    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;
    std::int32_t next;
    std::int32_t dist;
    std::int32_t bright;
    std::int32_t lradius;
    std::int32_t flist;
    std::int32_t exit;
    std::int32_t facet;
    std::int32_t dfcache;
    std::int32_t length;
    std::int32_t height;
    std::int32_t hi_map_x;
    std::int32_t hi_map_z;

    NIGHT_Dlight *ndl;
    NIGHT_Square *nq;
    DFacet *df;
    NIGHT_Dfcache *ndf;
    NIGHT_Colour *nc;
    std::uint32_t flags;

    //
    // Go through the used list of dynamic lights.
    //

    for (next = NIGHT_dlight_used; next; next = ndl->next) {
        ASSERT(WITHIN(next, 1, NIGHT_MAX_DLIGHTS - 1));

        ndl = &NIGHT_dlight[next];

        //
        // Mark all cached dfacets as unlit from this light.
        //

        NIGHT_dfcache_counter += 1;

        if (NIGHT_dfcache_counter == 0) {
            NIGHT_dfcache_counter = 1;

            for (dfcache = NIGHT_dfcache_used; dfcache; dfcache = NIGHT_dfcache[dfcache].next) {
                ASSERT(WITHIN(dfcache, 1, NIGHT_MAX_DFCACHES - 1));

                NIGHT_dfcache[dfcache].counter = 0;
            }
        }

        //
        // Light the ground and facets from this light.
        //

        lradius = ndl->radius << 2;
        mx1 = ndl->x - lradius >> PAP_SHIFT_LO;
        mz1 = ndl->z - lradius >> PAP_SHIFT_LO;
        mx2 = ndl->x + lradius >> PAP_SHIFT_LO;
        mz2 = ndl->z + lradius >> PAP_SHIFT_LO;

        SATURATE(mx1, 0, PAP_SIZE_LO - 1);
        SATURATE(mz1, 0, PAP_SIZE_LO - 1);
        SATURATE(mx2, 0, PAP_SIZE_LO - 1);
        SATURATE(mz2, 0, PAP_SIZE_LO - 1);

        for (mx = mx1; mx <= mx2; mx++)
            for (mz = mz1; mz <= mz2; mz++) {
                //
                // Does this mapsquare have cached lighting?
                //

                if (NIGHT_cache[mx][mz]) {
                    ASSERT(WITHIN(NIGHT_cache[mx][mz], 1, NIGHT_MAX_SQUARES - 1));

                    nq = &NIGHT_square[NIGHT_cache[mx][mz]];

                    hi_map_x = mx << 2;
                    hi_map_z = mz << 2;

                    //
                    // Light up this square.
                    //

                    x1 = ndl->x - lradius >> 8;
                    z1 = ndl->z - lradius >> 8;

                    x2 = ndl->x + lradius >> 8;
                    z2 = ndl->z + lradius >> 8;

                    x1 -= hi_map_x;
                    z1 -= hi_map_z;

                    x2 -= hi_map_x;
                    z2 -= hi_map_z;

                    if (x1 < 0) {
                        x1 = 0;
                    }
                    if (z1 < 0) {
                        z1 = 0;
                    }

                    if (x2 > PAP_BLOCKS - 1) {
                        x2 = PAP_BLOCKS - 1;
                    }
                    if (z2 > PAP_BLOCKS - 1) {
                        z2 = PAP_BLOCKS - 1;
                    }

                    for (x = x1; x <= x2; x++)
                        for (z = z1; z <= z2; z++) {
                            px = x + hi_map_x << 8;
                            pz = z + hi_map_z << 8;
                            py = PAP_calc_height_at_point(x + hi_map_x, z + hi_map_z);

                            //
                            // How far is this point from the light?
                            //

                            dx = ndl->x - px;
                            dy = ndl->y - py;
                            dz = ndl->z - pz;

                            dist = QDIST3(abs(dx), abs(dy), abs(dz));

                            if (dist >= lradius) {
                                //
                                // Out of range.
                                //
                            } else {
                                //
                                // Light proportional to distance.
                                //

                                bright = (256 - (dist << 8) / lradius) * NIGHT_LIGHT_MULTIPLIER;

                                SATURATE(bright, 0, 256);

                                red = nq->colour[x + z * PAP_BLOCKS].red;
                                green = nq->colour[x + z * PAP_BLOCKS].green;
                                blue = nq->colour[x + z * PAP_BLOCKS].blue;

                                if (subtract) {
                                    red -= ndl->red * bright >> 8;
                                    green -= ndl->green * bright >> 8;
                                    blue -= ndl->blue * bright >> 8;
                                } else {
                                    red += ndl->red * bright >> 8;
                                    green += ndl->green * bright >> 8;
                                    blue += ndl->blue * bright >> 8;
                                }

                                /*

                                ASSERT(WITHIN(red,   0, 255));
                                ASSERT(WITHIN(green, 0, 255));
                                ASSERT(WITHIN(blue,  0, 255));

                                */

                                SATURATE(red, 0, 255);
                                SATURATE(green, 0, 255);
                                SATURATE(blue, 0, 255);

                                nq->colour[x + z * PAP_BLOCKS].red = red;
                                nq->colour[x + z * PAP_BLOCKS].green = green;
                                nq->colour[x + z * PAP_BLOCKS].blue = blue;
                            }
                        }

                    //
                    // Look for facets on this square that need lighting.
                    //

                    flist = PAP_2LO(mx, mz).ColVectHead;
                    exit = false;
                    if (flist) {
                        while (!exit) {
                            facet = facet_links[flist++];

                            if (facet < 0) {
                                //
                                // This is the last facet.
                                //

                                facet = -facet;
                                exit = true;
                            }

                            ASSERT(WITHIN(facet, 1, next_dfacet - 1));

                            df = &dfacets[facet];

                            if (df->Dfcache) {
                                ASSERT(WITHIN(df->Dfcache, 1, NIGHT_MAX_DFCACHES - 1));

                                //
                                // This facet has cached lighting- have we already lit it?
                                //

                                ndf = &NIGHT_dfcache[df->Dfcache];

                                if (ndf->counter == NIGHT_dfcache_counter) {
                                    //
                                    // We have already lit this facet.
                                    //
                                } else {
                                    //
                                    // Light this facet all the time- ignore the normal of the facet.
                                    //

                                    df->FacetFlags |= FACET_FLAG_DLIT;

                                    NIGHT_get_facet_info(
                                        facet,
                                        &length,
                                        &height,
                                        &dfx,
                                        &dfy,
                                        &dfz,
                                        &flags);

                                    x = df->x[0] << 8;
                                    y = df->Y[0];
                                    z = df->z[0] << 8;

                                    nc = ndf->colour;

                                    for (i = 0; i < height; i++) {
                                        px = x;
                                        py = y;
                                        pz = z;

                                        for (j = 0; j < length; j++) {
                                            dx = ndl->x - px;
                                            dy = ndl->y - py;
                                            dz = ndl->z - pz;

                                            dist = QDIST3(abs(dx), abs(dy), abs(dz));

                                            if (dist >= lradius) {
                                                //
                                                // Out of range.
                                                //
                                            } else {
                                                //
                                                // Light proportional to distance.
                                                //

                                                bright = (256 - (dist << 8) / (lradius)) * NIGHT_LIGHT_MULTIPLIER;

                                                SATURATE(bright, 0, 256);

                                                red = nc->red;
                                                green = nc->green;
                                                blue = nc->blue;

                                                if (subtract) {
                                                    red -= ndl->red * bright >> 8;
                                                    green -= ndl->green * bright >> 8;
                                                    blue -= ndl->blue * bright >> 8;
                                                } else {
                                                    red += ndl->red * bright >> 8;
                                                    green += ndl->green * bright >> 8;
                                                    blue += ndl->blue * bright >> 8;
                                                }

                                                SATURATE(red, 0, 255);
                                                SATURATE(green, 0, 255);
                                                SATURATE(blue, 0, 255);

                                                nc->red = red;
                                                nc->green = green;
                                                nc->blue = blue;
                                            }

                                            px += dfx;
                                            pz += dfz;
                                            nc += 1;
                                        }

                                        y += dfy;
                                    }

                                    //
                                    // Mark this facet as already lit.
                                    //

                                    ndf->counter = NIGHT_dfcache_counter;
                                }
                            }
                        }
                    }
                }
            }
    }
}

void NIGHT_dlight_squares_up() {
    NIGHT_dlight_squares_do(false);
}

void NIGHT_dlight_squares_down() {
    std::int32_t next;

    NIGHT_Dlight *ndl;

    NIGHT_dlight_squares_do(true);

destroy_again:;

    for (next = NIGHT_dlight_used; next; next = ndl->next) {
        ASSERT(WITHIN(next, 1, NIGHT_MAX_DLIGHTS - 1));

        ndl = &NIGHT_dlight[next];

        if (ndl->flag & NIGHT_DLIGHT_FLAG_REMOVE) {
            NIGHT_dlight_destroy(next);

            goto destroy_again;
        }
    }
}

// ========================================================
//
// CACHED LIGHTING OF THE MAP AND PRIMS
//
// ========================================================

void NIGHT_cache_init() {
    std::int32_t i;

    //
    // Build the free list of cache squares.
    //

    NIGHT_square_free = 1;
    NIGHT_square_num_used = 0;

    for (i = 1; i < NIGHT_MAX_SQUARES - 1; i++) {
        NIGHT_square[i].next = i + 1;
        NIGHT_square[i].flag = 0;
    }

    NIGHT_square[NIGHT_MAX_SQUARES - 1].next = 0;

    //
    // Clear the cache entries.
    //

    memset((std::uint8_t *) NIGHT_cache, 0, sizeof(NIGHT_cache));
}

void NIGHT_cache_recalc() {
    std::int32_t i;

    std::int32_t lo_map_x;
    std::int32_t lo_map_z;

    std::int32_t ware;

    for (i = 1; i < NIGHT_MAX_SQUARES; i++) {
        if (NIGHT_square[i].flag & NIGHT_SQUARE_FLAG_USED) {
            ware = NIGHT_square[i].flag & NIGHT_SQUARE_FLAG_USED;

            lo_map_x = NIGHT_square[i].lo_map_x;
            lo_map_z = NIGHT_square[i].lo_map_z;

            NIGHT_cache_destroy(i);
            NIGHT_cache_create(lo_map_x, lo_map_z, ware);
        }
    }
}

std::int32_t NIGHT_old_amb_red;
std::int32_t NIGHT_old_amb_green;
std::int32_t NIGHT_old_amb_blue;
std::int32_t NIGHT_old_amb_norm_x;
std::int32_t NIGHT_old_amb_norm_y;
std::int32_t NIGHT_old_amb_norm_z;

void NIGHT_cache_create(std::uint8_t lo_map_x, std::uint8_t lo_map_z, std::uint8_t ware) {
    std::int32_t num_points;
    std::int32_t memory;
    std::int32_t square;
    std::int32_t vector[3];

    OB_Info *ofound;
    OB_Info *oi;
    NIGHT_Square *nq;
    NIGHT_Colour *nc;

    PrimObject *p_obj;

    ASSERT(WITHIN(lo_map_x, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(lo_map_z, 0, PAP_SIZE_LO - 1));

    if (ware) {
        //
        // Change the ambient light.
        //

        NIGHT_old_amb_red = NIGHT_amb_red;
        NIGHT_old_amb_green = NIGHT_amb_green;
        NIGHT_old_amb_blue = NIGHT_amb_blue;

        NIGHT_old_amb_norm_x = NIGHT_amb_norm_x;
        NIGHT_old_amb_norm_y = NIGHT_amb_norm_y;
        NIGHT_old_amb_norm_z = NIGHT_amb_norm_z;

        NIGHT_amb_red = 25;
        NIGHT_amb_green = 25;
        NIGHT_amb_blue = 25;

        NIGHT_amb_norm_x = 0;
        NIGHT_amb_norm_y = -255;
        NIGHT_amb_norm_z = 0;
    }

    //
    // Find all the lampost lights shining on this mapsquare.
    //

    NIGHT_llight_upto = 0;

    if (NIGHT_flag & NIGHT_FLAG_LIGHTS_UNDER_LAMPOSTS) {
        std::int32_t mx;
        std::int32_t mz;

        for (mx = lo_map_x - 1; mx <= lo_map_x + 1; mx++)
            for (mz = lo_map_z - 1; mz <= lo_map_z + 1; mz++) {
                if (WITHIN(mx, 0, PAP_SIZE_LO - 1) &&
                    WITHIN(mz, 0, PAP_SIZE_LO - 1)) {
                    for (oi = OB_find(mx, mz); oi->prim; oi++) {
                        if (prim_objects[oi->prim].flag & PRIM_FLAG_LAMPOST) {
                            //
                            // Found a lampost.
                            //

                            if (WITHIN(NIGHT_llight_upto, 0, NIGHT_MAX_LLIGHTS - 1)) {
                                FMATRIX_vector(
                                    vector,
                                    oi->yaw,
                                    oi->pitch);

                                NIGHT_llight[NIGHT_llight_upto].x = (-vector[0] >> 9) + oi->x;
                                NIGHT_llight[NIGHT_llight_upto].y = (-vector[1] >> 9) + oi->y + 0x100;
                                NIGHT_llight[NIGHT_llight_upto].z = (-vector[2] >> 9) + oi->z;

                                NIGHT_llight_upto += 1;
                            }
                        }
                    }
                }
            }
    }

    //
    // Get a spare cache square.
    //

    ASSERT(WITHIN(NIGHT_square_free, 1, NIGHT_MAX_SQUARES - 1));

    square = NIGHT_square_free;
    nq = &NIGHT_square[square];
    NIGHT_square_free = nq->next;

    //
    // Initialise the square.
    //

    nq->next = NULL;
    nq->flag = NIGHT_SQUARE_FLAG_USED;
    nq->lo_map_x = lo_map_x;
    nq->lo_map_z = lo_map_z;

    if (ware) {
        nq->flag |= NIGHT_SQUARE_FLAG_WARE;
    }

    //
    // How much memory do we need?
    //

    memory = PAP_BLOCKS * PAP_BLOCKS * sizeof(NIGHT_Colour);

    for (oi = OB_find(lo_map_x, lo_map_z); oi->prim; oi++) {
        if ((ware && !(oi->flags & OB_FLAG_WAREHOUSE)) ||
            (!ware && (oi->flags & OB_FLAG_WAREHOUSE))) {
            //
            // Not the correct warehouse state.
            //

            continue;
        }

        p_obj = &prim_objects[oi->prim];

        num_points = p_obj->EndPoint - p_obj->StartPoint;
        memory += num_points * sizeof(NIGHT_Colour);
    }

    //
    // Try to get the memory.
    //

    nq->colour = (NIGHT_Colour *) HEAP_get(memory);
    nq->sizeof_colour = memory;

    ASSERT(nq->colour);

    if (nq->colour == NULL)
        return;

    //
    // Light the mapsquare.
    //

    NIGHT_light_mapsquare(lo_map_x, lo_map_z, nq->colour, 0, 0);

    //
    // Light all the prims.
    //

    std::uint8_t debug1 = (((std::uint8_t *) (nq->colour)) + memory)[0];
    std::uint8_t debug2 = (((std::uint8_t *) (nq->colour)) + memory)[1];
    std::uint8_t debug3 = (((std::uint8_t *) (nq->colour)) + memory)[2];
    std::uint8_t debug4 = (((std::uint8_t *) (nq->colour)) + memory)[3];

    nc = nq->colour + (PAP_BLOCKS * PAP_BLOCKS);

    for (oi = OB_find(lo_map_x, lo_map_z); oi->prim; oi++) {
        if ((ware && !(oi->flags & OB_FLAG_WAREHOUSE)) ||
            (!ware && (oi->flags & OB_FLAG_WAREHOUSE))) {
            //
            // Not the correct warehouse state.
            //

            continue;
        }

        p_obj = &prim_objects[oi->prim];

        num_points = p_obj->EndPoint - p_obj->StartPoint;

        NIGHT_light_prim(
            oi->prim,
            oi->x,
            oi->y,
            oi->z,
            oi->yaw,
            oi->pitch,
            oi->roll,
            0,
            nc);

        nc += num_points;
    }

    ASSERT(debug1 == (((std::uint8_t *) (nq->colour)) + memory)[0]);
    ASSERT(debug2 == (((std::uint8_t *) (nq->colour)) + memory)[1]);
    ASSERT(debug3 == (((std::uint8_t *) (nq->colour)) + memory)[2]);
    ASSERT(debug4 == (((std::uint8_t *) (nq->colour)) + memory)[3]);

    //
    // Link this square to the cache mapwho.
    //

    NIGHT_cache[lo_map_x][lo_map_z] = square;

    NIGHT_square_num_used += 1;

    if (ware) {
        //
        // Restore the ambient light for the outside.
        //

        NIGHT_amb_red = NIGHT_old_amb_red;
        NIGHT_amb_green = NIGHT_old_amb_green;
        NIGHT_amb_blue = NIGHT_old_amb_blue;

        NIGHT_amb_norm_x = NIGHT_old_amb_norm_x;
        NIGHT_amb_norm_y = NIGHT_old_amb_norm_y;
        NIGHT_amb_norm_z = NIGHT_old_amb_norm_z;
    }
}

void NIGHT_cache_create_inside(std::uint8_t lo_map_x, std::uint8_t lo_map_z, std::int32_t floor_y) {
    std::int32_t num_points;
    std::int32_t memory;
    std::int32_t square;
    std::int32_t vector[3];

    OB_Info *ofound;
    OB_Info *oi;
    NIGHT_Square *nq;
    NIGHT_Colour *nc;

    PrimObject *p_obj;

    ASSERT(WITHIN(lo_map_x, 0, PAP_SIZE_LO - 1));
    ASSERT(WITHIN(lo_map_z, 0, PAP_SIZE_LO - 1));

    //
    // Get a spare cache square.
    //

    ASSERT(WITHIN(NIGHT_square_free, 1, NIGHT_MAX_SQUARES - 1));

    square = NIGHT_square_free;
    nq = &NIGHT_square[square];
    NIGHT_square_free = nq->next;

    //
    // Initialise the square.
    //

    nq->next = NULL;
    nq->flag = NIGHT_SQUARE_FLAG_USED;
    nq->lo_map_x = lo_map_x;
    nq->lo_map_z = lo_map_z;

    //
    // How much memory do we need?
    //

    memory = PAP_BLOCKS * PAP_BLOCKS * sizeof(NIGHT_Colour);

    for (oi = OB_find_inside(lo_map_x, lo_map_z, INDOORS_INDEX); oi->prim; oi++) {
        p_obj = &prim_objects[oi->prim];

        num_points = p_obj->EndPoint - p_obj->StartPoint;
        memory += num_points * sizeof(NIGHT_Colour);
    }

    //
    // Try to get the memory.
    //

    nq->colour = (NIGHT_Colour *) HEAP_get(memory);
    nq->sizeof_colour = memory;

    ASSERT(nq->colour);

    //
    // Light the mapsquare.
    //

    NIGHT_light_mapsquare(lo_map_x, lo_map_z, nq->colour, floor_y, 1);

    //
    // Light all the prims.
    //

    std::uint8_t debug1 = (((std::uint8_t *) (nq->colour)) + memory)[0];
    std::uint8_t debug2 = (((std::uint8_t *) (nq->colour)) + memory)[1];
    std::uint8_t debug3 = (((std::uint8_t *) (nq->colour)) + memory)[2];
    std::uint8_t debug4 = (((std::uint8_t *) (nq->colour)) + memory)[3];

    nc = nq->colour + (PAP_BLOCKS * PAP_BLOCKS);

    for (oi = OB_find_inside(lo_map_x, lo_map_z, INDOORS_INDEX); oi->prim; oi++) {
        p_obj = &prim_objects[oi->prim];

        num_points = p_obj->EndPoint - p_obj->StartPoint;

        NIGHT_light_prim(
            oi->prim,
            oi->x,
            oi->y,
            oi->z,
            oi->yaw,
            oi->pitch,
            oi->roll,
            oi->InsideIndex,
            nc);

        nc += num_points;
    }

    ASSERT(debug1 == (((std::uint8_t *) (nq->colour)) + memory)[0]);
    ASSERT(debug2 == (((std::uint8_t *) (nq->colour)) + memory)[1]);
    ASSERT(debug3 == (((std::uint8_t *) (nq->colour)) + memory)[2]);
    ASSERT(debug4 == (((std::uint8_t *) (nq->colour)) + memory)[3]);

    //
    // Link this square to the cache mapwho.
    //

    NIGHT_cache[lo_map_x][lo_map_z] = square;

    NIGHT_square_num_used += 1;
}

void NIGHT_cache_destroy(std::uint8_t square_index) {
    NIGHT_Square *nq;

    ASSERT(WITHIN(square_index, 0, NIGHT_MAX_SQUARES - 1));

    nq = &NIGHT_square[square_index];

    //
    // Make sure this square is in use.
    //

    ASSERT(nq->flag & NIGHT_SQUARE_FLAG_USED);

    //
    // Return the memory.
    //

    HEAP_give(nq->colour, nq->sizeof_colour);

    //
    // Insert into the free list.
    //

    nq->next = NIGHT_square_free;
    NIGHT_square_free = square_index;

    //
    // Clear the cache mapwho.
    //

    NIGHT_cache[nq->lo_map_x][nq->lo_map_z] = NULL;

    //
    // Mark as unused.
    //

    nq->flag = 0;

    NIGHT_square_num_used -= 1;
}

// ========================================================
//
// CACHED LIGHTING OF DFACETS ON THE SUPERMAP
//
// ========================================================

void NIGHT_dfcache_init() {
    std::int32_t i;

    //
    // Build the free list of dfcache squares.
    //

    memset(NIGHT_dfcache, 0, sizeof(NIGHT_dfcache));

    NIGHT_dfcache_free = 1;
    NIGHT_dfcache_used = 0;

    for (i = 1; i < NIGHT_MAX_DFCACHES - 1; i++) {
        NIGHT_dfcache[i].next = i + 1;
    }

    NIGHT_dfcache[NIGHT_MAX_DFCACHES - 1].next = 0;

    //
    // Clear all the cached lighting from the DFacets.
    //

    for (i = 1; i < next_dfacet; i++) {
        dfacets[i].Dfcache = 0;
    }
}

void NIGHT_dfcache_recalc() {
    std::int32_t dfcache;
    std::int32_t next;
    std::int32_t dfacet;

    for (dfcache = NIGHT_dfcache_used; dfcache; dfcache = next) {
        ASSERT(WITHIN(dfcache, 1, NIGHT_MAX_DFCACHES - 1));

        dfacet = NIGHT_dfcache[dfcache].dfacet;
        next = NIGHT_dfcache[dfcache].next;

        ASSERT(WITHIN(dfacet, 1, next_dfacet - 1));

        NIGHT_dfcache_destroy(dfcache);

        dfacets[dfacet].Dfcache = NIGHT_dfcache_create(dfacet);
    }
}

std::uint8_t NIGHT_dfcache_create(std::uint16_t dfacet_index) {
    std::int32_t i;
    std::int32_t j;
    std::int32_t k;

    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t nx;
    std::int32_t ny;
    std::int32_t nz;

    std::int32_t x;
    std::int32_t y;
    std::int32_t z;

    std::int32_t dsx1;
    std::int32_t dsy1;
    std::int32_t dsz1;

    std::int32_t dsx2;
    std::int32_t dsy2;
    std::int32_t dsz2;

    std::int32_t dlx;
    std::int32_t dly;
    std::int32_t dlz;

#ifndef NDEBUG
    void *min_address;
    void *max_address;
#endif

    std::int32_t dprod;
    std::int32_t length;
    std::int32_t height;
    std::int32_t num_points;
    std::int32_t num_faces;
    std::int32_t num_bytes;

    std::int32_t base_red;
    std::int32_t base_green;
    std::int32_t base_blue;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    std::int32_t dist;
    std::int32_t bright;

    std::uint8_t dfcache_index;
    std::uint8_t darken;

    NIGHT_Dfcache *nd;
    NIGHT_Colour *nc;
    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    std::uint8_t inside = 0;
    std::uint32_t flags;

#define NIGHT_MAX_SLIGHTS_PER_FACET 16

    struct
    {
        std::uint8_t index;
        std::uint8_t padding;
        std::uint16_t x;
        std::uint16_t z;

    } slight[16];
    std::int32_t slight_upto = 0;

    DFacet *df;

    ASSERT(WITHIN(dfacet_index, 1, next_dfacet - 1));

    df = &dfacets[dfacet_index];

    //
    // Calculate facet info.
    //

    NIGHT_get_facet_info(
        dfacet_index,
        &length,
        &height,
        &dx,
        &dy,
        &dz,
        &flags);

    ASSERT(height >= 2);

    num_points = length * height;
    num_faces = num_points - length - height + 1;

    //
    // Get a new cache element.
    //

    if (!WITHIN(NIGHT_dfcache_free, 1, NIGHT_MAX_DFCACHES - 1)) {
        //
        // No new cache elements available!
        //

        return NULL;
    }

    dfcache_index = NIGHT_dfcache_free;
    nd = &NIGHT_dfcache[dfcache_index];
    NIGHT_dfcache_free = nd->next;

    nd->next = NULL;
    nd->dfacet = dfacet_index;

    //
    // Add it to the used list.
    //

    nd->next = NIGHT_dfcache_used;
    NIGHT_dfcache_used = dfcache_index;

    //
    // Allocate memory. How many bytes to we need?
    //

    num_bytes = num_points * sizeof(NIGHT_Colour);
    num_bytes += num_faces + 3 >> 2;

    nd->colour = (NIGHT_Colour *) HEAP_get(num_bytes);
    nd->sizeof_colour = num_bytes;

    ASSERT(nd->colour != NULL);

#ifndef NDEBUG
    min_address = nd->colour;
    max_address = ((std::uint8_t *) nd->colour) + (num_bytes - 1);
#endif

    //
    // The normal of the facet
    //

    nx = -dz;
    ny = 0;
    nz = dx;

    dprod =
        NIGHT_amb_norm_x * nx +
            NIGHT_amb_norm_z * nz >>
        8;

    if (WARE_in) {
        //
        // Inside a warehouse we don't want directional light but we still want a
        // slight variation in hue.
        //

        dprod >>= 3;
        dprod += 128 + 64 + 32;

        //
        // The walls of a warehouse are seen from the other side.
        //

        nx = -nx;
        nz = -nz;
    } else {
        dprod >>= 2;
        dprod += 192;
    }

    base_red = NIGHT_amb_red * dprod >> 8;
    base_green = NIGHT_amb_green * dprod >> 8;
    base_blue = NIGHT_amb_blue * dprod >> 8;

    //
    // Find all the static lights effecting this facet.
    //

    {
        std::int32_t mx;
        std::int32_t mz;
        std::int32_t mx1;
        std::int32_t mz1;
        std::int32_t mx2;
        std::int32_t mz2;
        std::int32_t map_x;
        std::int32_t map_z;

        mx1 = df->x[0] << 8;
        mx2 = df->x[1] << 8;

        mz1 = df->z[0] << 8;
        mz2 = df->z[1] << 8;

        if (mx1 > mx2) {
            SWAP(mx1, mx2);
        }
        if (mz1 > mz2) {
            SWAP(mz1, mz2);
        }

        mx1 -= 0x400;
        mz1 -= 0x400;

        mx2 += 0x400;
        mz2 += 0x400;

        mx1 >>= PAP_SHIFT_LO;
        mz1 >>= PAP_SHIFT_LO;
        mx2 >>= PAP_SHIFT_LO;
        mz2 >>= PAP_SHIFT_LO;

        SATURATE(mx1, 0, PAP_SIZE_LO - 1);
        SATURATE(mz1, 0, PAP_SIZE_LO - 1);
        SATURATE(mx2, 0, PAP_SIZE_LO - 1);
        SATURATE(mz2, 0, PAP_SIZE_LO - 1);

        for (mx = mx1; mx <= mx2; mx++)
            for (mz = mz1; mz <= mz2; mz++) {
                map_x = mx << PAP_SHIFT_LO;
                map_z = mz << PAP_SHIFT_LO;

                ASSERT(WITHIN(mx, 0, PAP_SIZE_LO - 1));
                ASSERT(WITHIN(mz, 0, PAP_SIZE_LO - 1));

                ns = &NIGHT_smap[mx][mz];

                for (i = 0; i < ns->number; i++) {
                    ASSERT(WITHIN(ns->index + i, 0, NIGHT_slight_upto - 1));

                    nsl = &NIGHT_slight[ns->index + i];

                    lx = (nsl->x << 2) + map_x;
                    lz = (nsl->z << 2) + map_z;

                    //
                    // Is the facet facing away from the light?
                    //

                    dlx = (df->x[0] << 8) - lx;
                    dlz = (df->z[0] << 8) - lz;

                    dprod = dlx * nx + dlz * nz;

                    if (dprod > 0) {
                        //
                        // This light might light our facet up.
                        //

                        if (WITHIN(slight_upto, 0, NIGHT_MAX_SLIGHTS_PER_FACET - 1)) {
                            slight[slight_upto].index = ns->index + i;
                            slight[slight_upto].x = lx;
                            slight[slight_upto].z = lz;

                            slight_upto += 1;
                        }
                    }
                }
            }
    }

    //
    // Now light the points.
    //

    lx = df->x[0] << 8;
    ly = df->Y[0];
    lz = df->z[0] << 8;

    nc = nd->colour;

    for (i = 0; i < height; i++) {
        x = lx;
        y = ly;
        z = lz;

        darken = (NIGHT_flag & NIGHT_FLAG_DARKEN_BUILDING_POINTS) && (ly == PAP_calc_height_at_point(lx >> 8, ly >> 8));

        for (j = 0; j < length; j++) {
            red = base_red;
            green = base_green;
            blue = base_blue;

            if (darken) {
#define NIGHT_DARKEN_FACET 16

                red -= NIGHT_DARKEN_FACET;
                green -= NIGHT_DARKEN_FACET;
                blue -= NIGHT_DARKEN_FACET;
            }

            //
            // Static lighting.
            //

            for (k = 0; k < slight_upto; k++) {
                ASSERT(WITHIN(slight[k].index, 0, NIGHT_slight_upto - 1));

                nsl = &NIGHT_slight[slight[k].index];

                //
                // Only consider inside lights for inside facets or when we are
                // lighting warehouses.  Ignore outside lights when in a warehouse.
                //

                if ((WARE_in && (nsl->blue & 1)) || ((flags & NIGHT_FLAG_INSIDE) == (unsigned) (nsl->blue & 1))) {
                    dlx = x - slight[k].x;
                    dlz = z - slight[k].z;
                    dly = y - nsl->y;

                    dist = QDIST3(abs(dlx), abs(dly), abs(dlz)) + 1;

                    if (dist < (nsl->radius << 2)) {
                        dprod = dlx * nx + dlz * nz;

                        ASSERT(dprod >= 0); // We only consider lights facing the facet.

                        dprod /= dist;
                        bright = 256 - (dist << 8) / (nsl->radius << 2);
                        bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                        red += nsl->red * bright >> 8;
                        green += nsl->green * bright >> 8;
                        blue += nsl->blue * bright >> 8;
                    }
                }
            }

            SATURATE(red, 0, 255);
            SATURATE(green, 0, 255);
            SATURATE(blue, 0, 255);

            nc->red = red;
            nc->green = green;
            nc->blue = blue;

            x += dx;
            z += dz;

            nc += 1;
        }

        ly += dy;
    }

    ASSERT(nc == nd->colour + num_points);

    return dfcache_index;
}

void NIGHT_dfcache_destroy(std::uint8_t dfcache_index) {
    NIGHT_Dfcache *nd;

    ASSERT(WITHIN(dfcache_index, 1, NIGHT_MAX_DFCACHES - 1));

    //
    // Take out of the used list.
    //

    std::uint8_t next = NIGHT_dfcache_used;
    std::uint8_t *prev = &NIGHT_dfcache_used;

    while (1) {
        if (next == NULL) {
            //
            // It wasn't in the used list!
            //

            ASSERT(0);

            break;
        }

        ASSERT(WITHIN(next, 1, NIGHT_MAX_DFCACHES - 1));

        nd = &NIGHT_dfcache[next];

        if (next == dfcache_index) {
            //
            // This is the one to take out of the used list.
            //

            *prev = nd->next;

            break;
        }

        prev = &nd->next;
        next = nd->next;
    }

    //
    // Return the memory.
    //

    nd = &NIGHT_dfcache[dfcache_index];

    HEAP_give(nd->colour, nd->sizeof_colour);

    //
    // Insert into the free list.
    //

    nd->next = NIGHT_dfcache_free;
    NIGHT_dfcache_free = dfcache_index;
    nd->dfacet = NULL;
}

//
// Light at a point.
//

NIGHT_Colour NIGHT_get_light_at(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z) {
    std::int32_t i;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t xmid;
    std::int32_t zmid;

    std::int32_t xmin;
    std::int32_t zmin;

    std::int32_t xmax;
    std::int32_t zmax;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;
    std::int32_t lradius;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t dist;
    std::int32_t bright;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    std::int32_t vector[3];

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    NIGHT_Colour ans;

    OB_Info *oi;

    //
    // Initialise to the ambient light.
    //

    red = (NIGHT_amb_red * 3 >> 2) * NIGHT_LIGHT_MULTIPLIER;
    green = (NIGHT_amb_green * 3 >> 2) * NIGHT_LIGHT_MULTIPLIER;
    blue = (NIGHT_amb_blue * 3 >> 2) * NIGHT_LIGHT_MULTIPLIER;

    //
    // Look for static lights effecting this point.
    //

    xmid = x >> PAP_SHIFT_LO;
    zmid = z >> PAP_SHIFT_LO;

    xmin = xmid - 1;
    zmin = zmid - 1;

    xmax = xmid + 1;
    zmax = zmid + 1;

    SATURATE(xmin, 0, PAP_SIZE_LO - 1);
    SATURATE(zmin, 0, PAP_SIZE_LO - 1);
    SATURATE(xmax, 0, PAP_SIZE_LO - 1);
    SATURATE(zmax, 0, PAP_SIZE_LO - 1);

    for (mz = zmin; mz <= zmax; mz++)
        for (mx = xmin; mx <= xmax; mx++) {
            ns = &NIGHT_smap[mx][mz];

            map_x = mx << PAP_SHIFT_LO;
            map_z = mz << PAP_SHIFT_LO;

            for (i = 0; i < ns->number; i++) {
                nsl = &NIGHT_slight[ns->index + i];

                //
                // The position of the static light is relative
                // to the mapwho it is in.
                //

                lx = (nsl->x << 2) + map_x;
                ly = nsl->y;
                lz = (nsl->z << 2) + map_z;

                dx = abs(lx - x);
                dy = abs(ly - y);
                dz = abs(lz - z);

                lradius = nsl->radius << 2;

                //
                // Distance from the light.
                //

                dist = QDIST3(abs(dx), abs(dy), abs(dz));

                if (dist > lradius) {
                    //
                    // The point is too far from this light.
                    //
                } else {
                    bright = (128 - (dist * 128) / lradius) * NIGHT_LIGHT_MULTIPLIER;

                    red += nsl->red * bright >> 8;
                    green += nsl->green * bright >> 8;
                    blue += nsl->blue * bright >> 8;
                }
            }

            if (NIGHT_flag & NIGHT_FLAG_LIGHTS_UNDER_LAMPOSTS) {
                //
                // Lampost lights.
                //

                for (oi = OB_find(mx, mz); oi->prim; oi++) {
                    if (oi->prim < 5) {
                        FMATRIX_vector(
                            vector,
                            oi->yaw,
                            oi->pitch);

                        //
                        // The position of the static light is relative
                        // to the mapwho it is in.
                        //

                        lx = (-vector[0] >> 9) + oi->x;
                        ly = (-vector[1] >> 9) + oi->y + 0x100;
                        lz = (-vector[2] >> 9) + oi->z;

                        dx = abs(lx - x);
                        dy = abs(ly - y);
                        dz = abs(lz - z);

                        lradius = NIGHT_lampost_radius << 2;

                        //
                        // Distance from the light.
                        //

                        dist = QDIST3(abs(dx), abs(dy), abs(dz));

                        if (dist > lradius) {
                            //
                            // The point is too far from this light.
                            //
                        } else {
                            bright = (128 - (dist * 128) / lradius) * NIGHT_LIGHT_MULTIPLIER;

                            red += NIGHT_lampost_red * bright >> 8;
                            green += NIGHT_lampost_green * bright >> 8;
                            blue += NIGHT_lampost_blue * bright >> 8;
                        }
                    }
                }
            }
        }

    //
    // Fade out at the edge of the map.
    //

    if (x < (4 << 8) || x > (124 << 8) ||
        z < (4 << 8) || z > (124 << 8)) {
        std::int32_t mul;
        std::int32_t mulx;
        std::int32_t mulz;

        mulx = std::min(x, (128 << 8) - x);
        mulz = std::min(z, (128 << 8) - z);
        mul = std::min(mulx, mulz);

        red = red * mul >> 10;
        green = green * mul >> 10;
        blue = blue * mul >> 10;
    }

    SATURATE(red, 0, 255);
    SATURATE(green, 0, 255);
    SATURATE(blue, 0, 255);

    ans.red = red;
    ans.green = green;
    ans.blue = blue;

    return ans;
}

#ifndef PSX

NIGHT_Found NIGHT_found[NIGHT_MAX_FOUND];
std::int32_t NIGHT_found_upto;

void NIGHT_find(std::int32_t x, std::int32_t y, std::int32_t z) {
    std::int32_t i;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t xmid;
    std::int32_t zmid;

    std::int32_t xmin;
    std::int32_t zmin;

    std::int32_t xmax;
    std::int32_t zmax;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;
    std::int32_t lradius;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t dist;
    std::int32_t bright;

    std::int32_t vector[3];

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    NIGHT_Colour ans;

    OB_Info *oi;

    //
    // Clear the array with a red light!
    //

    NIGHT_found_upto = 0;

    NIGHT_found[NIGHT_found_upto].dx = 0;
    NIGHT_found[NIGHT_found_upto].dy = -256;
    NIGHT_found[NIGHT_found_upto].dz = 0;
    NIGHT_found[NIGHT_found_upto].r = 5.0f * NIGHT_LIGHT_MULTIPLIER;
    NIGHT_found[NIGHT_found_upto].g = 5.0f * NIGHT_LIGHT_MULTIPLIER;
    NIGHT_found[NIGHT_found_upto].b = 15.0f * NIGHT_LIGHT_MULTIPLIER;

    NIGHT_found_upto = 1;

    //
    // Look for static lights effecting this point.
    //

    xmid = x >> PAP_SHIFT_LO;
    zmid = z >> PAP_SHIFT_LO;

    xmin = xmid - 1;
    zmin = zmid - 1;

    xmax = xmid + 1;
    zmax = zmid + 1;

    SATURATE(xmin, 0, PAP_SIZE_LO - 1);
    SATURATE(zmin, 0, PAP_SIZE_LO - 1);
    SATURATE(xmax, 0, PAP_SIZE_LO - 1);
    SATURATE(zmax, 0, PAP_SIZE_LO - 1);

    for (mz = zmin; mz <= zmax; mz++)
        for (mx = xmin; mx <= xmax; mx++) {
            ns = &NIGHT_smap[mx][mz];

            map_x = mx << PAP_SHIFT_LO;
            map_z = mz << PAP_SHIFT_LO;

            for (i = 0; i < ns->number; i++) {
                nsl = &NIGHT_slight[ns->index + i];

                //
                // The position of the static light is relative
                // to the mapwho it is in.
                //

                lx = (nsl->x << 2) + map_x;
                ly = nsl->y + 0x20;
                lz = (nsl->z << 2) + map_z;

                dx = lx - x;
                dy = ly - y;
                dz = lz - z;

                lradius = nsl->radius << 2;

                //
                // Distance from the light.
                //

                dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                if (dist > lradius) {
                    //
                    // The point is too far from this light.
                    //
                } else {
                    //
                    // This light effects the sphere.
                    //

                    bright = (256 - (dist * 256) / lradius) * NIGHT_LIGHT_MULTIPLIER;

                    ASSERT(WITHIN(NIGHT_found_upto, 0, NIGHT_MAX_FOUND - 1));

                    NIGHT_found[NIGHT_found_upto].dx = dx * 256 / dist;
                    NIGHT_found[NIGHT_found_upto].dy = dy * 256 / dist;
                    NIGHT_found[NIGHT_found_upto].dz = dz * 256 / dist;
                    NIGHT_found[NIGHT_found_upto].r = nsl->red * bright >> 8;
                    NIGHT_found[NIGHT_found_upto].g = nsl->green * bright >> 8;
                    NIGHT_found[NIGHT_found_upto].b = nsl->blue * bright >> 8;

                    NIGHT_found_upto += 1;

                    if (NIGHT_found_upto >= NIGHT_MAX_FOUND) {
                        return;
                    }
                }
            }

            if (NIGHT_flag & NIGHT_FLAG_LIGHTS_UNDER_LAMPOSTS) {
                //
                // Lampost lights.
                //

                for (oi = OB_find(mx, mz); oi->prim; oi++) {
                    if (oi->prim < 5) {
                        FMATRIX_vector(
                            vector,
                            oi->yaw,
                            oi->pitch);

                        //
                        // The position of the static light is relative
                        // to the mapwho it is in.
                        //

                        lx = (-vector[0] >> 9) + oi->x;
                        ly = (-vector[1] >> 9) + oi->y + 0x120;
                        lz = (-vector[2] >> 9) + oi->z;

                        dx = lx - x;
                        dy = ly - y;
                        dz = lz - z;

                        lradius = NIGHT_lampost_radius << 2;

                        //
                        // Distance from the light.
                        //

                        dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                        if (dist > lradius) {
                            //
                            // The point is too far from this light.
                            //
                        } else {
                            ASSERT(WITHIN(NIGHT_found_upto, 0, NIGHT_MAX_FOUND - 1));

                            bright = (256 - (dist * 256) / lradius) * NIGHT_LIGHT_MULTIPLIER;

                            NIGHT_found[NIGHT_found_upto].dx = dx * 256 / dist;
                            NIGHT_found[NIGHT_found_upto].dy = dy * 256 / dist;
                            NIGHT_found[NIGHT_found_upto].dz = dz * 256 / dist;
                            NIGHT_found[NIGHT_found_upto].r = NIGHT_lampost_red * bright >> 8;
                            NIGHT_found[NIGHT_found_upto].g = NIGHT_lampost_green * bright >> 8;
                            NIGHT_found[NIGHT_found_upto].b = NIGHT_lampost_blue * bright >> 8;

                            NIGHT_found_upto += 1;

                            if (NIGHT_found_upto >= NIGHT_MAX_FOUND) {
                                return;
                            }
                        }
                    }
                }
            }
        }

    //
    // The dynamic lights...
    //

    NIGHT_Dlight *ndl;

    for (i = NIGHT_dlight_used; i; i = ndl->next) {
        ASSERT(WITHIN(i, 1, NIGHT_MAX_DLIGHTS - 1));

        ndl = &NIGHT_dlight[i];

        lx = ndl->x;
        ly = ndl->y + 0x20;
        lz = ndl->z;

        dx = lx - x;
        dy = ly - y;
        dz = lz - z;

        lradius = ndl->radius << 2;

        //
        // Distance from the light.
        //

        dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

        if (dist > lradius) {
            //
            // The point is too far from this light.
            //
        } else {
            ASSERT(WITHIN(NIGHT_found_upto, 0, NIGHT_MAX_FOUND - 1));

            bright = (256 - (dist * 256) / lradius) * NIGHT_LIGHT_MULTIPLIER;

            NIGHT_found[NIGHT_found_upto].dx = dx * 256 / dist;
            NIGHT_found[NIGHT_found_upto].dy = dy * 256 / dist;
            NIGHT_found[NIGHT_found_upto].dz = dz * 256 / dist;
            NIGHT_found[NIGHT_found_upto].r = ndl->red * bright >> 8;
            NIGHT_found[NIGHT_found_upto].g = ndl->green * bright >> 8;
            NIGHT_found[NIGHT_found_upto].b = ndl->blue * bright >> 8;

            NIGHT_found_upto += 1;

            if (NIGHT_found_upto >= NIGHT_MAX_FOUND) {
                return;
            }
        }
    }
}

#endif

void NIGHT_init() {
    //
    // Initialise the heap where all the cached memory lives.
    //

    HEAP_init();

    //
    // Initialises everything else.
    //

    NIGHT_cache_init();
    NIGHT_slight_init();
    NIGHT_dlight_init();
    NIGHT_dfcache_init();

    //
    // Sensible defaults.
    //

    NIGHT_sky_colour.red = 210;
    NIGHT_sky_colour.green = 200;
    NIGHT_sky_colour.blue = 240;

    NIGHT_lampost_red = 70;
    NIGHT_lampost_green = 70;
    NIGHT_lampost_blue = 36;

    NIGHT_lampost_radius = 255;

    NIGHT_flag = NIGHT_FLAG_DAYTIME | NIGHT_FLAG_DARKEN_BUILDING_POINTS;

    NIGHT_ambient(255, 255, 255, 110, -148, -177);
}

// ========================================================
//
// WALKABLE POINT LIGHTING
//
// ========================================================

#if !defined(NDEBUG) || defined(TARGET_DC)
// The DC doesn't actually need this fucntion, but the compiler gets very confused
// and exports the symbol (don't ask...) so then the linker gets all hot and bothered.

std::int32_t NIGHT_check_index(std::int32_t walkable_prim_point_index) {
    ASSERT(WITHIN(
        walkable_prim_point_index,
        first_walkable_prim_point,
        first_walkable_prim_point + number_of_walkable_prim_points - 1));

    return true;
}

#endif

NIGHT_Colour NIGHT_roof_walkable[MAX_ROOF_FACE4 * 4];
std::uint16_t hidden_roof_index[128][128];

void calc_lighting__for_point(std::int32_t prim_x, std::int32_t prim_y, std::int32_t prim_z, NIGHT_Colour *nc) {
    std::int32_t i;
    std::int32_t j;
    std::int32_t b;

    std::int32_t mx, mz;
    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    std::int32_t dist;
    std::int32_t dprod;
    std::int32_t bright;

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t mx1;
    std::int32_t mz1;
    std::int32_t mx2;
    std::int32_t mz2;

    std::int32_t lradius;
    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    //
    // The bounding box in which we search for static lighting affecting this point.
    //

    mx1 = prim_x - 0x400 >> PAP_SHIFT_LO;
    mz1 = prim_z - 0x400 >> PAP_SHIFT_LO;

    mx2 = prim_x + 0x400 >> PAP_SHIFT_LO;
    mz2 = prim_z + 0x400 >> PAP_SHIFT_LO;

    SATURATE(mx1, 0, PAP_SIZE_LO - 1);
    SATURATE(mz1, 0, PAP_SIZE_LO - 1);

    SATURATE(mx2, 0, PAP_SIZE_LO - 1);
    SATURATE(mz2, 0, PAP_SIZE_LO - 1);

    for (mx = mx1; mx <= mx2; mx++)
        for (mz = mz1; mz <= mz2; mz++) {
            map_x = mx << PAP_SHIFT_LO;
            map_z = mz << PAP_SHIFT_LO;

            ASSERT(WITHIN(mx, 0, PAP_SIZE_LO - 1));
            ASSERT(WITHIN(mz, 0, PAP_SIZE_LO - 1));

            ns = &NIGHT_smap[mx][mz];

            for (j = 0; j < ns->number; j++) {
                ASSERT(WITHIN(ns->index + j, 0, NIGHT_MAX_SLIGHTS - 1));

                nsl = &NIGHT_slight[ns->index + j];

                lx = (nsl->x << 2) + map_x;
                ly = nsl->y;
                lz = (nsl->z << 2) + map_z;

                lradius = nsl->radius << 2;

                if (ly <= prim_y) {
                    //
                    // We assume that the normals of the walkable points all point
                    // up.  So to be lit up the light must be higher than the point.
                    //

                    continue;
                }

                //
                // Light this point...
                //

                dx = prim_x - lx;
                dy = prim_y - ly;
                dz = prim_z - lz;

                dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                if (dist > lradius) {
                    //
                    // Too far away.
                    //

                    continue;
                }

                //
                // Lighting dot product... but with a hardcoded normal
                // of (0,1,0)
                //

                dprod = -256 * dy;
                dprod /= dist;
                bright = 256 - (dist << 8) / lradius;
                bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                red = nc->red;
                green = nc->green;
                blue = nc->blue;

                red += nsl->red * bright >> 8;
                green += nsl->green * bright >> 8;
                blue += nsl->blue * bright >> 8;

                SATURATE(red, 0, 255);
                SATURATE(green, 0, 255);
                SATURATE(blue, 0, 255);

                nc->red = red;
                nc->green = green;
                nc->blue = blue;
            }
        }
}

void NIGHT_generate_roof_walkable() {
    std::int32_t i;
    std::int32_t j;
    std::int32_t b;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t mx1;
    std::int32_t mz1;
    std::int32_t mx2;
    std::int32_t mz2;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t lradius;
    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t dist;
    std::int32_t dprod;
    std::int32_t bright;
    std::int32_t walk;

    std::int32_t amb_amount;
    std::int32_t amb_red;
    std::int32_t amb_green;
    std::int32_t amb_blue;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;
    NIGHT_Colour *nc;

    DBuilding *db;
    DWalkable *dw;
    std::int32_t max_face = 0;

    //
    // The first walkable prim-point...
    //

    NIGHT_first_walkable_prim_point = first_walkable_prim_point;

    //
    // The lighting for all the roof faces.
    //

    for (b = 1; b < next_dbuilding; b++) {
        db = &dbuildings[b];

        //
        // The ambient colour of the walkable prim points.
        //

        if (db->Type != BUILDING_TYPE_CRATE_IN) {
            amb_amount = 128;
            amb_amount += -NIGHT_amb_norm_y >> 1;
            amb_amount *= NIGHT_LIGHT_MULTIPLIER;

            amb_red = NIGHT_amb_red * amb_amount >> 8;
            amb_green = NIGHT_amb_green * amb_amount >> 8;
            amb_blue = NIGHT_amb_blue * amb_amount >> 8;
        } else {
            amb_red = 30;
            amb_green = 30;
            amb_blue = 30;
        }

        for (walk = db->Walkable; walk; walk = dw->Next) {
            dw = &dwalkables[walk];

            for (i = dw->StartFace4; i < dw->EndFace4; i++) {
                struct RoofFace4 *rf;
                std::int32_t prim_x, prim_y, prim_z, point;
                std::int32_t mx, mz;
                std::int32_t roof_face_x, roof_face_z;

                rf = &roof_faces4[i];
                roof_face_x = (rf->RX & 127) << 8;
                roof_face_z = (rf->RZ & 127) << 8;

                if (i > max_face) {
                    max_face = i;
                }

                for (point = 0; point < 4; point++) {
                    switch (point) {
                    case 0:
                        prim_x = roof_face_x;
                        prim_y = rf->Y;
                        prim_z = roof_face_z;
                        break;
                    case 1:
                        prim_x = roof_face_x + 256;
                        prim_y = (rf->Y) + (rf->DY[0] << ROOF_SHIFT);
                        prim_z = roof_face_z;
                        break;
                    case 3:
                        prim_x = roof_face_x + 256;
                        prim_y = (rf->Y) + (rf->DY[1] << ROOF_SHIFT);
                        prim_z = roof_face_z + 256;
                        break;
                    case 2:
                        prim_x = roof_face_x;
                        prim_y = (rf->Y) + (rf->DY[2] << ROOF_SHIFT);
                        prim_z = roof_face_z + 256;
                        break;
                    }

                    //
                    // Initialise the points to almost all the ambient colour.
                    //

                    nc = &NIGHT_roof_walkable[i * 4 + point];

                    nc->red = amb_red;
                    nc->green = amb_green;
                    nc->blue = amb_blue;

                    calc_lighting__for_point(prim_x, prim_y, prim_z, nc);
                }
            }
        }
    }

    {
        std::int32_t x, z, point, prim_x, prim_y, prim_z;

        amb_amount = 128;
        amb_amount += -NIGHT_amb_norm_y >> 1;
        amb_amount *= NIGHT_LIGHT_MULTIPLIER;

        amb_red = NIGHT_amb_red * amb_amount >> 8;
        amb_green = NIGHT_amb_green * amb_amount >> 8;
        amb_blue = NIGHT_amb_blue * amb_amount >> 8;

        for (x = 0; x < 128; x++) {
            for (z = 0; z < 128; z++) {
                if (PAP_hi[x][z].Flags & PAP_FLAG_ROOF_EXISTS) {
                    max_face++;

                    hidden_roof_index[x][z] = max_face;

                    for (point = 0; point < 4; point++) {
                        switch (point) {
                        case 0:
                            prim_x = (x) << 8;
                            prim_y = MAVHEIGHT(x, z) << 6;
                            prim_z = (z) << 8;
                            break;
                        case 1:
                            prim_x = (x + 1) << 8;
                            prim_y = MAVHEIGHT(x, z) << 6;
                            prim_z = (z) << 8;
                            break;
                        case 2:
                            prim_x = (x + 1) << 8;
                            prim_y = MAVHEIGHT(x, z) << 6;
                            prim_z = (z + 1) << 8;
                            break;
                        case 3:
                            prim_x = (x) << 8;
                            prim_y = MAVHEIGHT(x, z) << 6;
                            prim_z = (z + 1) << 8;
                            break;
                        }

                        nc = &NIGHT_roof_walkable[max_face * 4 + point];

                        nc->red = amb_red;
                        nc->green = amb_green;
                        nc->blue = amb_blue;

                        calc_lighting__for_point(prim_x, prim_y, prim_z, nc);
                    }
                }
            }
        }
    }
}

std::int32_t NIGHT_first_walkable_prim_point;
NIGHT_Colour NIGHT_walkable[NIGHT_MAX_WALKABLE];

void NIGHT_generate_walkable_lighting() {
    std::int32_t i;
    std::int32_t j;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t mx1;
    std::int32_t mz1;
    std::int32_t mx2;
    std::int32_t mz2;

    std::int32_t map_x;
    std::int32_t map_z;

    std::int32_t lradius;
    std::int32_t lx;
    std::int32_t ly;
    std::int32_t lz;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t dist;
    std::int32_t dprod;
    std::int32_t bright;

    std::int32_t amb_amount;
    std::int32_t amb_red;
    std::int32_t amb_green;
    std::int32_t amb_blue;

    std::int32_t red;
    std::int32_t green;
    std::int32_t blue;

    PrimPoint *pp;
    NIGHT_Smap *ns;
    NIGHT_Slight *nsl;
    NIGHT_Colour *nc;

    //
    // generate the roof walkable face lighting
    //
    NIGHT_generate_roof_walkable();

    //
    // The first walkable prim-point...
    //

    NIGHT_first_walkable_prim_point = first_walkable_prim_point;

    //
    // The ambient colour of the walkable prim points.
    //

    return;

    amb_amount = 128;
    amb_amount += -NIGHT_amb_norm_y >> 1;
    amb_amount *= NIGHT_LIGHT_MULTIPLIER;

    amb_red = NIGHT_amb_red * amb_amount >> 8;
    amb_green = NIGHT_amb_green * amb_amount >> 8;
    amb_blue = NIGHT_amb_blue * amb_amount >> 8;

    //
    // The lighting for all walkable the prim points.
    //

    for (i = 0; i < number_of_walkable_prim_points; i++) {
        ASSERT(WITHIN(i, 0, NIGHT_MAX_WALKABLE));
        ASSERT(WITHIN(first_walkable_prim_point + i, 1, next_prim_point - 1));

        pp = &prim_points[first_walkable_prim_point + i];

        //
        // Initialise the points to almost all the ambient colour.
        //

        nc = &NIGHT_walkable[i];

        nc->red = amb_red;
        nc->green = amb_green;
        nc->blue = amb_blue;

        //
        // The bounding box in which we search for static lighting affecting this point.
        //

        mx1 = pp->X - 0x400 >> PAP_SHIFT_LO;
        mz1 = pp->Z - 0x400 >> PAP_SHIFT_LO;

        mx2 = pp->X + 0x400 >> PAP_SHIFT_LO;
        mz2 = pp->Z + 0x400 >> PAP_SHIFT_LO;

        SATURATE(mx1, 0, PAP_SIZE_LO - 1);
        SATURATE(mz1, 0, PAP_SIZE_LO - 1);

        SATURATE(mx2, 0, PAP_SIZE_LO - 1);
        SATURATE(mz2, 0, PAP_SIZE_LO - 1);

        for (mx = mx1; mx <= mx2; mx++)
            for (mz = mz1; mz <= mz2; mz++) {
                map_x = mx << PAP_SHIFT_LO;
                map_z = mz << PAP_SHIFT_LO;

                ASSERT(WITHIN(mx, 0, PAP_SIZE_LO - 1));
                ASSERT(WITHIN(mz, 0, PAP_SIZE_LO - 1));

                ns = &NIGHT_smap[mx][mz];

                for (j = 0; j < ns->number; j++) {
                    ASSERT(WITHIN(ns->index + j, 0, NIGHT_MAX_SLIGHTS - 1));

                    nsl = &NIGHT_slight[ns->index + j];

                    lx = (nsl->x << 2) + map_x;
                    ly = nsl->y;
                    lz = (nsl->z << 2) + map_z;

                    lradius = nsl->radius << 2;

                    if (ly <= pp->Y) {
                        //
                        // We assume that the normals of the walkable points all point
                        // up.  So to be lit up the light must be higher than the point.
                        //

                        continue;
                    }

                    //
                    // Light this point...
                    //

                    dx = pp->X - lx;
                    dy = pp->Y - ly;
                    dz = pp->Z - lz;

                    dist = QDIST3(abs(dx), abs(dy), abs(dz)) + 1;

                    if (dist > lradius) {
                        //
                        // Too far away.
                        //

                        continue;
                    }

                    //
                    // Lighting dot product... but with a hardcoded normal
                    // of (0,1,0)
                    //

                    dprod = -256 * dy;
                    dprod /= dist;
                    bright = 256 - (dist << 8) / lradius;
                    bright = (bright * dprod >> 8) * NIGHT_LIGHT_MULTIPLIER;

                    red = nc->red;
                    green = nc->green;
                    blue = nc->blue;

                    red += nsl->red * bright >> 8;
                    green += nsl->green * bright >> 8;
                    blue += nsl->blue * bright >> 8;

                    SATURATE(red, 0, 255);
                    SATURATE(green, 0, 255);
                    SATURATE(blue, 0, 255);

                    nc->red = red;
                    nc->green = green;
                    nc->blue = blue;
                }
            }
    }
}

void NIGHT_destroy_all_cached_info() {
    HEAP_init();
    NIGHT_cache_init();
    NIGHT_dfcache_init();
}

std::int32_t NIGHT_load_ed_file(char *name) {
    // #ifndef	PSX
    std::int32_t i;

    std::int32_t sizeof_ed_light;
    std::int32_t ed_max_lights;
    std::int32_t sizeof_night_colour;
    std::int32_t ed_light_free;
    std::int32_t data_left;
    std::uint8_t version = 0;

    ED_Light el;

    NIGHT_Colour col;

    //	FILE *handle;
    MFFileHandle handle;

    //	handle = MF_Fopen(name, "rb");
    handle = FileOpen(name);

    if ((!handle) || (handle == FILE_OPEN_ERROR)) {
        return false;
    }

    //
    // The header.
    //

    /*	if (fread(&sizeof_ed_light,     sizeof(std::int32_t), 1, handle) != 1) goto file_error;
            if (fread(&ed_max_lights,       sizeof(std::int32_t), 1, handle) != 1) goto file_error;
            if (fread(&sizeof_night_colour, sizeof(std::int32_t), 1, handle) != 1) goto file_error;*/
    if (FileRead(handle, &sizeof_ed_light, sizeof(std::int32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &ed_max_lights, sizeof(std::int32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &sizeof_night_colour, sizeof(std::int32_t)) < 0)
        goto file_error;

    version = sizeof_ed_light >> 16;
    sizeof_ed_light &= 0xffff;

    if (version)
        TRACE("(new version %d lighting)\n", version);
    else
        TRACE("(old version lighting)\n");

    if (sizeof_ed_light != sizeof(ED_Light) ||
        sizeof_night_colour != sizeof(NIGHT_Colour)) {
        //
        // Incompatable files.
        //

        //		MF_Fclose(handle);
        FileClose(handle);

        return false;
    }

    // okay. since the original lighting editor files didn't include a file version
    // number byte *poke* we have to 'guess'...

    //	data_left=FileSize(handle)-FileSeek(handle,SEEK_MODE_CURRENT,0);
    /*	data_left=FileSize(handle)-12;
            if (data_left!=(sizeof_ed_light*ed_max_lights)+40) {
                    FileRead(handle,&version,sizeof(version));
                    TRACE("(\"new\" lighting file version: %d)\n",version);
            } else TRACE("(\"old\" lighting file)\n");*/

    //
    // The lights.
    //

    for (i = 0; i < ed_max_lights; i++) {
        std::int32_t count = 0;
        //		if (fread(&el, sizeof(ED_Light), 1, handle) != 1) goto file_error;
        if (FileRead(handle, &el, sizeof(ED_Light)) < 0)
            goto file_error;

        if (el.used) {
            count++;

            NIGHT_slight_create(
                el.x,
                el.y,
                el.z,
                el.range,
                el.red,
                el.green,
                el.blue);
        }
    }

    //
    // We can ignore this.
    //

    //	if (fread(&ed_light_free, sizeof(std::int32_t), 1, handle) != 1) goto file_error;
    if (FileRead(handle, &ed_light_free, sizeof(std::int32_t)) < 0)
        goto file_error;

    //
    // Other stuff.
    //

    std::uint32_t flag;
    std::uint32_t amb_d3d_colour;
    std::uint32_t amb_d3d_specular;
    std::int32_t amb_red;
    std::int32_t amb_green;
    std::int32_t amb_blue;
    std::int8_t lampost_red;
    std::int8_t lampost_green;
    std::int8_t lampost_blue;
    std::uint8_t padding;
    std::int32_t lampost_radius;
    NIGHT_Colour sky_colour;

    /*	if (fread(&flag,             sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&amb_d3d_colour,   sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&amb_d3d_specular, sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&amb_red,          sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&amb_green,        sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&amb_blue,         sizeof(std::uint32_t),        1, handle) != 1) goto file_error;
            if (fread(&lampost_red,      sizeof(std::int8_t),        1, handle) != 1) goto file_error;
            if (fread(&lampost_green,    sizeof(std::int8_t),        1, handle) != 1) goto file_error;
            if (fread(&lampost_blue,     sizeof(std::int8_t),        1, handle) != 1) goto file_error;
            if (fread(&padding,          sizeof(std::uint8_t),        1, handle) != 1) goto file_error;
            if (fread(&lampost_radius,   sizeof(std::int32_t),        1, handle) != 1) goto file_error;
            if (fread(&sky_colour,       sizeof(NIGHT_Colour), 1, handle) != 1) goto file_error;*/

    if (FileRead(handle, &flag, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &amb_d3d_colour, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &amb_d3d_specular, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &amb_red, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &amb_green, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &amb_blue, sizeof(std::uint32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &lampost_red, sizeof(std::int8_t)) < 0)
        goto file_error;
    if (FileRead(handle, &lampost_green, sizeof(std::int8_t)) < 0)
        goto file_error;
    if (FileRead(handle, &lampost_blue, sizeof(std::int8_t)) < 0)
        goto file_error;
    if (FileRead(handle, &padding, sizeof(std::uint8_t)) < 0)
        goto file_error;
    if (FileRead(handle, &lampost_radius, sizeof(std::int32_t)) < 0)
        goto file_error;
    if (FileRead(handle, &sky_colour, sizeof(NIGHT_Colour)) < 0)
        goto file_error;

    NIGHT_ambient(
        amb_red * 820 >> 8,
        amb_green * 820 >> 8,
        amb_blue * 820 >> 8,
        110, -148, -177); // These constants crop up everywhere!

    NIGHT_flag = flag;
    NIGHT_sky_colour = sky_colour;
    NIGHT_lampost_radius = lampost_radius;
    NIGHT_lampost_red = lampost_red;
    NIGHT_lampost_green = lampost_green;
    NIGHT_lampost_blue = lampost_blue;

    //	MF_Fclose(handle);
    FileClose(handle);

    return true;

file_error:;

    //	MF_Fclose(handle);
    TRACE("Oops it's a bit buggered.\n");
    FileClose(handle);

    // #endif
    return false;
}
