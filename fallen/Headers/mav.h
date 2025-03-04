//
// Marks navigation system.
//

#ifndef _MAV_
#define _MAV_


#include "pap.h"
#include "structs.h"


//
// This array is indexed by the MAV_nav[] array
//

typedef struct
{
	std::uint8_t opt[4];	// The options for moving in each direction.

} MAV_Opt;

#define MAV_MAX_OPTS 1024	// don't change this!!!

extern MAV_Opt *MAV_opt;
extern std::int32_t    MAV_opt_upto;

//
// How you can move out of each square.  This is a 2D array
// whose pitch is given by MAV_nav_pitch;
//

extern std::uint16_t *MAV_nav;
extern std::int32_t  MAV_nav_pitch;

#define MAV_SPARE_FLAG_WATER  (1 << 0)	// The first spare bit
#define MAV_SPARE_FLAG_UNUSED (1 << 1)	// The second spare bit

#define MAV_NAV(x,z)			(MAV_nav[((x) * MAV_nav_pitch) + (z)] & 1023)			// 10 bits
#define MAV_CAR(x,z)			((MAV_nav[((x) * MAV_nav_pitch) + (z)] >> 10) & 15)		// 4 bits
#define MAV_SPARE(x,z)			(MAV_nav[((x) * MAV_nav_pitch) + (z)] >> 14)			// 2 bits
#define SET_MAV_NAV(x,z,v)		MAV_nav[((x) * MAV_nav_pitch) + (z)] = (MAV_nav[((x) * MAV_nav_pitch) + (z)] & 0xFC00) | ((v) & 1023)
#define SET_MAV_CAR(x,z,v)		MAV_nav[((x) * MAV_nav_pitch) + (z)] = (MAV_nav[((x) * MAV_nav_pitch) + (z)] & 0xC3FF) | (((v) & 15) << 10)
#define SET_MAV_SPARE(x,z,v)	MAV_nav[((x) * MAV_nav_pitch) + (z)] = (MAV_nav[((x) * MAV_nav_pitch) + (z)] & 0x3FFF) | ((v) << 14)

#define MAV_CAR_GOTO(x,z,d)		(!!(MAV_CAR(x,z) & (1 << d)))

#define	MAVHEIGHT(x,z)			(PAP_hi[x][z].Height)

//
// A std::uint8_t of height for each mapsquare.
// The height is in quarter blocks.
//

//typedef	std::int8_t MAV_height_workaround[PAP_SIZE_HI];
//extern MAV_height_workaround *MAV_height;


//
// Call this function first.
//

void MAV_init();



// ========================================================
//
// TO CALCULATE THE MAV ARRAYS FOR INSIDE WAREHOUSES
//
// ========================================================

//
// First, work out the MAV_height array without the warehouses.
//

void MAV_calc_height_array(std::int32_t ignore_warehouses);

//
// You must have set the bounding box and the
// mav_pitch and mav fields of the warehouse properly.
//

void MAV_precalculate_warehouse_nav(std::uint8_t ware);	// Index into the WARE_ware array.



//
// Works out everything for the current map. The MAV_height array and the
// MAV_nav array.
//

void MAV_precalculate();



//
// Returns what someone should do next in order to get somewhere.
//

#define MAV_ACTION_GOTO			0
#define MAV_ACTION_JUMP			1		// Jump one block.
#define MAV_ACTION_JUMPPULL		2		// Jump one block by pulling yourself up
#define MAV_ACTION_JUMPPULL2	3		// Jump two blocks by pulling yourself up.
#define MAV_ACTION_PULLUP		4
#define MAV_ACTION_CLIMB_OVER	5
#define MAV_ACTION_FALL_OFF		6
#define MAV_ACTION_LADDER_UP	7

#define MAV_DIR_XS	0
#define MAV_DIR_XL	1
#define MAV_DIR_ZS	2
#define MAV_DIR_ZL	3

#define MAV_CAPS_GOTO		(1 << MAV_ACTION_GOTO)
#define MAV_CAPS_JUMP		(1 << MAV_ACTION_JUMP)
#define MAV_CAPS_JUMPPULL	(1 << MAV_ACTION_JUMPPULL)
#define MAV_CAPS_JUMPPULL2	(1 << MAV_ACTION_JUMPPULL2)
#define MAV_CAPS_PULLUP		(1 << MAV_ACTION_PULLUP)
#define MAV_CAPS_CLIMB_OVER	(1 << MAV_ACTION_CLIMB_OVER)
#define MAV_CAPS_FALL_OFF	(1 << MAV_ACTION_FALL_OFF)
#define MAV_CAPS_LADDER_UP	(1 << MAV_ACTION_LADDER_UP)

#define MAV_CAPS_DARCI (0xff)	// She can do everything.

MAV_Action MAV_do(
			std::int32_t me_x,		// 0-bit fixed point- these are mapsquares.
			std::int32_t me_z,
			std::int32_t dest_x,	// 0-bit fixed point- these are mapsquares.
			std::int32_t dest_z,
			std::uint8_t caps);	// OR together all the thing you can do

//
// After calling MAV_do() if this value is true, then the call found
// a route all the way to the destination. If this value is false there
// still might be a way to get there, but if it is true there definitely is.
//

extern std::uint8_t MAV_do_found_dest;

//
// Using the MAV_height array, this function returns true if the given point
// is underground or within a building.
// 

std::int32_t MAV_inside(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z);

//
// Returns the caps for going from the square in the given direction.
//

std::uint8_t MAV_get_caps(
		std::uint8_t x,
		std::uint8_t z,
		std::uint8_t dir);


#ifndef TARGET_DC
//
// Draw the MAV_info in the given squares (these are mapsquares... fixed point 0).
//

void MAV_draw(
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);
#endif //#ifndef TARGET_DC


//
// A crude los function that only uses the MAV_height array. Coordinates are
// world coordinates given with 8-bits per mapsquare.
//

extern std::int32_t MAV_height_los_fail_x;
extern std::int32_t MAV_height_los_fail_y;
extern std::int32_t MAV_height_los_fail_z;

std::int32_t MAV_height_los_fast(
		std::int32_t x1, std::int32_t y1, std::int32_t z1,
		std::int32_t x2, std::int32_t y2, std::int32_t z2);

std::int32_t MAV_height_los_slow(
		std::int32_t ware,
		std::int32_t x1, std::int32_t y1, std::int32_t z1,
		std::int32_t x2, std::int32_t y2, std::int32_t z2);


//
// For changing NAV info on the fly... rather dangerous.
// 

void MAV_turn_movement_on (std::uint8_t mx, std::uint8_t mz, std::uint8_t dir);
void MAV_turn_movement_off(std::uint8_t mx, std::uint8_t mz, std::uint8_t dir);

//
// For changing car MAV info on the fly... not dangerous at all.
//

void MAV_turn_car_movement_on (std::uint8_t mx, std::uint8_t mz, std::uint8_t dir);
void MAV_turn_car_movement_off(std::uint8_t mx, std::uint8_t mz, std::uint8_t dir);


#endif
