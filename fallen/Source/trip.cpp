//
// Tripwires.
//

#include "game.h"
#include "trip.h"
#include "animate.h"
#include <algorithm>

TRIP_Wire *TRIP_wire; //[TRIP_MAX_WIRES];
std::int32_t TRIP_wire_upto;

#ifndef PSX
void TRIP_init() {
    TRIP_wire_upto = 1;
}
#endif

std::uint8_t TRIP_create(
    std::int32_t y,
    std::int32_t x1,
    std::int32_t z1,
    std::int32_t x2,
    std::int32_t z2) {
    std::int32_t i;

    TRIP_Wire *tw;

    //
    // Look for a tripwire that already exists.
    //

    for (i = 1; i < TRIP_wire_upto; i++) {
        tw = &TRIP_wire[i];

        if (tw->y == y &&
            tw->x1 == x1 &&
            tw->z1 == z1 &&
            tw->x2 == x2 &&
            tw->z2 == z2) {
            //
            // Found a tripwire in this position already!
            //

            return i;
        }
    }

    //
    // We have to create a new tripwire.
    //

    if (TRIP_wire_upto >= TRIP_MAX_WIRES) {
        //
        // No more trip-wires available.
        //

        return NULL;
    }

    tw = &TRIP_wire[TRIP_wire_upto++];

    tw->counter = rand();
    tw->y = y;
    tw->x1 = x1;
    tw->z1 = z1;
    tw->x2 = x2;
    tw->z2 = z2;
    tw->along = 255;
    tw->wait = 0;

    return TRIP_wire_upto - 1;
}

//
// If (x,z) lies along the line this funciton returns the amount
// along the line that (x,z) is in 8-bit fixed point, otherwise
// it returns TRIP_NOT_COLLIDED.
//

#define TRIP_NOT_COLLIDED (-1)

std::int32_t TRIP_collide(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t x,
    std::int32_t z) {
    std::int32_t dx;
    std::int32_t dz;

    std::int32_t da;
    std::int32_t db;

    std::int32_t len;
    std::int32_t dist;
    std::int32_t along;

    std::int32_t dprod;
    std::int32_t cprod;

    dx = x2 - x1;
    dz = z2 - z1;

    da = x - x1;
    db = z - z1;

    cprod = da * dz - db * dx;
    len = QDIST2(abs(dx), abs(dz)) + 1;
    dist = cprod / len;

    if (abs(dist) < 0x20) {
        dprod = da * dx + db * dz;
        along = dprod / len;

        //
        // 8-bit fixed point...
        //

        along = (along * 256) / len;

        if (WITHIN(along, 0, 256)) {
            return along;
        }
    }

    return TRIP_NOT_COLLIDED;
}

void TRIP_process() {
    std::int32_t i;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t lfx;
    std::int32_t lfy;
    std::int32_t lfz;

    std::int32_t rfx;
    std::int32_t rfy;
    std::int32_t rfz;

    std::int32_t hx;
    std::int32_t hy;
    std::int32_t hz;

    std::int32_t tx;
    std::int32_t tz;

    std::int32_t dist;
    std::int32_t along;
    std::int32_t feet;
    std::int32_t head;

    TRIP_Wire *tw;
    Thing *darci = NET_PERSON(0);

    for (i = 1; i < TRIP_wire_upto; i++) {
        tw = &TRIP_wire[i];

        if (tw->x1 == TRIP_X1_DEACTIVATED) {
            //
            // This magic number means the trip wire is deactivated
            //

            continue;
        }

        tw->counter += 50;

        if (tw->wait) {
            ASSERT(tw->along != 255);

            //
            // The tripwire is triggered- don't detrigger for a while!
            //

            tw->wait -= 1;

            continue;
        }

        tw->along = 255;

        dx = (darci->WorldPos.X >> 8) - tw->x1;
        dz = (darci->WorldPos.Z >> 8) - tw->z1;

        dist = abs(dx) + abs(dz);

        if (dist > 0x500) {
            //
            // Too far away to be worth even bothering about.
            //

            continue;
        } else {
            //
            // Take Darci's height from her foot.
            //

            calc_sub_objects_position(
                darci,
                darci->Draw.Tweened->AnimTween,
                SUB_OBJECT_LEFT_FOOT,
                &lfx,
                &lfy,
                &lfz);

            calc_sub_objects_position(
                darci,
                darci->Draw.Tweened->AnimTween,
                SUB_OBJECT_RIGHT_FOOT,
                &rfx,
                &rfy,
                &rfz);

            feet = std::min(lfy, rfy);
            feet += darci->WorldPos.Y >> 8;

            calc_sub_objects_position(
                darci,
                darci->Draw.Tweened->AnimTween,
                SUB_OBJECT_HEAD,
                &hx,
                &hy,
                &hz);

            head = hy;
            head += darci->WorldPos.Y >> 8;

            if (head < feet) {
                //
                // Darci can do a summersault
                //

                SWAP(head, feet);
            }

            //
            // Has darci set off the trip-wire?
            //

            if (!WITHIN(tw->y, feet - 0x10, head + 0x10)) {
                //
                // Out of y-range.
                //

                continue;
            }

            along = TRIP_collide(
                tw->x1, tw->z1,
                tw->x2, tw->z2,
                darci->WorldPos.X >> 8,
                darci->WorldPos.Z >> 8);

            if (along != TRIP_NOT_COLLIDED) {
                if (along == 255) {
                    along = 254;
                }

                tw->along = along;
                tw->wait = 4;
            }
        }
    }
}

std::int32_t TRIP_get_upto;
TRIP_Info TRIP_get_info;

void TRIP_get_start() {
    TRIP_get_upto = 1;
}

TRIP_Info *TRIP_get_next() {
    TRIP_Wire *tw;

tail_recurse:;

    if (TRIP_get_upto >= TRIP_wire_upto) {
        return NULL;
    }

    ASSERT(WITHIN(TRIP_get_upto, 0, TRIP_wire_upto - 1));

    tw = &TRIP_wire[TRIP_get_upto++];

    if (tw->x1 == TRIP_X1_DEACTIVATED) {
        //
        // This magic number means the trip wire is deactivated
        //

        goto tail_recurse;
    }

    TRIP_get_info.y = tw->y;
    TRIP_get_info.x1 = tw->x1;
    TRIP_get_info.z1 = tw->z1;
    TRIP_get_info.x2 = tw->x2;
    TRIP_get_info.z2 = tw->z2;
    TRIP_get_info.counter = tw->counter;
    TRIP_get_info.along = tw->along;

    return &TRIP_get_info;
}

std::int32_t TRIP_activated(std::uint8_t tripwire) {
    TRIP_Wire *tw;

    ASSERT(WITHIN(tripwire, 0, TRIP_wire_upto - 1));

    tw = &TRIP_wire[tripwire];

    return (tw->along != 255);
}

void TRIP_deactivate(std::uint8_t tripwire) {
    TRIP_Wire *tw;

    ASSERT(WITHIN(tripwire, 0, TRIP_wire_upto - 1));

    tw = &TRIP_wire[tripwire];

    tw->x1 = TRIP_X1_DEACTIVATED;
}
