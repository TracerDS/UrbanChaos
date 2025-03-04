#pragma once



#include "structs.h"



//
// defines
//

#define	MAX_INSIDE_RECT	2000
#define	MAX_INSIDE_MEM	64000
#define	MAX_INSIDE_STAIRS	(MAX_INSIDE_RECT*4)

#define	START_PAGE_FOR_FLOOR	8



#define	FLAG_DOOR_LEFT	(1<<4)
#define	FLAG_DOOR_UP	(1<<5)
#define	FLAG_INSIDE_STAIR_UP (1<<6)
#define	FLAG_INSIDE_STAIR_DOWN (1<<7)
#define	FLAG_INSIDE_STAIR (3<<6)

//
//structures
//


struct	InsideStorey
{
	std::uint8_t	MinX;           // bounding rectangle of floor
	std::uint8_t	MinZ;
	std::uint8_t	MaxX;
	std::uint8_t	MaxZ;
	std::uint16_t	InsideBlock;    // index into inside_block (block of data of size bounding rect) data is room numbers 1..15 top 4 bits reserved
	std::uint16_t	StairCaseHead;  // link list of stair structures for this floor
	std::uint16_t	TexType;		// Inside style to use for floor
	std::uint16_t	FacetStart;     // index into facets that make up this building
	std::uint16_t	FacetEnd;		// Facet After last used facet for inside the floor
	std::int16_t	StoreyY;	    // Y co-ord could come in handy
	std::uint16_t	Building;
	std::uint16_t	Dummy[2];
};

struct	Staircase
{
	std::uint8_t	X,Z;         // pos of staircase
	std::uint8_t	Flags;       // flags for direction + up or down or both
	std::uint8_t	ID;          // padding
	std::int16_t	NextStairs;  // link to next stair structure
	std::int16_t	DownInside;  // link to next insidestorey for going downstairs
	std::int16_t	UpInside;  	 // link to next InsideStorey for going upstairs
};







//
//Data
//

extern std::uint8_t	slide_inside_stair;



//extern struct 	DInsideRect	inside_rect[MAX_INSIDE_RECT];

extern std::uint16_t	next_inside_storey;
extern std::uint16_t	next_inside_stair;
extern std::int32_t	next_inside_block;



//
// Functions
//



extern std::int32_t	get_inside_alt(std::int32_t	inside);
extern std::int32_t find_inside_room(std::int32_t inside,std::int32_t x,std::int32_t z);
extern std::int32_t find_inside_flags(std::int32_t inside,std::int32_t x,std::int32_t z);
extern std::int32_t	person_slide_inside(std::int32_t inside,std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t *x2,std::int32_t *y2,std::int32_t *z2);
extern void	stair_teleport_bodge(Thing *p_person);
extern std::int32_t	find_stair_y(Thing *p_person,std::int32_t *y1,std::int32_t x,std::int32_t y,std::int32_t z,std::uint16_t *new_floor);


// ========================================================
//
// INSIDE NAVIGATION
//
// ========================================================

#if 0
// Never used!
MAV_Action INSIDE2_mav_enter (Thing *p_person, std::int32_t inside, std::uint8_t caps);		// To enter the building with the given inside
MAV_Action INSIDE2_mav_inside(Thing *p_person, std::int32_t inside, std::uint8_t x, std::uint8_t z);	// Navigating within a floor
MAV_Action INSIDE2_mav_stair (Thing *p_person, std::int32_t inside, std::int32_t new_inside); // Going up the stairs to another floor
MAV_Action INSIDE2_mav_exit  (Thing *p_person, std::int32_t inside);					// Exit the building
#endif




