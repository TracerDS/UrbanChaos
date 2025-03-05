#include "game.h"
#include <MFStdLib.h>
#include "glitter.h"

#ifndef PSX

//
// The sparks.
//

#define GLITTER_SPARK_LIFE (32)
#define GLITTER_SPARK_GRAVITY (-2)

typedef struct
{
    std::uint16_t x;
    std::int16_t y;
    std::uint16_t z;
    std::int8_t dx;
    std::int8_t dy;
    std::int8_t dz;
    std::uint8_t next; // The next spark in the linked list.
    std::uint8_t die;
    std::uint8_t useless_padding;

} GLITTER_Spark;

#define GLITTER_MAX_SPARKS 128

GLITTER_Spark GLITTER_spark[GLITTER_MAX_SPARKS];
std::uint8_t GLITTER_spark_free;

//
// The collections of sparks.
//

typedef struct
{
    std::uint8_t flag;
    std::uint8_t spark; // The linked list of sparks.
    std::uint8_t next;  // On this mapwho.
    std::uint8_t map_x;
    std::uint8_t map_z;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

} GLITTER_Glitter;

#define GLITTER_MAX_GLITTER 32

GLITTER_Glitter GLITTER_glitter[GLITTER_MAX_GLITTER];
std::int32_t GLITTER_glitter_last;

//
// The glitter mapwho.
//

#define GLITTER_MAPWHO 128

std::uint8_t GLITTER_mapwho[GLITTER_MAPWHO];

void GLITTER_init() {
    std::int32_t i;

    //
    // Clear the mapwho.
    //

    for (i = 0; i < GLITTER_MAPWHO; i++) {
        GLITTER_mapwho[i] = 0;
    }

    //
    // Mark all the glitter as unused.
    //

    for (i = 0; i < GLITTER_MAX_GLITTER; i++) {
        GLITTER_glitter[i].flag &= ~GLITTER_FLAG_USED;
    }

    GLITTER_glitter_last = 1;

    //
    // Build the free list of sparks.
    //

    GLITTER_spark_free = 1;

    for (i = 1; i < GLITTER_MAX_SPARKS - 1; i++) {
        GLITTER_spark[i].next = i + 1;
    }

    GLITTER_spark[GLITTER_MAX_SPARKS - 1].next = 0;
}

std::uint8_t GLITTER_create(
    std::uint8_t flag,
    std::uint8_t map_x,
    std::uint8_t map_z,
    std::uint32_t colour) {
    std::int32_t i;

    GLITTER_Glitter *gg;

    if (!WITHIN(map_z, 0, GLITTER_MAPWHO - 1)) {
        //
        // Off the map.
        //

        return 0;
    }

    //
    // Look for a spare glitter structure.
    //

    for (i = 0; i < GLITTER_MAX_GLITTER; i++) {
        GLITTER_glitter_last += 1;

        if (GLITTER_glitter_last >= GLITTER_MAX_GLITTER) {
            GLITTER_glitter_last = 1;
        }

        gg = &GLITTER_glitter[GLITTER_glitter_last];

        if (!(gg->flag & GLITTER_FLAG_USED)) {
            goto found_unused_glitter;
        }
    }

    //
    // No spare glitter.
    //

    return 0;

found_unused_glitter:;

    //
    // Initialise the new glitter.
    //

    gg->flag = flag;
    gg->flag |= GLITTER_FLAG_USED;
    gg->flag &= ~GLITTER_FLAG_DESTROY;
    gg->map_x = map_x;
    gg->map_z = map_z;
    gg->red = (colour >> 16) & 0xff;
    gg->green = (colour >> 8) & 0xff;
    gg->blue = (colour >> 0) & 0xff;
    gg->spark = 0;

    //
    // Add te glitter to the mapwho.
    //

    gg->next = GLITTER_mapwho[map_z];
    GLITTER_mapwho[map_z] = GLITTER_glitter_last;

    return GLITTER_glitter_last;
}

