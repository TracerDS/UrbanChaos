//
// Platforms are moving prims.  You can walk on the bounding box of the
// walkable faces of the prim.
//

#pragma once

//
// The platforms linked from the things.
// 

struct Plat
{
	std::uint8_t used;
	std::uint8_t colour;
	std::uint8_t group;
	std::uint8_t move;

	std::uint8_t state;
	std::uint8_t wspeed;		// The speed you want to go at.
	std::uint8_t speed;		// The speed you are going at.
	std::uint8_t flag;
	std::uint16_t counter;		// In millisecs...
	std::uint16_t waypoint;

};


#define RPLAT_MAX_PLATS 32
#define	PLAT_MAX_PLATS		(save_table[SAVE_TABLE_PLATS].Maximum)

#define	TO_PLAT(x)		&PLAT_plat[x]
#define	PLAT_NUMBER(x)	(std::uint16_t)(x-TO_PLAT(0))

extern Plat  *PLAT_plat; //[PLAT_MAX_PLATS];

//
// So we can have a pointer from the thing structure.
//

typedef struct Plat *PlatPtr;

extern std::int32_t PLAT_plat_upto;

//
// Initialises all platform info.
//

void PLAT_init();


//
// Looks for the nearest ob to the given location. It removes that ob
// and in its place puts an identical platform.  Returns the THING_NUMBER
// of the new PLAT thing or nullptr if the call failed.
//

#define PLAT_MOVE_STILL         0
#define PLAT_MOVE_PATROL		1
#define PLAT_MOVE_PATROL_RAND	2

#define PLAT_FLAG_LOCK_MOVE	   (1 << 0)
#define PLAT_FLAG_LOCK_ROT     (1 << 1)
#define PLAT_FLAG_BODGE_ROCKET (1 << 2)

std::uint16_t PLAT_create(
		std::uint8_t colour,
		std::uint8_t group,
		std::uint8_t move,
		std::uint8_t flag,
		std::uint8_t speed,
		std::int32_t world_x,
		std::int32_t world_y,
		std::int32_t world_z);





