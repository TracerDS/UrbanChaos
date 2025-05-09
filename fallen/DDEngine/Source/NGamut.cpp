//
// The gamut.
//

#include <MFStdLib.h>
#include "ngamut.h"

#include <algorithm>

NGAMUT_Gamut NGAMUT_gamut[NGAMUT_SIZE];
std::int32_t NGAMUT_zmin;
std::int32_t NGAMUT_zmax;
std::int32_t NGAMUT_xmin;

//
// Initialises the gamut.
//

void NGAMUT_init() {
    std::int32_t i;

    //
    // This could be the first time we have called NGAMUT_init.
    // We have to initialise the whole array.
    //

    for (i = 0; i < NGAMUT_SIZE; i++) {
        NGAMUT_gamut[i].xmin = INFINITY;
        NGAMUT_gamut[i].xmax = -INFINITY;
    }

    NGAMUT_xmin = INFINITY;
    NGAMUT_zmin = INFINITY;
    NGAMUT_zmax = -INFINITY;
}

//
// Pushes out the gamut so it includes the given square.
//

inline void NGAMUT_add_square(std::int32_t x, std::int32_t z) {
    if (!WITHIN(z, 0, NGAMUT_SIZE - 2)) {
        //
        // The zline is off the map.
        //

        return;
    }

    //
    // Make sure that the x-coord is in range.
    //

    SATURATE(x, 0, NGAMUT_SIZE - 2);

    //
    // Push out the gamut.
    //

    if (z < NGAMUT_zmin) {
        NGAMUT_zmin = z;
    }
    if (z > NGAMUT_zmax) {
        NGAMUT_zmax = z;
    }

    if (x < NGAMUT_xmin) {
        NGAMUT_xmin = x;
    }

    if (x < NGAMUT_gamut[z].xmin) {
        NGAMUT_gamut[z].xmin = x;
    }
    if (x > NGAMUT_gamut[z].xmax) {
        NGAMUT_gamut[z].xmax = x;
    }
}

//
// Pushes out the gamut so it includes the given line.
//

void NGAMUT_add_line(float p1x, float p1z, float p2x, float p2z) {
    float x;
    std::int32_t z;

    std::int32_t m1x;
    std::int32_t m1z;

    std::int32_t m2x;
    std::int32_t m2z;

    float xfrac;
    float zfrac;

    float dx;
    float dz;
    float dxdz;

    //
    // Sort the points by z.
    //

    if (p2z < p1z) {
        SWAP_FL(p1x, p2x);
        SWAP_FL(p1z, p2z);
    }

    //
    // Add the end points of the line.
    //

    m1x = std::int32_t(p1x);
    m1z = std::int32_t(p1z);

    NGAMUT_add_square(m1x, m1z);

    m2x = std::int32_t(p2x);
    m2z = std::int32_t(p2z);

    NGAMUT_add_square(m2x, m2z);

    //
    // Go along the line.
    //

    dx = p2x - p1x;
    dz = p2z - p1z;

    if (dz == 0.0f) {
        //
        // No z to traverse.
        //
    } else {
        dxdz = dx / dz;

        //
        // Move down to the next zline.
        //

        zfrac = 1.0F - (p1z - m1z);
        xfrac = zfrac * dxdz;

        x = p1x + xfrac;
        z = m1z + 1;

        while (z <= m2z) {
            NGAMUT_add_square(std::int32_t(x), std::int32_t(z));
            NGAMUT_add_square(std::int32_t(x), std::int32_t(z - 1));

            x += dxdz;
            z += 1;
        }
    }
}

void NGAMUT_view_square(float mid_x, float mid_z, float radius) {
    std::int32_t i;
    std::int32_t z;

    std::int32_t zmin;
    std::int32_t zmax;

    std::int32_t xmin;
    std::int32_t xmax;

    zmin = std::int32_t(mid_z - radius);
    zmax = std::int32_t(mid_z + radius);

    xmin = std::int32_t(mid_x - radius);
    xmax = std::int32_t(mid_x + radius);

    SATURATE(zmin, 0, NGAMUT_SIZE - 1);
    SATURATE(zmax, 0, NGAMUT_SIZE - 1);

    SATURATE(xmin, 0, NGAMUT_SIZE - 1);
    SATURATE(xmax, 0, NGAMUT_SIZE - 1);

    NGAMUT_zmin = zmin;
    NGAMUT_zmax = zmax;

    NGAMUT_xmin = xmin;

    //
    // Initialise the rest of the gamut...
    //

    for (i = 0; i < NGAMUT_SIZE; i++) {
        NGAMUT_gamut[i].xmin = INFINITY;
        NGAMUT_gamut[i].xmax = -INFINITY;
    }

    //
    // Mark the square of the gamut.
    //

    for (z = zmin; z <= zmax; z++) {
        NGAMUT_gamut[z].xmin = xmin;
        NGAMUT_gamut[z].xmax = xmax;
    }
}

NGAMUT_Gamut NGAMUT_point_gamut[NGAMUT_SIZE];
std::int32_t NGAMUT_point_zmin;
std::int32_t NGAMUT_point_zmax;