void GLITTER_add(
    std::uint8_t glitter,
    std::int32_t x,
    std::int32_t y,
    std::int32_t z) {
    std::uint8_t spark;

    if (!glitter) {
        return;
    }

    //
    // Are there any sparks in the free list?
    //

    if (!GLITTER_spark_free) {
        return;
    }

    //
    // The glitter we are adding the spark to.
    //

    GLITTER_Glitter *gg;

    ASSERT(WITHIN(glitter, 1, GLITTER_MAX_GLITTER - 1));

    gg = &GLITTER_glitter[glitter];

    //
    // The new spark.
    //

    GLITTER_Spark *gs;

    ASSERT(WITHIN(GLITTER_spark_free, 1, GLITTER_MAX_SPARKS - 1));

    spark = GLITTER_spark_free;
    gs = &GLITTER_spark[spark];
    GLITTER_spark_free = gs->next;

    gs->x = x;
    gs->y = y;
    gs->z = z;

    std::int32_t dx = (rand() & 0x3f) - 0x1f;
    std::int32_t dy = (rand() & 0x3f) - 0x1f;
    std::int32_t dz = (rand() & 0x3f) - 0x1f;

    if (gg->flag & GLITTER_FLAG_DXPOS) {
        dx = +abs(dx);
    }
    if (gg->flag & GLITTER_FLAG_DXNEG) {
        dx = -abs(dx);
    }
    if (gg->flag & GLITTER_FLAG_DYPOS) {
        dy = +abs(dy);
    }
    if (gg->flag & GLITTER_FLAG_DYNEG) {
        dy = -abs(dy);
    }
    if (gg->flag & GLITTER_FLAG_DZPOS) {
        dz = +abs(dz);
    }
    if (gg->flag & GLITTER_FLAG_DZNEG) {
        dz = -abs(dz);
    }

    gs->dx = dx;
    gs->dy = dy;
    gs->dz = dz;

    gs->die = GLITTER_SPARK_LIFE;

    //
    // Put the spark in the glitter's linked list.
    //

    gs->next = gg->spark;
    gg->spark = spark;

    return;
}

void GLITTER_destroy(std::uint8_t glitter) {
    if (!glitter) {
        return;
    }

    ASSERT(WITHIN(glitter, 1, GLITTER_MAX_GLITTER));

    GLITTER_glitter[glitter].flag |= GLITTER_FLAG_DESTROY;
}

void GLITTER_process() {
    std::int32_t i;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::uint8_t spark;
    std::uint8_t next;
    std::uint8_t *prev;

    GLITTER_Glitter *gg;
    GLITTER_Spark *gs;

    for (i = 1; i < GLITTER_MAX_GLITTER; i++) {
        gg = &GLITTER_glitter[i];

        if (!(gg->flag & GLITTER_FLAG_USED)) {
            continue;
        }

        //
        // Remove all dead sparks from the glitter.
        //

        next = gg->spark;
        prev = &gg->spark;

        while (1) {
            if (next == 0) {
                break;
            }

            ASSERT(WITHIN(next, 1, GLITTER_MAX_SPARKS - 1));

            gs = &GLITTER_spark[next];

            if (gs->die == 0) {
                spark = next;

                //
                // Take this spark out of the linked list.
                //

                *prev = gs->next;
                next = gs->next;

                //
                // Put this spark in the free list.
                //

                gs->next = GLITTER_spark_free;
                GLITTER_spark_free = spark;
            } else {
                //
                // Go onto the next spark.
                //

                prev = &gs->next;
                next = gs->next;
            }
        }

        if (gg->flag & GLITTER_FLAG_DESTROY) {
            if (!gg->spark) {
                //
                // Kill this glitter.
                //

                gg->flag &= ~GLITTER_FLAG_USED;

                //
                // Take it out of the mapwho.
                //

                ASSERT(WITHIN(gg->map_z, 0, GLITTER_MAPWHO - 1));

                prev = &GLITTER_mapwho[gg->map_z];
                next = GLITTER_mapwho[gg->map_z];

                while (1) {
                    if (next == i) {
                        //
                        // Found the glitter.
                        //

                        *prev = gg->next;

                        break;
                    } else {
                        ASSERT(WITHIN(next, 1, GLITTER_MAX_GLITTER - 1));

                        prev = &GLITTER_glitter[next].next;
                        next = GLITTER_glitter[next].next;
                    }
                }

                continue;
            }
        }

        //
        // Process all the sparks in this glitter's linked list.
        //

        for (spark = gg->spark; spark; spark = gs->next) {
            ASSERT(WITHIN(spark, 1, GLITTER_MAX_SPARKS - 1));

            gs = &GLITTER_spark[spark];

            //
            // Gravity...
            //

            dy = gs->dy;
            dy += GLITTER_SPARK_GRAVITY;

            if (dy < -127) {
                dy = -127;
            }

            gs->dy = dy;

            gs->x += gs->dx >> 3;
            gs->y += gs->dy >> 3;
            gs->z += gs->dz >> 3;

            gs->die -= 1;
        }
    }
}

