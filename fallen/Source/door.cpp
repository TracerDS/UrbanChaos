//
// Doors
//

#include "game.h"
#include "door.h"
#include "mav.h"
#include "supermap.h"
#include "memory.h"

//
// Doors in the process of opening or closing.
//

DOOR_Door* DOOR_door;

//
// Finds a door facet.
//

std::uint16_t DOOR_find(
    std::int32_t world_x,
    std::int32_t world_z) {
    std::int32_t fx;
    std::int32_t fz;
    std::int32_t dx;
    std::int32_t dz;
    std::int32_t score;

    std::int32_t mx1 = world_x - 0x200 >> PAP_SHIFT_LO;
    std::int32_t mz1 = world_z - 0x200 >> PAP_SHIFT_LO;
    std::int32_t mx2 = world_x + 0x200 >> PAP_SHIFT_LO;
    std::int32_t mz2 = world_z + 0x200 >> PAP_SHIFT_LO;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t f_list;
    std::int32_t exit;
    std::int32_t i_facet;

    DFacet* df;

    SATURATE(mx1, 0, PAP_SIZE_LO - 1);
    SATURATE(mz1, 0, PAP_SIZE_LO - 1);
    SATURATE(mx2, 0, PAP_SIZE_LO - 1);
    SATURATE(mz2, 0, PAP_SIZE_LO - 1);

    std::int32_t best_score = INFINITY;
    std::int32_t best_facet = 0;

    for (mx = mx1; mx <= mx2; mx++)
        for (mz = mz1; mz <= mz2; mz++) {
            f_list = PAP_2LO(mx, mz).ColVectHead;

            if (f_list) {
                exit = false;

                while (1) {
                    i_facet = facet_links[f_list];

                    if (i_facet < 0) {
                        i_facet = -i_facet;
                        exit = true;
                    }

                    df = &dfacets[i_facet];

                    if (df->FacetType == STOREY_TYPE_OUTSIDE_DOOR) {
                        fx = df->x[0] + df->x[1] << 7;
                        fz = df->z[0] + df->z[1] << 7;

                        dx = abs(fx - world_x);
                        dz = abs(fz - world_z);

                        score = QDIST2(dx, dz);

                        if (best_score > score) {
                            best_score = score;
                            best_facet = i_facet;
                        }
                    }

                    if (exit) {
                        break;
                    }

                    f_list += 1;
                }
            }
        }

    return best_facet;
}

void DOOR_open(std::int32_t world_x, std::int32_t world_z) {
    std::uint16_t facet;

    facet = DOOR_find(world_x, world_z);

    if (facet) {
        std::int32_t i;

        std::int32_t x;
        std::int32_t z;

        std::int32_t dx;
        std::int32_t dz;

        std::int32_t mx;
        std::int32_t mz;

        std::int32_t left;
        std::int32_t right;

        DFacet* df = &dfacets[facet];

        //
        // Start it opening.
        //

        df->FacetFlags |= FACET_FLAG_OPEN;

        for (i = 0; i < DOOR_MAX_DOORS; i++) {
            if (DOOR_door[i].facet == facet) {
                goto already_being_processed;
            }
        }

        for (i = 0; i < DOOR_MAX_DOORS; i++) {
            if (!DOOR_door[i].facet) {
                DOOR_door[i].facet = facet;
            }
        }

    already_being_processed:;

        //
        // Make sure nobody can navigate through this facet.
        //

        dx = SIGN(df->x[1] - df->x[0]);
        dz = SIGN(df->z[1] - df->z[0]);

        x = df->x[0];
        z = df->z[0];

        if (dx) {
            if (dx == 1) {
                left = MAV_DIR_ZS;
                right = MAV_DIR_ZL;
            } else {
                left = MAV_DIR_ZL;
                right = MAV_DIR_ZS;
            }
        } else {
            if (dz == 1) {
                left = MAV_DIR_XL;
                right = MAV_DIR_XS;
            } else {
                left = MAV_DIR_XS;
                right = MAV_DIR_XL;
            }
        }

        while (1) {
            mx = (x << 8) + (dx << 7);
            mz = (z << 8) + (dz << 7);

            mx += -dz << 7;
            mz += +dx << 7;

            mx >>= 8;
            mz >>= 8;

            MAV_turn_movement_on(mx, mz, left);
            MAV_turn_car_movement_on(mx, mz, left);

            mx = (x << 8) + (dx << 7);
            mz = (z << 8) + (dz << 7);

            mx -= -dz << 7;
            mz -= +dx << 7;

            mx >>= 8;
            mz >>= 8;

            MAV_turn_movement_on(mx, mz, right);
            MAV_turn_car_movement_on(mx, mz, right);

            x += dx;
            z += dz;

            if (x == df->x[1] &&
                z == df->z[1]) {
                break;
            }
        }
    }
}

