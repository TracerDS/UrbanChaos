//
// Furniture on the streets and inside houses.
//

#ifndef FURN_H
#define FURN_H

#define FLAG_FURN_DRIVING (1 << 0)
#define FLAG_FURN_WHEEL1_GRIP (1 << 1)
#define FLAG_FURN_WHEEL2_GRIP (1 << 2)
#define FLAG_FURN_WHEEL3_GRIP (1 << 3)
#define FLAG_FURN_WHEEL4_GRIP (1 << 4)

//
// The furniture structure. A static piece of furniture does not have
// a structure associated with it. (The pointer in the Thing structure
// is nullptr). If the furniture starts moving, however, it allocates one
// of these structures and decallocates it once it stops.
//

typedef struct
{
    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;
    std::int32_t dyaw;
    std::int32_t dpitch;
    std::int32_t droll;

    //
    // temp stuff for cars...

    //
    std::int16_t Wheel; // steering wheel position
    std::int16_t RAngle;
    std::int16_t OverSteer;
    std::int16_t DeltaOverSteer;
    std::int16_t Compression[4]; // suspension extension *4
    std::int16_t SpringDY[4];
    std::uint16_t Flags;
    std::uint16_t Driver;

    //
    // Command system stuff.
    //

    std::uint16_t Command;
    std::uint16_t Waypoint;

    //
    // Temp stuff for doors...
    //

    std::uint16_t closed_angle;
    std::uint16_t ajar;
} Furniture;

typedef Furniture *FurniturePtr;
#define MAX_FURNITURE 10

//
// The furniture state functions... there is only one type
// of furniture.
//

extern StateFunction FURN_statefunctions[];

//
// Initialises the furniture structures.
// Clears a furniture thing.
//

void init_furniture();
void free_furniture(Thing *furniture_thing);
Furniture *FURN_alloc_furniture();

//
// Creates the furniture thing and puts it on the mapwho in
// a stationary position.
//

THING_INDEX FURN_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t yaw,
    std::int32_t pitch,
    std::int32_t roll,
    std::int32_t prim);

THING_INDEX VEHICLE_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t angle,
    std::int32_t prim);

//
// Turns a normal furniture thing into a door.
//

void FURN_turn_into_door(
    THING_INDEX furniture_thing,
    std::uint16_t closed_angle,
    std::uint16_t ajar,
    std::uint8_t am_i_locked);

//
// Slides a movement vector of someone with the given radius
// against the bounding box of the fucniture.  It returns
// where you should slide to.  If 'dont_slide', then if the
// vector intersects with the box, instead of sliding along it,
// it zeros out the movement vector. It returns true in this
// case.
//
// It assumes that the pitch of the furniture is zero!!!
//

std::int32_t FURN_slide_along(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2,
    std::int32_t radius,
    std::int32_t dont_slide);

//
// Checks a movement vector against a piece of furniture and
// returns a change in angle so make the moving thing avoid
// the furniture. (Either +1 or -1).
//

std::int32_t FURN_avoid(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2);

//
// Starts modelling the furniture with hypermatter.
//

void FURN_hypermatterise(THING_INDEX thing);

//
// Applies the force to the given piece of furniture.
//

void FURN_push(
    THING_INDEX thing,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2);

//
// For each furniture thing, scan for walkable faces, copy them,
// then index them to the thing.
//

void FURN_add_walkable();

#endif
