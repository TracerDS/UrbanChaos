//
// Doors
//

#pragma once



//
// Doors in the process of opening or closing.
//

typedef struct
{
	std::uint16_t facet;	// nullptr => Unused.

} DOOR_Door;

#define DOOR_MAX_DOORS 4

extern DOOR_Door *DOOR_door;//[DOOR_MAX_DOORS];



//
// Opens\shuts a door near to the given position.
//

void DOOR_open(std::int32_t world_x, std::int32_t world_z);
void DOOR_shut(std::int32_t world_x, std::int32_t world_z);



//
// Processes the doors.
//

void DOOR_process();