void DOOR_shut(std::int32_t world_x, std::int32_t world_z) {
    std::uint16_t facet;

    facet = DOOR_find(world_x, world_z);

    if (facet) {
        std::int32_t i;

        std::int32_t x;
        std::int32_t z;

        std::int32_t dx;
        std::int32_t dz;

        std::int32_t mx;
        std::int32_t mz;

        std::int32_t left;
        std::int32_t right;

        DFacet* df = &dfacets[facet];

        //
        // Start it shutting.
        //

        df->FacetFlags &= ~FACET_FLAG_OPEN;

        for (i = 0; i < DOOR_MAX_DOORS; i++) {
            if (DOOR_door[i].facet == facet) {
                goto already_being_processed;
            }
        }

        for (i = 0; i < DOOR_MAX_DOORS; i++) {
            if (!DOOR_door[i].facet) {
                DOOR_door[i].facet = facet;
            }
        }

    already_being_processed:;

        //
        // Make sure nobody can navigate through this facet.
        //

        dx = SIGN(df->x[1] - df->x[0]);
        dz = SIGN(df->z[1] - df->z[0]);

        x = df->x[0];
        z = df->z[0];

        if (dx) {
            //
            // This is right...
            //

            if (dx == 1) {
                left = MAV_DIR_ZS;
                right = MAV_DIR_ZL;
            } else {
                left = MAV_DIR_ZL;
                right = MAV_DIR_ZS;
            }
        } else {
            if (dz == 1) {
                left = MAV_DIR_XL;
                right = MAV_DIR_XS;
            } else {
                left = MAV_DIR_XS;
                right = MAV_DIR_XL;
            }
        }

        while (1) {
            mx = (x << 8) + (dx << 7);
            mz = (z << 8) + (dz << 7);

            mx += -dz << 7;
            mz += +dx << 7;

            mx >>= 8;
            mz >>= 8;

            MAV_turn_movement_off(mx, mz, left);
            MAV_turn_car_movement_off(mx, mz, left);

            mx = (x << 8) + (dx << 7);
            mz = (z << 8) + (dz << 7);

            mx -= -dz << 7;
            mz -= +dx << 7;

            mx >>= 8;
            mz >>= 8;

            MAV_turn_movement_off(mx, mz, right);
            MAV_turn_car_movement_off(mx, mz, right);

            x += dx;
            z += dz;

            if (x == df->x[1] &&
                z == df->z[1]) {
                break;
            }
        }
    }
}

//
// Make the door knock people over as it opens...
//

void DOOR_knock_over_people(DFacet* df, std::int32_t side) {
    std::int32_t dx;
    std::int32_t dz;

    std::int32_t x1;
    std::int32_t z1;

    std::int32_t x2;
    std::int32_t z2;

    dx = df->x[1] - df->x[0];
    dz = df->z[1] - df->z[0];

    //
    // Well.. there are more important things to do!
    //
}

void DOOR_process() {
    std::int32_t i;
    std::int32_t open;
    std::int32_t max;

    DFacet* df;

    for (i = 0; i < DOOR_MAX_DOORS; i++) {
        if (DOOR_door[i].facet) {
            df = &dfacets[DOOR_door[i].facet];

            open = df->Open;

            if (df->FacetFlags & FACET_FLAG_OPEN) {
                open += 4;

#define DOOR_AJAR 15

                if (df->FacetFlags & FACET_FLAG_90DEGREE) {
                    max = 128 - DOOR_AJAR;
                } else {
                    max = 255 - DOOR_AJAR;
                }

                if (open > max) {
                    open = max;

                    DOOR_door[i].facet = 0;
                }
            } else {
                open -= 4;

                if (open < 0) {
                    open = 0;

                    DOOR_door[i].facet = 0;
                }
            }

            df->Open = open;
        }
    }
}
