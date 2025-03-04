//
// The sewer editor.
//

#pragma once


#include "pap.h"


//
// The map we edit, load and save.
//

#define ES_TYPE_ROCK	0
#define ES_TYPE_SEWER	1
#define ES_TYPE_GROUND	2
#define ES_TYPE_HOLE	3

#define ES_FLAG_GRATING		(1 << 0)
#define ES_FLAG_ENTRANCE	(1 << 1)
#define ES_FLAG_NOCURBS		(1 << 2)

typedef struct
{
	std::uint8_t type;
	std::uint8_t height;
	std::uint8_t flag;
	std::uint8_t water;	// 0 => no water.

} ES_Hi;

extern ES_Hi ES_hi[PAP_SIZE_HI][PAP_SIZE_HI];

//
// Water in the city.
//

extern std::uint8_t ES_city_water_on   [PAP_SIZE_HI][PAP_SIZE_HI];
extern std::int8_t ES_city_water_level[PAP_SIZE_LO][PAP_SIZE_LO];


//
// The things.
//

#define ES_THING_TYPE_UNUSED 0
#define ES_THING_TYPE_LADDER 1
#define ES_THING_TYPE_PRIM	 2

typedef struct
{
	std::uint8_t type;
	std::uint8_t padding;
	std::uint8_t prim;
	std::uint8_t height;
	std::uint8_t x1;
	std::uint8_t z1;
	std::uint8_t x2;
	std::uint8_t z2;
	std::uint16_t yaw;
	std::int16_t x;
	std::int16_t y;
	std::int16_t z;

} ES_Thing;

#define ES_MAX_THINGS 512

extern ES_Thing ES_thing[ES_MAX_THINGS];

//
// The lo-res mapwho.
//

typedef struct
{
	//
	// Inside the square.
	//

	std::uint8_t light_on;	// true or false
	std::uint8_t light_x;
	std::uint8_t light_y;
	std::uint8_t light_z;

} ES_Lo;

extern ES_Lo ES_lo[PAP_SIZE_LO][PAP_SIZE_LO];


//
// Initialises the ES module.
//

void ES_init();


//
// Draws the game editor.
//

void ES_draw_editor(
		std::int32_t  cam_x,
		std::int32_t  cam_y,
		std::int32_t  cam_z,
		std::int32_t  cam_yaw,
		std::int32_t  cam_pitch,
		std::int32_t  cam_roll,
		std::int32_t  mouse_x,
		std::int32_t  mouse_y,
		std::int32_t *mouse_over_valid,
		std::int32_t *mouse_over_x,
		std::int32_t *mouse_over_y,
		std::int32_t *mouse_over_z,
		std::int32_t  draw_prim_at_mouse,
		std::int32_t  prim_object,
		std::int32_t  prim_yaw);

//
// Raises the height of the given square- all adjacent sewer squares
// are set to the same height.
//

void ES_change_height(
		std::int32_t map_x,
		std::int32_t map_z,
		std::int32_t dheight);


//
// Build the game NS sewers from the editor description.
//

void ES_build_sewers();


// ========================================================
//
// LADDER FUNCTIONS
//
// ========================================================

//
// Creates a ladder made by dragging a point from a to b.
//

void ES_ladder_create(
		std::int32_t ax,
		std::int32_t az,
		std::int32_t bx,
		std::int32_t bz);

//
// Changes the height of a ladder near the given world position.
//

void ES_ladder_dheight(
		std::int32_t x,
		std::int32_t z,
		std::int32_t dheight);

//
// Deletes a ladder near the given world position.
//

void ES_ladder_delete(
		std::int32_t x,
		std::int32_t z);


// ========================================================
//
// LIGHT FUNCTIONS
//
// ========================================================

//
// Validates the light in the implied lo-res mapsquare and
// moves it to the given world (x,z) location.
// 

void ES_light_move(std::int32_t x, std::int32_t z);

//
// Changes the height of the light inside the lo-res mapsquare
// given by the world (x,z) position.
//

void ES_light_dheight(std::int32_t x, std::int32_t z, std::int32_t dheight);

//
// Deletes the light inside the lo-res mapsquare
// given by the world (x,z) position.
//

void ES_light_delete(std::int32_t x, std::int32_t z);

// ========================================================
//
// WATER FUNCTIONS
//
// ========================================================

//
// Changes the height of the sewer water at world coordinate (x,z)
//

void ES_sewer_water_dheight(std::int32_t x, std::int32_t z, std::int32_t dheight);

//
// Set/get the city water 'on' status of the city water at (x,z)
//

std::int32_t ES_city_water_get(std::int32_t x, std::int32_t z);
void ES_city_water_set(std::int32_t x, std::int32_t z, std::int32_t on_or_not);

//
// Changes the level of the water at the city world coordinate (x,z).
//

void ES_city_water_dlevel(std::int32_t x, std::int32_t z, std::int32_t dlevel);


// ========================================================
//
// PRIM FUNCTIONS
//
// ========================================================

//
// Creates a new prim.
//

void ES_prim_create(
		std::int32_t prim,
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t yaw);

//
// Deletes a prim near the given coordinate.
//

void ES_prim_delete(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z);

//
// Changes the height of the prim near (x,y,z)
//

void ES_prim_dheight(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t dheight);

// ========================================================
//
// UNDO / REDO FUNCTIONS.
//
// ========================================================

void ES_undo_store();

void ES_undo_undo();
void ES_undo_redo();

std::int32_t ES_undo_undo_valid();
std::int32_t ES_undo_redo_valid();


//
// Loading / saving the sewer editor. Returns true on success.
//

std::int32_t ES_load(char* filename);
std::int32_t ES_save(char* filename);