std::uint8_t GLITTER_get_z;
std::uint8_t GLITTER_get_xmin;
std::uint8_t GLITTER_get_xmax;
std::uint8_t GLITTER_get_glitter;
std::uint8_t GLITTER_get_spark;
GLITTER_Info GLITTER_get_info;

void GLITTER_get_start(std::uint8_t xmin, std::uint8_t xmax, std::uint8_t z) {
    GLITTER_get_z = z;
    GLITTER_get_xmin = xmin;
    GLITTER_get_xmax = xmax;

    if (WITHIN(GLITTER_get_z, 0, GLITTER_MAPWHO - 1)) {
        GLITTER_get_glitter = GLITTER_mapwho[GLITTER_get_z];

        if (GLITTER_get_glitter) {
            ASSERT(WITHIN(GLITTER_get_glitter, 1, GLITTER_MAX_GLITTER - 1));

            GLITTER_get_spark = GLITTER_glitter[GLITTER_get_glitter].spark;
        }
    } else {
        GLITTER_get_glitter = 0;
        GLITTER_get_spark = 0;
    }
}

GLITTER_Info *GLITTER_get_next() {
    GLITTER_Glitter *gg;
    GLITTER_Spark *gs;

tail_recurse:;

    if (!GLITTER_get_glitter) {
        return nullptr;
    }

    ASSERT(WITHIN(GLITTER_get_glitter, 1, GLITTER_MAX_GLITTER - 1));

    gg = &GLITTER_glitter[GLITTER_get_glitter];

    if (!GLITTER_get_spark) {
        ASSERT(WITHIN(gg->next, 0, GLITTER_MAX_GLITTER - 1));

        GLITTER_get_glitter = gg->next;
        GLITTER_get_spark = GLITTER_glitter[gg->next].spark;

        goto tail_recurse;
    }

    ASSERT(WITHIN(GLITTER_get_spark, 1, GLITTER_MAX_SPARKS - 1));

    gs = &GLITTER_spark[GLITTER_get_spark];

    GLITTER_get_info.x1 = gs->x;
    GLITTER_get_info.y1 = gs->y;
    GLITTER_get_info.z1 = gs->z;
    GLITTER_get_info.x2 = gs->x + (gs->dx >> 2);
    GLITTER_get_info.y2 = gs->y + (gs->dy >> 2);
    GLITTER_get_info.z2 = gs->z + (gs->dz >> 2);

    std::int32_t red = (gg->red * gs->die) / GLITTER_SPARK_LIFE;
    std::int32_t green = (gg->green * gs->die) / GLITTER_SPARK_LIFE;
    std::int32_t blue = (gg->blue * gs->die) / GLITTER_SPARK_LIFE;

    GLITTER_get_info.colour = (red << 16) | (green << 8) | (blue << 0);

    GLITTER_get_spark = gs->next;

    return &GLITTER_get_info;
}

#endif