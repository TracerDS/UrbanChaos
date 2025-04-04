//
// A system for wandering people.
//

#include "game.h"
#include "pap.h"
#include "road.h"
#include "wand.h"
#include "memory.h"
#include "ob.h"

#ifndef PSX
void WAND_init() {
    std::int32_t x;
    std::int32_t z;

    std::int32_t dx;
    std::int32_t dz;

    std::int32_t mx;
    std::int32_t mz;

    PAP_Hi *ph;
    OB_Info *oi;

    //
    // Clear the 'wander' bit from the mapwho.
    //

    for (x = 0; x < PAP_SIZE_HI; x++)
        for (z = 0; z < PAP_SIZE_HI; z++) {
            ph = &PAP_2HI(x, z);

            ph->Flags &= ~PAP_FLAG_WANDER;
        }

    //
    // We can wander on all squares that are one or two squares from a road
    //

    for (x = 0; x < PAP_SIZE_HI; x++)
        for (z = 0; z < PAP_SIZE_HI; z++) {
            ph = &PAP_2HI(x, z);

            if (!ROAD_is_road(x, z) && !(ph->Flags & PAP_FLAG_HIDDEN)) {
                for (dx = -2; dx <= 2; dx++)
                    for (dz = -2; dz <= 2; dz++) {
                        mx = x + dx;
                        mz = z + dz;

                        if (WITHIN(mx, 0, PAP_SIZE_HI - 1) &&
                            WITHIN(mz, 0, PAP_SIZE_HI - 1)) {
                            if (ROAD_is_road(mx, mz)) {
                                if ((ph->Flags & PAP_FLAG_HIDDEN) == 0) {
                                    ph->Flags |= PAP_FLAG_WANDER;
                                }

                                goto done_this_square;
                            }
                        }
                    }
            } else if (ROAD_is_zebra(x, z)) {
                if ((ph->Flags & PAP_FLAG_HIDDEN) == 0)
                    ph->Flags |= PAP_FLAG_WANDER;
            }

        done_this_square:;
        }

    //
    // Don't wander on squares convered by prims.
    //

    for (x = 0; x < PAP_SIZE_LO; x++)
        for (z = 0; z < PAP_SIZE_LO; z++) {
            for (oi = OB_find(x, z); oi->prim; oi++) {
                switch (prim_get_collision_model(oi->prim)) {
                case PRIM_COLLIDE_BOX:
                case PRIM_COLLIDE_SMALLBOX:
                case PRIM_COLLIDE_CYLINDER:

                    //
                    // If this prim covers the middle of a mapsquare, stop it being
                    // a wander square.
                    //

                    mx = oi->x >> 8;
                    mz = oi->z >> 8;

                    if (!(PAP_2HI(mx, mz).Flags & PAP_FLAG_HIDDEN)) {
                        if (oi->y <= PAP_calc_map_height_at(oi->x, oi->z) + 0x40) {
                            PAP_2HI(mx, mz).Flags &= ~PAP_FLAG_WANDER;
                        }
                    }

                    break;

                case PRIM_COLLIDE_NONE:
                    break;

                default:
                    ASSERT(0);
                    break;
                }
            }
        }
}

#endif

std::int32_t WAND_square_is_wander(std::int32_t mx, std::int32_t mz) {
    if ((PAP_2HI(mx, mz).Flags & PAP_FLAG_WANDER) &&
        (PAP_2HI(mx, mz).Flags & PAP_FLAG_HIDDEN) == 0) {
        return true;
    } else {
        return false;
    }
}

//
// Returns true if the person wants to wander on this square.
//

std::int32_t WAND_square_for_person(Thing *p_person, std::int32_t mx, std::int32_t mz) {
    if (p_person->Class == CLASS_BAT) {
        return WAND_square_is_wander(mx, mz) || ROAD_is_road(mx, mz);
    } else {
        ASSERT(p_person->Class == CLASS_PERSON);

        if (p_person->Genus.Person->pcom_zone) {
            extern std::uint8_t PCOM_get_zone_for_position(std::int32_t x, std::int32_t z);

            return PCOM_get_zone_for_position(mx << 8, mz << 8) & p_person->Genus.Person->pcom_zone;
        } else {
            return WAND_square_is_wander(mx, mz);
        }
    }
}

