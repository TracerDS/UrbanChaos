// Thing.h
// Guy Simmons, 15th October 1997.

#ifndef THING_H
#define THING_H
#include "../Headers/Game.h"
#include "../Headers/drawtype.h"
#include "../Headers/vehicle.h"

//---------------------------------------------------------------
/*
#define	MAX_PRIMARY_THINGS		MAX_PLAYERS		+		\
                                                                MAX_PROJECTILES	+		\
                                                                MAX_PEOPLE		+		\
                                                                MAX_FURNITURE	+		\
                                                                MAX_SPECIALS	+		\
                                                                BAT_MAX_BATS	+		\
                                                                100
#define	MAX_SECONDARY_THINGS	MAX_SWITCHES	+		\
                                                                TRACK_BUFFER_LENGTH +   \
                                                                1
*/

#define MAX_PRIMARY_THINGS (400)
#ifndef BUILD_PSX
#define MAX_SECONDARY_THINGS (300)
#else
#define MAX_SECONDARY_THINGS (50)
#endif

#define MAX_THINGS (MAX_PRIMARY_THINGS + MAX_SECONDARY_THINGS + 1)

#define FLAGS_ON_MAPWHO (1 << 0)
#define FLAGS_IN_VIEW (1 << 1)
#define FLAGS_TWEEN_TO_QUEUED_FRAME (1 << 2)
#define FLAGS_PROJECTILE_MOVEMENT (1 << 3)
#define FLAGS_LOCKED_ANIM (1 << 4)
#define FLAGS_IN_BUILDING (1 << 5)
#define FLAGS_INDOORS_GENERATED (1 << 6)
#define FLAGS_LOCKED (1 << 7)         // A locked door...
#define FLAGS_CABLE_BUILDING (1 << 8) // A building thing that is a cable.
#define FLAGS_COLLIDED (1 << 9)       // For a vehicle that collided with a wall last frame.
#define FLAGS_IN_SEWERS (1 << 10)
#define FLAGS_SWITCHED_ON (1 << 11)
#define FLAGS_PLAYED_FOOTSTEP (1 << 12)
#define FLAGS_HAS_GUN (1 << 13)
#define FLAGS_BURNING (1 << 14)            // flag burning: illegal in some countries
#define FLAGS_HAS_ATTACHED_SOUND (1 << 15) // a sound fx is "attached" to this thing

#define FLAGS_PERSON_BEEN_SHOT (1 << 16)

#define FLAGS_PERSON_AIM_AND_RUN (1 << 6)

#define FLAG_EDIT_PRIM_ON_FLOOR (1 << 2)
#define FLAG_EDIT_PRIM_JUST_FLOOR (1 << 3)
#define FLAG_EDIT_PRIM_INSIDE (1 << 4)

#define FLAG_SPECIAL_UNTOUCHED (1 << 2)
#define FLAG_SPECIAL_HIDDEN (1 << 3)

//---------------------------------------------------------------

struct Thing {
    std::uint8_t Class,
        State,
        OldState,
        SubState;
    std::uint32_t Flags;
    THING_INDEX Child,
        Parent,
        LinkChild,
        LinkParent;

    GameCoord WorldPos;

    void (*StateFn)(Thing *); // Things state function.

    union {
        DrawTween *Tweened;
        DrawMesh *Mesh;
    } Draw;

    union {
        Vehicle* Vehicle;
        Furniture* Furniture;
        Person* Person;
        Animal* Animal;
        Chopper* Chopper;
        Pyro* Pyro;
        Projectile* Projectile;
        Player* Player;
        Special* Special;
        Switch* Switch;
        Track* Track;
        Plat* Plat;
        Barrel* Barrel;
#ifdef BIKE
        Bike* Bike;
#endif
        Bat* Bat;

    } Genus;
    std::uint8_t DrawType;
    std::uint8_t Lead; // 42

    // TEMP STUFF.
    std::int16_t Velocity; //
    std::int16_t DeltaVelocity;

    std::int16_t RequiredVelocity;
    std::int16_t DY;

    std::uint16_t Index;
    std::int16_t OnFace;

    std::uint16_t NextLink; // BuildingList;	// For a building thing...
    std::uint16_t DogPoo1;  // Timer1;

    THING_INDEX DogPoo2; // SwitchThing;	//	Temporary for building unlock switches.
};

typedef struct Thing Thing;

extern std::uint16_t *thing_class_head;

//---------------------------------------------------------------

void init_things();
THING_INDEX alloc_primary_thing(std::uint16_t thing_class);
void free_primary_thing(THING_INDEX thing);
THING_INDEX alloc_secondary_thing(std::uint16_t secondary_thing);
void free_secondary_thing(THING_INDEX thing);
void add_thing_to_map(Thing *t_thing);
void remove_thing_from_map(Thing *t_thing);
void move_thing_on_map(Thing *t_thing, GameCoord *new_position);
void process_things(std::int32_t f_r_i);

void log_primary_used_list();
void log_primary_unused_list();

Thing *alloc_thing(std::int8_t classification);
void free_thing(Thing *t_thing);

Thing *nearest_class(Thing *the_thing, std::uint32_t class_mask, std::uint32_t *closest);

inline void set_thing_pos(Thing *t, std::int32_t x, std::int32_t y, std::int32_t z) {
    t->WorldPos.X = x;
    t->WorldPos.Y = y;
    t->WorldPos.Z = z;
}

//
// Returns the distance between the two things.
// This is in 8-bits per mapsquares and approximate.
//

std::int32_t THING_dist_between(Thing *p_thing_a, Thing *p_thing_b);

//
// Removes a thing from the map and the game. It clears up all the memory
// it is supposed to aswell.
//

void THING_kill(Thing *t);

//
// A handy array for finding things with...
//

#define THING_ARRAY_SIZE 32

extern THING_INDEX THING_array[THING_ARRAY_SIZE];

//
// Fills an array with all the things in the given bounding sphere.
// It returns the number of things found.  'Classes' gives which classes
// of thing to include and which to ignore.  EG if you want to only find
// projectiles, pass (1 << CLASS_PROJECTILE), there is a bit for each
// class type.
//
// Returns the number of things found and put into the array.
//

#define THING_FIND_EVERYTHING (0xffffffff)
#define THING_FIND_PEOPLE (1 << CLASS_PERSON)
#define THING_FIND_LIVING ((1 << CLASS_PERSON) | (1 << CLASS_ANIMAL))
#define THING_FIND_MOVING ((1 << CLASS_PERSON) | (1 << CLASS_ANIMAL) | (1 << CLASS_PROJECTILE))

std::int32_t THING_find_sphere(
    std::int32_t centre_x, // 50 << 8
    std::int32_t centre_y, //  0
    std::int32_t centre_z, // 50 << 8
    std::int32_t radius,
    THING_INDEX *array,
    std::int32_t array_size,
    std::uint32_t classes);

std::int32_t THING_find_box(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    THING_INDEX *array,
    std::int32_t array_size,
    std::uint32_t classes);

//
// Finds the nearest thing of the given class. Returns nullptr on failure.
//

std::int32_t THING_find_nearest(
    std::int32_t centre_x,
    std::int32_t centre_y,
    std::int32_t centre_z,
    std::int32_t radius,
    std::uint32_t classes);

//---------------------------------------------------------------

#endif
