//
// Building related stuff
//

#ifndef IN_BUILDING_H
#define IN_BUILDING_H
#include "Game.h"


//
// Stories are identified by the height of their floor.
// The height is restricted to quarter-mapsquare boundaries... 0x40
//

//
// Says whether you can enter a building or not, if you can't
// it returns a nullptr building index, otherwise it returns the
// building index and where you should be inside the building.
//

typedef struct
{
	std::int32_t       dbuilding;	// The dbuilding index.
	std::uint8_t       map_x;
	std::uint8_t       map_z;

} ENTER_Okay;

ENTER_Okay ENTER_can_i(THING_INDEX me);


//
// Returns true if you can leave the building you are in.
// If so, it fills in map_x,map_z with where should be
// outside the building.
//

std::int32_t ENTER_leave(THING_INDEX me, std::uint8_t *map_x, std::uint8_t *map_z);


//
// Returns the height of the ground floor and the top storey of
// the given dbuilding.  The floors of the building are restricted
// to quarter mapsquare boundaries.
//

void ENTER_get_extents(
		std::int32_t  dbuilding,
		std::int32_t *height_ground_floor,
		std::int32_t *height_of_top_storey);


//
// Sets-up the ID module to represent the insides of the given storey
// of the given building thing.  The storey must be part of the building's
// storey linked list!  Returns true on sucess.
//
// If (find_best_layout) then the ENTER module asks the ID module to find
// the best layout it can starting with the given seed. It then puts the
// seed the ID module actually used into the dbuilding.SeedInside field.
//

std::int32_t ENTER_setup(std::int32_t dbuilding, std::int32_t height, std::uint8_t furnished, std::uint8_t find_best_layout);



#endif
