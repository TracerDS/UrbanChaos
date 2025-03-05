//
// An A-Z of the current city.
//

#include "game.h"
#include "az.h"
#include "pap.h"
#include "supermap.h"
#include "memory.h"

//
// The lines.
//

#define AZ_MAX_LINES 512

AZ_Line AZ_line[AZ_MAX_LINES];
std::int32_t AZ_line_upto;

void AZ_init() {
    AZ_line_upto = 0;
}

void AZ_create_lines() {
    std::int32_t x;
    std::int32_t z;

    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;

    std::int32_t found_curb;

    AZ_line_upto = 0;

    //
    // Scan through the map looking for the edges of roads.
    //

    for (x = 0; x < MAP_WIDTH; x++) {
        found_curb = false;

        for (z = 0; z < MAP_HEIGHT; z++) {
            if ((PAP_2HI(x, z).Flags & PAP_FLAG_SINK_POINT) &&
                !(PAP_2HI(x, z).Flags & PAP_FLAG_NOUPPER)) {
                //
                // This is a point on a curb.
                //

                if (!found_curb) {
                    found_curb = true;
                    x1 = x;
                    z1 = z;
                }
            } else {
                if (found_curb) {
                    found_curb = false;
                    x2 = x;
                    z2 = z - 1;

                    if (x1 != x2 || z1 != z2) {
                        //
                        // Add a road line.
                        //

                        if (WITHIN(AZ_line_upto, 0, AZ_MAX_LINES - 1)) {
                            AZ_line[AZ_line_upto].type = AZ_LINE_TYPE_ROAD;
                            AZ_line[AZ_line_upto].x1 = x1;
                            AZ_line[AZ_line_upto].z1 = z1;
                            AZ_line[AZ_line_upto].x2 = x2;
                            AZ_line[AZ_line_upto].z2 = z2;

                            AZ_line_upto += 1;
                        }
                    }
                }
            }
        }
    }

    for (z = 0; z < MAP_HEIGHT; z++) {
        found_curb = false;

        for (x = 0; x < MAP_WIDTH; x++) {
            if ((PAP_2HI(x, z).Flags & PAP_FLAG_SINK_POINT) &&
                !(PAP_2HI(x, z).Flags & PAP_FLAG_NOUPPER)) {
                //
                // This is a point on a curb.
                //

                if (!found_curb) {
                    found_curb = true;
                    x1 = x;
                    z1 = z;
                }
            } else {
                if (found_curb) {
                    found_curb = false;
                    x2 = x - 1;
                    z2 = z;

                    if (x1 != x2 || z1 != z2) {
                        //
                        // Add a road line.
                        //

                        if (WITHIN(AZ_line_upto, 0, AZ_MAX_LINES - 1)) {
                            AZ_line[AZ_line_upto].type = AZ_LINE_TYPE_ROAD;
                            AZ_line[AZ_line_upto].x1 = x1;
                            AZ_line[AZ_line_upto].z1 = z1;
                            AZ_line[AZ_line_upto].x2 = x2;
                            AZ_line[AZ_line_upto].z2 = z2;

                            AZ_line_upto += 1;
                        }
                    }
                }
            }
        }
    }

    //
    // Put the lines in for the buildings- just use the dfacets!
    //

    std::int32_t i;
    std::int32_t fence;

    DFacet *df;

    for (i = 1; i < next_dfacet; i++) {
        df = &dfacets[i];

        x1 = df->x[0];
        z1 = df->z[0];
        x2 = df->x[1];
        z2 = df->z[1];

        fence =
            df->FacetType == STOREY_TYPE_FENCE ||
            df->FacetType == STOREY_TYPE_FENCE_BRICK ||
            df->FacetType == STOREY_TYPE_FENCE_FLAT;

        if (WITHIN(AZ_line_upto, 0, AZ_MAX_LINES - 1)) {
            AZ_line[AZ_line_upto].type = (fence) ? AZ_LINE_TYPE_FENCE : AZ_LINE_TYPE_BUILDING;
            AZ_line[AZ_line_upto].x1 = x1;
            AZ_line[AZ_line_upto].z1 = z1;
            AZ_line[AZ_line_upto].x2 = x2;
            AZ_line[AZ_line_upto].z2 = z2;

            AZ_line_upto += 1;
        }
    }
}
