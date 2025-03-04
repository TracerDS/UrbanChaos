//
// Interior design module.
//

#pragma once


//
// The size of the box inside which the floorplan can be.
// The maximum dimension of any building.
//

#define ID_FLOOR_SIZE 256
#define ID_PLAN_SIZE  32


//
// Given the outline of a storey of a building, this module returns a floor plan
// of where the rooms and the furniture should go.
//

//
// First give the outline of the storey of the building. Does not work with overlapping
// or intersecting walls!
//

void ID_clear_floorplan();
void ID_set_outline(
		std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2,
		std::int32_t id,
		std::int32_t num_blocks);

//
// The 'get_type' function returns whether there is a door or a window or wall on
// the 'block''th blocks along the wall with the given 'id'
//

#define ID_BLOCK_TYPE_WALL		1
#define ID_BLOCK_TYPE_WINDOW	2
#define ID_BLOCK_TYPE_DOOR		3

void ID_set_get_type_func(std::int32_t (*get_type)(std::int32_t id, std::int32_t block));

//
// This function generates the floorplan for the given type of building.
//

#define ID_STAIR_TYPE_BOTTOM	1
#define ID_STAIR_TYPE_MIDDLE	2
#define ID_STAIR_TYPE_TOP		3

typedef struct
{
	std::uint8_t type;
	std::uint8_t id;
	std::uint16_t shit;
	std::int32_t handle_up;		// The value returned when you go up the stairs.
	std::int32_t handle_down;		// The value returned when you go down the stairs.

	//
	// The two squares the stairs take up.  These are ordered
	// such that the stairs are on the left looking from
	// (x1,z1) to (x2,z2).
	//

	std::uint8_t x1;
	std::uint8_t z1;
	std::uint8_t x2;
	std::uint8_t z2;

} ID_Stair;

#define ID_STOREY_TYPE_HOUSE_GROUND			1
#define ID_STOREY_TYPE_HOUSE_UPPER			2
#define ID_STOREY_TYPE_OFFICE_GROUND		3
#define ID_STOREY_TYPE_OFFICE_UPPER			4
#define ID_STOREY_TYPE_WAREHOUSE			5
#define ID_STOREY_TYPE_APARTEMENT_GROUND	6
#define ID_STOREY_TYPE_APARTEMENT_UPPER		7

//
// If 'find_good_layout' then this function uses the seed and a large
// number derived from it to create many layouts, and then picks the
// best one, otherwise it always uses the given seed.  It returns the
// seed it used.
//
// The seed it uses always fits in a std::uint16_t. If the result is negative
// then an error occured and it could not generate a floorplan for the
// building.
//

std::int32_t ID_generate_floorplan(
			std::int32_t    type, 
			ID_Stair stair[],
			std::int32_t num_stairs,
			std::uint16_t seed,
			std::uint8_t find_good_layout,
			std::uint8_t furnished);

//
// Inserts collision vectors for the inside walls.
// Removes collision vectors for the inside walls.
//
// Always call these functions in pairs.
//

void ID_wall_colvects_insert();
void ID_wall_colvects_remove();

//
// Removes all the furniture and things created for the inside
// of the building.
//

void ID_remove_inside_things();


//
// Returs the room index of a mapsquare.  0 == nullptr room index.
// This is a mapsquare, not a world position.
//

std::uint8_t ID_get_mapsquare_room(std::int32_t x, std::int32_t z);

//
// Returns the world camera position for the given room.
//

void ID_get_room_camera(std::uint8_t room, std::int32_t *x, std::int32_t *y, std::int32_t *z);

//
// If (x,z) is a position on a staircase from where you should
// go to another floor and handle is either up_handle
// or down_handle of the staircase you've come from.
//
// Reutrns -1, 0 or +1 depending on whether you go up, nowhere, or down.
//

std::int32_t ID_change_floor(
		std::int32_t  x,
		std::int32_t  z,
		std::int32_t *new_x,
		std::int32_t *new_z,
		std::int32_t *handle);

//
// Accessing the inside of the building.
//

void ID_get_floorplan_bounding_box(
		std::int32_t *x1,
		std::int32_t *z1,
		std::int32_t *x2,
		std::int32_t *z2);

//
// Returns info about a floor square.
//

std::int32_t ID_am_i_completely_outside(std::int32_t x, std::int32_t z);
std::int32_t ID_should_i_draw_mapsquare(std::int32_t x, std::int32_t z);
std::int32_t ID_get_mapsquare_texture  (std::int32_t x, std::int32_t z,
		float *u0, float *v0,
		float *u1, float *v1,
		float *u2, float *v2,
		float *u3, float *v3);

