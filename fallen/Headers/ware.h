//
// Warehouses- and you can go into them!
//

#ifndef _WARE_
#define _WARE_

//
// Warehouses have their own private MAVigation system and they know
// where their entrances are.
//

//
// The different directions the warehouse doors can be.
//

#define WARE_DOOR_XS 0
#define WARE_DOOR_XL 1
#define WARE_DOOR_ZS 2
#define WARE_DOOR_ZL 3

#define WARE_MAX_DOORS 4

typedef struct
{
    struct
    {
        std::uint8_t out_x;
        std::uint8_t out_z;
        std::uint8_t in_x;
        std::uint8_t in_z;

    } door[WARE_MAX_DOORS]; // Upto four doors.
    std::uint8_t door_upto;

    std::uint8_t minx;
    std::uint8_t minz;
    std::uint8_t maxx; // Inclusive
    std::uint8_t maxz; // Inclusive

    std::uint8_t nav_pitch; // The pitch of the WARE_mav array
    std::uint16_t nav;      // Index into the WARE_mav array for this warehouse's MAVigation data.
    std::uint16_t building; // This building this warehouse is for.
    std::uint16_t height;   // Index into the WARE_height array for this warehouse's MAV_height data
    std::uint16_t rooftex;  // Index into the WARE_rooftex array for the roof-top textures of the warehouse
    std::uint8_t ambience;  // Sets the ambience sound to play while inside the warehouse
    std::uint8_t padding;   // Upto 32 bytes?

} WARE_Ware;

#define WARE_MAX_WARES 32

extern WARE_Ware *WARE_ware; //[WARE_MAX_WARES];
extern std::uint16_t WARE_ware_upto;

//
// The warehouse MAVigation data. This is where we store all the
// 2D MAV_nav arrays for the warehouses.
//

#define WARE_MAX_NAVS 4096

extern std::uint16_t *WARE_nav; //[WARE_MAX_NAVS];
extern std::uint16_t WARE_nav_upto;

//
// The warehouses MAV_height data.
//

#define WARE_MAX_HEIGHTS 8192

extern std::int8_t *WARE_height; //[WARE_MAX_HEIGHTS];
extern std::uint16_t WARE_height_upto;

//
// The rooftop textures
//

#define WARE_MAX_ROOFTEXES 4096

extern std::uint16_t *WARE_rooftex; //[WARE_MAX_ROOFTEXES];
extern std::uint16_t WARE_rooftex_upto;

//
// Set to the index of the warehouse if we are inside a warehouse or
// nullptr if we are not.
//

extern std::uint8_t WARE_in;

//
// Goes through the map looking for warehouses. When it finds one it creates a new
// WARE_ware structure and makes a link to it from that building.  i.e. it calculates
// MAV info for that building and works out where its doors are.  It also sets the
// PAP_LO_FLAG_WAREHOUSE for all PAP_lo squares with square inside warehouses.
//
// CALL THIS FUNCTION BEFORE YOU CALL MAV_precalculate!
//

void WARE_init();

//
// Returns true if the mapsquare is contained within the warehouse. Beware! There are
// cases when this function will get it wrong... but it isn't often!
//

std::int32_t WARE_in_floorplan(std::uint8_t ware, std::uint8_t x, std::uint8_t z);

//
// Returns which warehouse contains the given mapsqure or nullptr if no warehouse does.
//

std::int32_t WARE_which_contains(std::uint8_t x, std::uint8_t z);

//
// Returns whether the given (x,y,z) is 'underground' inside the warehouse. i.e. if
// it inside a crate. If returns true if the point is outside the warehouse aswell.
//

std::int32_t WARE_inside(std::uint8_t ware, std::int32_t x, std::int32_t y, std::int32_t z);

//
// Returns the caps for going from the square in the given direction.
//

std::uint8_t WARE_get_caps(
    std::uint8_t ware,
    std::uint8_t x,
    std::uint8_t z,
    std::uint8_t dir);

//
// Returns the height at the given location inside a warehouse.
//

std::int32_t WARE_calc_height_at(std::uint8_t ware, std::int32_t x, std::int32_t z);

//
// Sets up the game to be inside the given building. It sets WARE_in and invalidates
// all the cached lighting.
//

void WARE_enter(std::int32_t building);
void WARE_exit();

//
// Does a mavigation for a person to enter the given warehouse.
// Does a mavigation for a person inside a warehouse.
// Does a mavigation for a person to exit the warehouse.
//

MAV_Action WARE_mav_enter(Thing *p_person, std::uint8_t ware, std::uint8_t caps);
MAV_Action WARE_mav_inside(Thing *p_person, std::uint8_t dest_x, std::uint8_t dest_z, std::uint8_t caps);
MAV_Action WARE_mav_exit(Thing *p_person, std::uint8_t caps);

//
// Draws debug info for the warehouses.
//

void WARE_debug();

#endif
