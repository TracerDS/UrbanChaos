//
// Objects (prims) on the map.
//

#ifndef _OB_
#define _OB_

#include "pap.h"

// defines

#define OB_MAX_OBS 2048
#define OB_SIZE PAP_SIZE_LO
#define OB_MAX_PER_SQUARE 31

#define OB_FLAG_ON_FLOOR (1 << 0)
#define OB_FLAG_SEARCHABLE (1 << 1)
// #define	OB_FLAG_INSIDE		(1<<2)	// This flag is deceased. It is an ex-flag.
#define OB_FLAG_NOT_ON_PSX (1 << 2)
#define OB_FLAG_DAMAGED (1 << 3)
#define OB_FLAG_WAREHOUSE (1 << 4) // This OB is inside a warehouse
#define OB_FLAG_HIDDEN_ITEM (1 << 5)

#define OB_FLAG_RESERVED1 (1 << 6) // These two flags mean special things for
#define OB_FLAG_RESERVED2 (1 << 7) // damaged prims.

// structures

typedef struct
{
    std::uint16_t index : 11;
    std::uint16_t num : 5;

} OB_Mapwho;

//
// Info about the objects in lo-res mapwho square (x,z). The array is
// terminated by the prim being zero.
//

typedef struct
{
    std::uint16_t prim; // 0 => No more info.
    std::uint16_t x;
    std::int16_t y;
    std::uint16_t z;
    std::uint16_t yaw;
    std::uint16_t pitch;
    std::uint16_t roll;
    std::uint16_t index;  // The index of this object
    std::uint8_t crumple; // How crumpled up this object is from 0 - 4 inclusive
    std::uint8_t InsideIndex;
    std::uint8_t Room;
    std::uint8_t flags;

} OB_Info;

//
// The objects.
//

typedef struct
{
    std::int16_t y;
    std::uint8_t x;
    std::uint8_t z;
    std::uint8_t prim;
    std::uint8_t yaw;
    std::uint8_t flags;
    std::uint8_t InsideIndex;

} OB_Ob;

//
// Data
//

typedef OB_Mapwho OB_workaround[OB_SIZE];
extern OB_workaround *OB_mapwho; //[OB_SIZE][OB_SIZE];
extern OB_Ob *OB_ob;             //[OB_MAX_OBS];
extern std::int32_t OB_ob_upto;

//
// Initialise all the furniture.
//

void OB_init();

//
// Loads the prims objects needed to render the OBjects.
//

void OB_load_needed_prims();

//
// Places some furniture.
//

void OB_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t yaw,
    std::int32_t pitch,
    std::int32_t roll,
    std::int32_t prim,
    std::uint8_t flag,
    std::uint16_t Inside,
    std::uint8_t Room);

//
// Returns an array of all the prims found on the lo-res mapsquare.
// The array is terminated with an object having a nullptr prim.
//

OB_Info *OB_find(std::int32_t lo_map_x, std::int32_t lo_map_z);
OB_Info *OB_find_inside(std::int32_t x, std::int32_t z, std::int32_t indoors);

//
// Finds the nearest object whose prim object contains one
// of the given flags.  Returns false if no object was found
// in the range.
//

#define FIND_OB_TRIPWIRE (1 << 9)
#define FIND_OB_SWITCH_OR_VALVE (1 << 10)

std::int32_t OB_find_type(
    std::int32_t mid_x,
    std::int32_t mid_y,
    std::int32_t mid_z,
    std::int32_t max_range,
    std::uint32_t prim_flags,
    std::int32_t *ob_x,
    std::int32_t *ob_y,
    std::int32_t *ob_z,
    std::int32_t *ob_yaw,
    std::int32_t *ob_prim,
    std::int32_t *ob_index);

//
// Removes the given OB_ob. Only the index and (x,z) of the
// OB_Info need be valid- or you can pass the OB_Info got
// back from a call to OB_Find.
//

void OB_remove(OB_Info *oi);

//
// Checks a movement vector against the given object
// returns a change in angle so make the moving thing avoid
// the furniture. (Either +1 or -1).
//

std::int32_t OB_avoid(
    std::int32_t ob_x,
    std::int32_t ob_y,
    std::int32_t ob_z,
    std::int32_t ob_yaw,
    std::int32_t ob_prim,
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2);

//
// Damages the prim due to a force from the given location.
//

void OB_damage(
    std::int32_t index,   // The index of this object,
    std::int32_t from_dx, // The position of the damaging thing relative to the object.
    std::int32_t from_dz,
    std::int32_t ob_x, // The position of the ob!
    std::int32_t ob_z,
    Entity *p_aggressor); // Who caused the damage or nullptr if you don't know

//
// Returns true if there is a prim at (x,y,z)
//

std::int32_t OB_inside_prim(std::int32_t x, std::int32_t y, std::int32_t z);

//
// Processes the objects. Not much happens here except that damaged fire
// hydrants spout water.
//

void OB_process();

//
// Converts all dustbin prims into barrels.
//

void OB_convert_dustbins_to_barrels();

//
// Makes sure that all the switches are the correct
// height above the ground.
//

void OB_make_all_the_switches_be_at_the_proper_height();

//
// Adds the object walkable faces.
//

void OB_add_walkable_faces();

std::int32_t OB_find_min_y(std::int32_t prim);
OB_Info *OB_find_index(std::int32_t mid_x, std::int32_t mid_y, std::int32_t mid_z, std::int32_t max_range, std::int32_t must_be_searchable);
// std::int32_t OB_find_index(std::int32_t  mid_x,std::int32_t  mid_y,std::int32_t  mid_z,std::int32_t  max_range);

#endif
