//
// Arrays of points for morphing between.
//

#include "game.h"
#include <MFStdLib.h>
#include "morph.h"

//
// The filenames of the sex files describing each morph.
//

char *MORPH_filename[MORPH_NUMBER] =
    {
        "morph\\bird01.asc",
        "morph\\bird03.asc"};

//
// All the points.
//

#define MORPH_MAX_POINTS 1024

MORPH_Point MORPH_point[MORPH_MAX_POINTS];
std::int32_t MORPH_point_upto;

typedef struct
{
    std::uint16_t num_points;
    std::uint16_t index;

} MORPH_Morph;

MORPH_Morph MORPH_morph[MORPH_NUMBER];

#ifndef PSX
//
// Load in the morphs.
//

void MORPH_load() {
    std::int32_t i;
    std::int32_t d;
    float x;
    float y;
    float z;
    std::int32_t match;

    MORPH_Morph *mm;

    FILE *handle;

    char line[256];
    char *ch;

    //
    // Get rid of old point info.
    //

    MORPH_point_upto = 0;

    for (i = 0; i < MORPH_NUMBER; i++) {
        mm = &MORPH_morph[i];

        mm->num_points = 0;
        mm->index = MORPH_point_upto;
        ;

        handle = MF_Fopen(MORPH_filename[i], "rb");

        if (handle != NULL) {
            while (fgets(line, 256, handle)) {
                for (ch = line; *ch; ch++) {
                    if (*ch == ',') {
                        *ch = '.';
                    }
                }

                match = sscanf(line, "Vertex %d: X: %f Y: %f Z: %f", &d, &x, &y, &z);

                if (match == 4) {
                    ASSERT(WITHIN(MORPH_point_upto, 0, MORPH_MAX_POINTS - 1));

                    //
                    // Convert from 3ds orientation to our one.
                    //

                    SWAP_FL(y, z);

                    //
                    // Rescale.
                    //

                    x *= 2.56F;
                    y *= 2.56F;
                    z *= 2.56F;

                    MORPH_point[MORPH_point_upto].x = std::int16_t(x);
                    MORPH_point[MORPH_point_upto].y = std::int16_t(y);
                    MORPH_point[MORPH_point_upto].z = std::int16_t(z);

                    MORPH_point_upto += 1;
                    mm->num_points += 1;
                }

                match = sscanf(line, "Vertex: (%f,%f,%f)", &x, &y, &z);

                if (match == 3) {
                    ASSERT(WITHIN(MORPH_point_upto, 0, MORPH_MAX_POINTS - 1));

                    //
                    // Convert from 3ds orientation to our one.
                    //

                    SWAP_FL(y, z);

                    //
                    // Rescale.
                    //

                    x *= 2.56F;
                    y *= 2.56F;
                    z *= 2.56F;

                    MORPH_point[MORPH_point_upto].x = std::int16_t(x);
                    MORPH_point[MORPH_point_upto].y = std::int16_t(y);
                    MORPH_point[MORPH_point_upto].z = std::int16_t(z);

                    MORPH_point_upto += 1;
                    mm->num_points += 1;
                }
            }

            MF_Fclose(handle);
        }
    }
}

MORPH_Point *MORPH_get_points(std::int32_t morph) {
    MORPH_Morph *mm;
    ASSERT(WITHIN(morph, 0, MORPH_NUMBER - 1));
    mm = &MORPH_morph[morph];
    return &MORPH_point[mm->index];
}

std::int32_t MORPH_get_num_points(std::int32_t morph) {
    MORPH_Morph *mm;
    ASSERT(WITHIN(morph, 0, MORPH_NUMBER - 1));
    mm = &MORPH_morph[morph];
    return mm->num_points;
}

#endif