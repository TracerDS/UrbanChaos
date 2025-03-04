//
// Tells you where to put staircases in a building.
//

#ifndef STAIR_H
#define STAIR_H


#include "id.h"


//
// Clear all the current building info.
//

void STAIR_init();

//
// Tell it about the bounding box of the new building. The bounding box
// is that of the walls, so for a 1-square building the bounding box
// should be (0,0)-(1,1).
//

void STAIR_set_bounding_box(std::uint8_t x1, std::uint8_t z1, std::uint8_t x2, std::uint8_t z2);

//
// Tell it about all the storeys of the building.
// When adding a wall, if (opposite) then the stair module tries to
// put staircases opposite that wall but not touching it. It only
// uses the last opposite wall it is given.
//
// STAIR_storey_finish() returns false if the walls added for the storey
// are invalid and you can't create stairs for this building.
//

void STAIR_storey_new   (std::int32_t handle, std::uint8_t height);				// 0 => Ground floor, 1 => First floor
void STAIR_storey_wall  (std::uint8_t x1, std::uint8_t z1, std::uint8_t x2, std::uint8_t z2, std::int32_t opposite);
std::int32_t STAIR_storey_finish();

//
// Calculates where the stairs should go for each storey.
//

void STAIR_calculate(std::uint16_t seed);

//
// Returns the stairs for the given floor.  The 'id' of the ID_stair is
// not filled in.  Returns false if it couldn't find a storey with
// the given handle.
//

std::int32_t STAIR_get(std::int32_t handle, ID_Stair **stair, std::int32_t *num_stairs);



#endif
