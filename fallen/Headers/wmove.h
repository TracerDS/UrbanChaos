//
// Moving walkable faces.
//

#pragma once

//#include	"c:\fallen\headers\memory.h"

typedef struct
{
	std::uint16_t x;
	std::int16_t y;
	std::uint16_t z;
	
} WMOVE_Point;



typedef struct
{
	std::uint16_t face4;	// Index into the prim_faces4[] array for the walkable face.
	
	//
	// The position of the walkable face last frame. All WMOVE faces are rectangles.
	// Only three points are given. The last point is implicit.
	//

	WMOVE_Point last[3];

	std::uint16_t thing;	// The thing that this face is attached to.
	std::uint16_t number;	// If a thing has more than one face associated with it, this is the number of the face.

} WMOVE_Face;

#define RWMOVE_MAX_FACES 192
#define WMOVE_MAX_FACES	(save_table[SAVE_TABLE_WMOVE].Maximum)
//#define WMOVE_MAX_FACES 192

extern WMOVE_Face *WMOVE_face; //[WMOVE_MAX_FACES];
extern std::int32_t       WMOVE_face_upto;


//
// Initialises everything.
//

void WMOVE_init();


//
// Creates a moving walkable face attached to the given thing.
//

void WMOVE_create(Thing *);


//
// Removes all WMOVE_faces belonging to things of the given class.
//

#if 0
void WMOVE_remove(std::uint8_t which_class);
#endif

//
// Moves all the moving walkable faces.
//

void WMOVE_process();


//
// If somebody is standing on a walkable face, this gives their
// new world position so they can stay in the same place relative
// to the walkable face if it has moved.
//

void WMOVE_relative_pos(
		std::uint8_t  wmove_index,	// The WMOVE face stood on.
		std::int32_t  last_x,
		std::int32_t  last_y,
		std::int32_t  last_z,
		std::int32_t *now_x,
		std::int32_t *now_y,
		std::int32_t *now_z,
		std::int32_t *now_dangle);


//
// Draws the walkable faces using AENG_world_line()
// 

void WMOVE_draw();




