//
// Bits of dirt that get blown around. Dirt only exists
// around one focal point (the camera). If a bit of dirt gets
// too far away, then it teleports to somewhere on the
// edge on the focus point.
//

#ifndef DIRT_H
#define DIRT_H

#define DIRT_TYPE_UNUSED 0
#define DIRT_TYPE_LEAF 1
#define DIRT_TYPE_CAN 2
#define DIRT_TYPE_PIGEON 3
#define DIRT_TYPE_WATER 4
#define DIRT_TYPE_HELDCAN 5
#define DIRT_TYPE_THROWCAN 6
#define DIRT_TYPE_HEAD 7
#define DIRT_TYPE_HELDHEAD 8
#define DIRT_TYPE_THROWHEAD 9
#define DIRT_TYPE_BRASS 10 // 10 was grenade and is now brass (ejected shells)
#define DIRT_TYPE_MINE 11
#define DIRT_TYPE_URINE 12
#define DIRT_TYPE_SPARKS 13
#define DIRT_TYPE_BLOOD 14
#define DIRT_TYPE_SNOW 15
#define DIRT_TYPE_NUMBER 16

typedef struct
{
    std::uint8_t type;
    std::uint8_t owner;
    std::uint8_t flag;
    std::uint8_t counter;

    union {
        struct
        {
            std::uint8_t state; // For pigeons only....
            std::uint8_t morph1;
            std::uint8_t morph2;
            std::uint8_t tween;
        } Pidgeon;
        struct
        {
            std::uint16_t col;
            std::uint16_t fade; // Adding this shouldn't affect the struct size as Pidgeon is already larger...
        } Leaf;

        struct
        {
            std::uint16_t prim; // Which prim to use as the prim object.

        } Head;

        struct
        {
            std::uint16_t timer; // How long before we explode?

        } ThingWithTime;
    } UU;

    std::int16_t dyaw;
    std::int16_t dpitch;
    std::int16_t droll;

    std::int16_t yaw;
    std::int16_t pitch;
    std::int16_t roll;

    std::int16_t x;
    std::int16_t y;
    std::int16_t z;

    std::int16_t dx;
    std::int16_t dy; // dy is now shifted up TICK_SHIFT bits to allow for frame-rate independent gravity ;)
    std::int16_t dz;

} DIRT_Dirt;

//
// Removes all dirt.
//

void DIRT_init(
    std::int32_t prob_leaf, // Relative probabilites of each bit of dirt
    std::int32_t prob_can,
    std::int32_t prob_pigeon,
    std::int32_t pigeon_map_x1, // The bounding box in which only pigeons are made.
    std::int32_t pigeon_map_z1,
    std::int32_t pigeon_map_x2,
    std::int32_t pigeon_map_z2);

//
// Sets the area of focus. If a bit of dirt is out of
// range then it dissapears and reappears somewhere on
// the edge of the focal range.
//

void DIRT_set_focus(
    std::int32_t x,
    std::int32_t z,
    std::int32_t radius);

//
// Makes the dirt react to a gust of wind. The gust happens
// at (x1,z1).  (x2,z2) gives strength and direction.
//

void DIRT_gust(
    Thing *p_thing, // The thing that caused the gust.
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2);

//
// Creates a wind blowing through the whole city.
//

void DIRT_gale(
    std::int32_t dx,
    std::int32_t dz);

//
// Creates a bit of water dirt.
//

void DIRT_new_water(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t dx,
    std::int32_t dy,
    std::int32_t dz,
    std::int32_t dirt_type = DIRT_TYPE_WATER);

//
// Process the dirt.
//

void DIRT_process();

//
// Returns the distance of the nearest coke can. Returns INFINITY
// if it couldn't find a coke can.
//

std::int32_t DIRT_get_nearest_can_or_head_dist(std::int32_t x, std::int32_t y, std::int32_t z);

//
// Looks for the nearest coke can or head to the given person and attaches
// it to their right hand.
//

void DIRT_pick_up_can_or_head(Thing *p_person);

//
// Releases the given held coke can or head with the given power.
//

void DIRT_release_can_or_head(Thing *p_person, std::int32_t power); // 0 <= power <= 256

//
// Beheads the given person. It takes the velocity from the position of
// the attacker.  If attacker == nullptr, then a random velocity is given.
//

void DIRT_behead_person(Thing *p_person, Thing *p_attacker);

//
// Creates a grenade thrown by the person with the given velocity and time
// to detonation.  The ticks of the time to detonation is 16*20 ticks per second.
//

void DIRT_create_grenade(Thing *p_person, std::int32_t ticks_to_go, std::int32_t power);

//
// Creates/Destroys a mine.
//

std::uint16_t DIRT_create_mine(Thing *p_person);
void DIRT_destroy_mine(std::uint16_t dirt_mine);

//
// This person is trying to shoot a bit of dirt. It returns true if
// the person shot anything.
//

std::int32_t DIRT_shoot(Thing *p_person);

//
// Makes a few newspapers and crisp packets appear and float around
// at the given spot.
//

void DIRT_create_papers(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z);

void DIRT_create_cans(
    std::int32_t x,
    std::int32_t z,
    std::int32_t angle);

//
// The dirt module will tell you about each bit of dirt
// and how to draw it.
//

#ifdef PSX
#define DIRT_MAX_DIRT (128) // MAKE SURE ITS A POWER OF 2!
#else
#ifdef TARGET_DC
#define DIRT_MAX_DIRT (256) // MAKE SURE ITS A POWER OF 2!
#else
#define DIRT_MAX_DIRT (1024) // MAKE SURE ITS A POWER OF 2!
#endif
#endif

extern DIRT_Dirt DIRT_dirt[DIRT_MAX_DIRT];

#define DIRT_INFO_TYPE_UNUSED 0
#define DIRT_INFO_TYPE_LEAF 1
#define DIRT_INFO_TYPE_PRIM 2
#define DIRT_INFO_TYPE_MORPH 3
#define DIRT_INFO_TYPE_WATER 4
#define DIRT_INFO_TYPE_URINE 5
#define DIRT_INFO_TYPE_SPARKS 6
#define DIRT_INFO_TYPE_BLOOD 7
#define DIRT_INFO_TYPE_SNOW 8

#define DIRT_FLAG_STILL (1 << 0)
#define DIRT_FLAG_HIT_FLOOR (1 << 1)
#define DIRT_FLAG_DELETE_OK (1 << 2)

typedef struct
{
    std::uint8_t type;
    std::uint8_t held; // For PRIM types. This prim is being held.
    std::uint8_t morph1;
    std::uint8_t morph2;
    std::uint16_t prim;
    std::uint16_t tween; // used as a colour for leafs
    std::uint16_t yaw;
    std::uint16_t pitch;
    std::uint16_t roll;
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

} DIRT_Info;

std::int32_t DIRT_get_info(std::int32_t which, DIRT_Info *ans);

//
// Tell the dirt module when a bit of dirt is off-screen.
// This helps the dirt module know which bits of dirt
// to delete.
//

void DIRT_mark_as_offscreen(std::int32_t which);

#define DIRT_MARK_AS_OFFSCREEN_QUICK(which)           \
    {                                                 \
        DIRT_dirt[which].flag |= DIRT_FLAG_DELETE_OK; \
    }

void DIRT_new_sparks(std::int32_t px, std::int32_t py, std::int32_t pz, std::uint8_t dir);

#endif