void WAND_get_next_place(
    Thing *p_person,
    std::int32_t *wand_world_x,
    std::int32_t *wand_world_z) {
    std::int32_t i;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t dx;
    std::int32_t dz;

    std::int32_t dprod;
    std::int32_t score;

    std::int32_t mid_x;
    std::int32_t mid_z;

    std::int32_t off_x;
    std::int32_t off_z;

    std::int32_t best_x;
    std::int32_t best_z;
    std::int32_t best_score;

    mid_x = p_person->WorldPos.X >> 16;
    mid_z = p_person->WorldPos.Z >> 16;

#define WAND_MAX_LOOKS 8
#define WAND_MAX_MOVE 3

    const struct
    {
        std::int8_t dx;
        std::int8_t dz;

    } look[WAND_MAX_LOOKS] =
        {
            {+WAND_MAX_MOVE, 0             },
            {-WAND_MAX_MOVE, 0             },

            {0,              +WAND_MAX_MOVE},
            {0,              -WAND_MAX_MOVE}
    };

    std::int8_t offset[4] = {-1, 0, 0, +1};

    //
    // Carry on walking in the same direction by default.
    //

    dx = -SIN(p_person->Draw.Tweened->Angle) >> 7;
    dz = -COS(p_person->Draw.Tweened->Angle) >> 7;

    best_score = -INFINITY;
    best_x = (p_person->WorldPos.X >> 8) + dx >> 8;
    best_z = (p_person->WorldPos.Z >> 8) + dz >> 8;

    SATURATE(best_x, 0, PAP_SIZE_HI - 1);
    SATURATE(best_z, 0, PAP_SIZE_HI - 1);

    for (i = 0; i < WAND_MAX_LOOKS; i++) {
        off_x = look[i].dx + offset[(Random() >> 3) & 0x3];
        off_z = look[i].dz + offset[(Random() >> 3) & 0x3];

        mx = mid_x + off_x;
        mz = mid_z + off_z;

        if (WITHIN(mx, 0, PAP_SIZE_HI - 1) &&
            WITHIN(mz, 0, PAP_SIZE_HI - 1)) {
            if (WAND_square_for_person(p_person, mx, mz)) {
                dprod = dx * off_x + dz * off_z;

                if (dprod <= 0) {
                    if (dprod == 0) {
                        score = 0;
                    } else {
                        score = -Random() % (dprod >> 1);
                    }
                } else {
                    score = Random() % dprod;
                }

                if (score > best_score) {
                    best_x = mx;
                    best_z = mz;
                    best_score = score;
                }
            }
        }
    }

    //
    // Didn't find anywhere? Then extend your search!
    //

    if (best_score == -INFINITY) {
        //
        // REVERSE OUR SCORING SYSTEM! The smaller the score the better.
        //

        best_score = INFINITY;

        for (i = 0; i < 16; i++) {
            off_x = (Random() & 0x1f) - 0xf;
            off_z = (Random() & 0x1f) - 0xf;

            mx = mid_x + off_x;
            mz = mid_z + off_z;

            if (WITHIN(mx, 0, PAP_SIZE_HI - 1) &&
                WITHIN(mz, 0, PAP_SIZE_HI - 1)) {
                if (WAND_square_for_person(p_person, mx, mz)) {
                    score = abs(off_x) + abs(off_z);

                    if (score < best_score) {
                        best_score = score;
                        best_x = mx;
                        best_z = mz;
                    }
                }
            }
        }
    }

    *wand_world_x = (best_x << 8) + 0x80;
    *wand_world_z = (best_z << 8) + 0x80;
}

#ifndef PSX
#ifndef TARGET_DC
void WAND_draw(std::int32_t map_x, std::int32_t map_z) {
    std::int32_t dx;
    std::int32_t dz;

    std::int32_t mx;
    std::int32_t mz;

    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;
    std::int32_t y;

    for (dx = -10; dx <= 10; dx++)
        for (dz = -10; dz <= 10; dz++) {
            mx = map_x + dx;
            mz = map_z + dz;

            if (WITHIN(mx, 0, PAP_SIZE_HI - 1) &&
                WITHIN(mz, 0, PAP_SIZE_HI - 1)) {
                if (WAND_square_is_wander(mx, mz)) {
                    x1 = mx + 0 << 8;
                    z1 = mz + 0 << 8;

                    x2 = mx + 1 << 8;
                    z2 = mz + 1 << 8;

                    y = PAP_calc_map_height_at(x1 + x2 << 7, z1 + z2 << 7);

                    AENG_world_line(
                        x1, y, z1, 32, 0xffffff,
                        x2, y, z2, 32, 0xffffff,
                        true);

                    AENG_world_line(
                        x1, y, z2, 32, 0xffffff,
                        x2, y, z1, 32, 0xffffff,
                        true);
                }
            }
        }
}
#endif
#endif

#define SEARCH_SIZE 1