//
// For drawing rooms you are inside. First tell the ID module where you are
// and then it can tell you whether or not to draw a mapsquare.
//

void  ID_this_is_where_i_am(std::int32_t x, std::int32_t z);
std::int32_t ID_should_i_draw     (std::int32_t x, std::int32_t z);	// (x,z) must be in the bounding square of the floorplan.

//
// Returns index of the first face above a floor square. 0 => nullptr index.
// Returns true if the face is a quad, otherwise it is a triangle.
// Returns the texture of the face.
// Returns the next face in the linked list of faces above a floor square.
//

std::int32_t ID_get_first_face  (std::int32_t x, std::int32_t z);
std::int32_t ID_is_face_a_quad  (std::int32_t face);
std::int32_t ID_get_next_face   (std::int32_t face);

//
// Fills in the texture coordinates of the given face and
// returns the page.
//

std::int32_t ID_get_face_texture(std::int32_t face,
		float *u0, float *v0,
		float *u1, float *v1,
		float *u2, float *v2,
		float *u3, float *v3);

//
// Drawing the furniture inside a building...
//

typedef struct
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::uint16_t prim;
	std::uint16_t yaw;

} ID_Finfo;

std::int32_t     ID_get_num_furn();
ID_Finfo *ID_get_furn(std::int32_t number);	// Starting from zero.



//
// Points are two types. They are either structures with (x,y,z) position
// and an 'index' field, or they are structures with just an (x,z) mapsquare
// coordinate.
//

//
// Clears the 'index' field in all the points.
// Returns true if the point is just a mapsquare coordinate.
// Returns the mapsquare coord of the point.
// Returns the position of the point.
// Returns the value of the 'index' field of the point.
// Sets the value of the 'index' field of the point.
//

void  ID_clear_indices       ();
std::int32_t ID_is_point_a_mapsquare(std::int32_t face, std::int32_t point);
void  ID_get_point_mapsquare (std::int32_t face, std::int32_t point, std::int32_t *x, std::int32_t *z);	// Map coordinates
void  ID_get_point_position  (std::int32_t face, std::int32_t point, std::int32_t *x, std::int32_t *y, std::int32_t *z); // ELE_SHIFT fixed point.
std::uint16_t ID_get_point_index     (std::int32_t face, std::int32_t point);
void  ID_set_point_index	 (std::int32_t face, std::int32_t point, std::uint16_t index);


// ########################################################
// ========================================================
//
// COLLISION STUFF...
//
// ========================================================
// ########################################################


//
// Returns true if the vector collides with the insides.
//

std::int32_t ID_collide_3d(
		std::int32_t x1, std::int32_t y1, std::int32_t z1,
		std::int32_t x2, std::int32_t y2, std::int32_t z2);

//
// Returns the height of the floor at (x,z)
// The coordinate must be inside the building.
//

std::int32_t ID_calc_height_at(std::int32_t x, std::int32_t z);


//
// Returns if the 2D vector on the ground collides with
// a wall. If it does, it returns the new end point of
// the vector that will let it 'slide' along the wall.
//

std::int32_t ID_collide_2d(
		std::int32_t  x1, std::int32_t z1,
		std::int32_t  x2, std::int32_t Z2,
		std::int32_t  radius,
		std::int32_t *slide_x,
		std::int32_t *slide_z);


// ########################################################
// ========================================================
//
// EDITOR STUFF...
//
// ========================================================
// ########################################################

//
// Returns the position of all the inside walls and all the rooms.
//

typedef struct
{
	std::uint8_t door[4];	// 255 => No door along this wall, else the number of
					// the block with a door in it.
	std::int32_t x1;
	std::int32_t z1;
	std::int32_t x2;
	std::int32_t z2;

} ID_Wallinfo;

typedef struct
{
	std::int32_t  x;		// A position inside the room.
	std::int32_t  z;
	char* what;	// A string describing the room.

} ID_Roominfo;

typedef struct
{
	std::int32_t x1;		// The two squares that contain the staircase.
	std::int32_t z1;
	std::int32_t x2;
	std::int32_t z2;

} ID_Stairinfo;

void ID_editor_start_get_rooms ();
void ID_editor_start_get_walls ();
void ID_editor_start_get_stairs();

//
// These functions return false if there are no more rooms, walls
// or stairs, otherwise they fill out the given structure with
// info describing the next room, wall or staircase.
//

std::int32_t ID_editor_get_room (ID_Roominfo  *ans);
std::int32_t ID_editor_get_wall (ID_Wallinfo  *ans);
std::int32_t ID_editor_get_stair(ID_Stairinfo *ans);