void NGAMUT_calculate_point_gamut() {
    std::int32_t i;

    NGAMUT_point_zmin = NGAMUT_zmin;
    NGAMUT_point_zmax = NGAMUT_zmax + 1;

    for (i = NGAMUT_zmin + 1; i <= NGAMUT_zmax; i++) {
        NGAMUT_point_gamut[i].xmin = std::min(NGAMUT_gamut[i].xmin, NGAMUT_gamut[i - 1].xmin);
        NGAMUT_point_gamut[i].xmax = std::max(NGAMUT_gamut[i].xmax + 1, NGAMUT_gamut[i - 1].xmax + 1);
    }

    NGAMUT_point_gamut[NGAMUT_point_zmin].xmin = NGAMUT_gamut[NGAMUT_zmin].xmin;
    NGAMUT_point_gamut[NGAMUT_point_zmin].xmax = NGAMUT_gamut[NGAMUT_zmin].xmax + 1;
    NGAMUT_point_gamut[NGAMUT_point_zmax].xmin = NGAMUT_gamut[NGAMUT_zmax].xmin;
    NGAMUT_point_gamut[NGAMUT_point_zmax].xmax = NGAMUT_gamut[NGAMUT_zmax].xmax + 1;
}

NGAMUT_Gamut NGAMUT_out_gamut[NGAMUT_SIZE];
std::int32_t NGAMUT_out_zmin;
std::int32_t NGAMUT_out_zmax;

#ifndef MIN3
#define MIN3(a, b, c) (std::min(std::min(a, b), c))
#endif

#ifndef MAX3
#define MAX3(a, b, c) (std::max(std::max(a,b,c))
#endif

void NGAMUT_calculate_out_gamut() {
    std::int32_t i;

    std::int32_t z1;
    std::int32_t z2;
    std::int32_t z3;

    std::int32_t xmin;
    std::int32_t xmax;

    NGAMUT_out_zmin = NGAMUT_zmin - 1;
    NGAMUT_out_zmax = NGAMUT_zmax + 1;

    SATURATE(NGAMUT_out_zmin, 0, NGAMUT_SIZE - 1);
    SATURATE(NGAMUT_out_zmax, 0, NGAMUT_SIZE - 1);

    for (i = NGAMUT_out_zmin; i <= NGAMUT_out_zmax; i++) {
        z1 = i - 1;
        z2 = i;
        z3 = i + 1;

        if (z1 < NGAMUT_zmin) {
            z1 = NGAMUT_zmin;
        }
        if (z3 > NGAMUT_zmax) {
            z3 = NGAMUT_zmax;
        }

        SATURATE(z2, NGAMUT_zmin, NGAMUT_zmax);

        xmin = std::min({NGAMUT_gamut[z1].xmin - 1, NGAMUT_gamut[z2].xmin - 1, NGAMUT_gamut[z3].xmin - 1});
        xmax = std::max({NGAMUT_gamut[z1].xmax + 1, NGAMUT_gamut[z2].xmax + 1, NGAMUT_gamut[z3].xmax + 1});

        if (xmin < 0) {
            xmin = 0;
        }
        if (xmax > NGAMUT_SIZE - 1) {
            xmax = NGAMUT_SIZE - 1;
        }

        NGAMUT_out_gamut[i].xmin = xmin;
        NGAMUT_out_gamut[i].xmax = xmax;
    }
}

NGAMUT_Gamut NGAMUT_lo_gamut[NGAMUT_SIZE_LO];
std::int32_t NGAMUT_lo_zmin;
std::int32_t NGAMUT_lo_zmax;

void NGAMUT_calculate_lo_gamut() {
    std::int32_t i;
    std::int32_t j;

    std::int32_t z;

    std::int32_t xmin;
    std::int32_t xmax;

    std::int32_t out_xmin;
    std::int32_t out_xmax;

    NGAMUT_lo_zmin = (NGAMUT_point_zmin - 1) / (NGAMUT_SIZE / NGAMUT_SIZE_LO);
    NGAMUT_lo_zmax = (NGAMUT_point_zmax + 1) / (NGAMUT_SIZE / NGAMUT_SIZE_LO);

    SATURATE(NGAMUT_lo_zmin, 0, NGAMUT_SIZE_LO - 1);
    SATURATE(NGAMUT_lo_zmax, 0, NGAMUT_SIZE_LO - 1);

    for (i = NGAMUT_lo_zmin; i <= NGAMUT_lo_zmax; i++) {
        xmin = +INFINITY;
        xmax = -INFINITY;

        for (j = 0; j < (NGAMUT_SIZE / NGAMUT_SIZE_LO); j++) {
            z = i * (NGAMUT_SIZE / NGAMUT_SIZE_LO);
            z += j;

            if (WITHIN(z, NGAMUT_point_zmin, NGAMUT_point_zmax)) {
                out_xmin = (NGAMUT_point_gamut[z].xmin - 1) / (NGAMUT_SIZE / NGAMUT_SIZE_LO);
                out_xmax = (NGAMUT_point_gamut[z].xmax + 1) / (NGAMUT_SIZE / NGAMUT_SIZE_LO);

                if (out_xmin < xmin) {
                    xmin = out_xmin;
                }
                if (out_xmax > xmax) {
                    xmax = out_xmax;
                }
            }
        }
        ASSERT(xmax >= xmin || (xmin == +INFINITY && xmax == -INFINITY));

        SATURATE(xmin, 0, NGAMUT_SIZE_LO - 1);
        SATURATE(xmax, 0, NGAMUT_SIZE_LO - 1);

        NGAMUT_lo_gamut[i].xmin = xmin;
        NGAMUT_lo_gamut[i].xmax = xmax;
    }
}