std::int32_t WAND_find_good_start_point(std::int32_t *mapx, std::int32_t *mapz) {
    Thing *p_person;
    std::int32_t dx, dz, x, z, minx, maxx, minz, maxz;
    std::int32_t angle;

    p_person = NET_PERSON(0);
    angle = p_person->Draw.Tweened->Angle;

    angle += (Random() & 511) - 256;
    angle &= 2047;
    dx = -(SIN(angle) * DRAW_DIST) >> 8;
    dz = -(COS(angle) * DRAW_DIST) >> 8;

    x = (p_person->WorldPos.X >> 8) + dx;
    z = (p_person->WorldPos.Z >> 8) + dz;

    x >>= 8;
    z >>= 8;

    minx = x - SEARCH_SIZE;
    maxx = x + SEARCH_SIZE;
    minz = z - SEARCH_SIZE;
    maxz = z + SEARCH_SIZE;

    SATURATE(minx, 2, 125);
    SATURATE(minz, 2, 125);
    SATURATE(maxx, 2, 125);
    SATURATE(maxz, 2, 125);

    for (x = minx; x <= maxx; x++)
        for (z = minz; z <= maxz; z++) {
            if (WAND_square_is_wander(x, z)) {
                *mapx = (x << 8) + 128;
                *mapz = (z << 8) + 128;
                return (1);
            }
        }

    return (0);
}
#define SEARCH_SIZE2 2
std::int32_t WAND_find_good_start_point_near(std::int32_t *mapx, std::int32_t *mapz) {
    std::int32_t dx, dz, x, z, minx, maxx, minz, maxz;
    std::int32_t angle;

    x = *mapx;
    z = *mapz;

    minx = x - SEARCH_SIZE2;
    maxx = x + SEARCH_SIZE2;
    minz = z - SEARCH_SIZE2;
    maxz = z + SEARCH_SIZE2;

    SATURATE(minx, 2, 125);
    SATURATE(minz, 2, 125);
    SATURATE(maxx, 2, 125);
    SATURATE(maxz, 2, 125);

    for (x = minx; x <= maxx; x++)
        for (z = minz; z <= maxz; z++) {
            if (WAND_square_is_wander(x, z)) {
                *mapx = (x << 8) + 128;
                *mapz = (z << 8) + 128;
                return (1);
            }
        }

    return (0);
}
std::int32_t WAND_find_good_start_point_for_car(std::int32_t *posx, std::int32_t *posz, std::int32_t *yaw, std::int32_t anywhere) {
    Thing *p_person = NET_PERSON(0);
    std::int32_t x, z;

    // get a random heading
    std::int32_t heading = (Random() >> 9) & 1;

    if (anywhere) {
        x = ((Random() % 112) + 8) << 8;
        z = ((Random() % 112) + 8) << 8;
    } else {
        // get a random angle - not just ahead of the player
        std::int32_t angle = Random() & 2047;

        // get the position at this angle and (DRAW_DIST) away
        std::int32_t dx = (SIN(angle) * DRAW_DIST) >> 8;
        std::int32_t dz = (COS(angle) * DRAW_DIST) >> 8;

        x = (p_person->WorldPos.X >> 8) + dx;
        z = (p_person->WorldPos.Z >> 8) + dz;

        //		SATURATE(x,8<<8,120<<8);   //clipping these values onto map, seems a good idea
        //		SATURATE(z,8<<8,120<<8);
    }

    // find the nearest bit of road
    std::int32_t rn1;
    std::int32_t rn2;

    ROAD_find(x, z, &rn1, &rn2);

    // get the nearest point on the road, and select
    // a side of the road
    std::int32_t x1, z1;
    std::int32_t x2, z2;

    ROAD_node_pos(rn1, &x1, &z1);
    ROAD_node_pos(rn2, &x2, &z2);

    if (x1 == x2) {
        // NS road
        if (heading) {
            *yaw = 0;
            x = x1 - 0x180;
        } else {
            *yaw = 1024;
            x = x1 + 0x180;
        }
        // don't appear right on a junction

        if ((z < z1 && z < z2) || (z > z1 && z > z2)) // stops cars being created on rooftops or other silly places
            return (0);

        if (abs(z - z1) < 0x280) return 0;
        if (abs(z - z2) < 0x280) return 0;

        if (PAP_2HI(x >> 8, z >> 8).Flags & PAP_FLAG_HIDDEN) {
            //
            // create on a roof are you mad?
            //
            ASSERT(0);
        }
    } else {
        // EW road
        if (heading) {
            *yaw = 1536;
            z = z1 - 0x180;
        } else {
            *yaw = 512;
            z = z1 + 0x180;
        }

        if ((x < x1 && x < x2) || (x > x1 && x > x2)) // stops cars being created on rooftops or other silly places
            return (0);

        // don't appear right on a junction
        if (abs(x - x1) < 0x280) return 0;
        if (abs(x - x2) < 0x280) return 0;

        if (PAP_2HI(x >> 8, z >> 8).Flags & PAP_FLAG_HIDDEN) {
            //
            // create on a roof are you mad?
            //
            ASSERT(0);
        }
    }

    // check it hasn't moved right near to Darci
    if (!anywhere) {
        std::int32_t dx = (p_person->WorldPos.X >> 8) - x;
        std::int32_t dz = (p_person->WorldPos.Z >> 8) - z;

        if (QDIST2(dx, dz) < (DRAW_DIST << 8)) {
            return 0;
        }
    }

    // check for things nearby
    if (THING_find_nearest(x, 0, z, 0x400, 1 << CLASS_VEHICLE)) {
        return 0;
    }

    if (PAP_2HI(x >> 8, z >> 8).Flags & PAP_FLAG_HIDDEN) {
        //
        // create on a roof are you mad?
        //
        ASSERT(0);
    }

    //
    // Definately create on a road,
    //
    ASSERT(ROAD_is_road(x >> 8, z >> 8));

    *posx = x;
    *posz = z;
    return 1;
}
