//
// SphereMatter - Hypermatter version 2!
//

#pragma once




//
// Initialises all the sphere matter
//

void SM_init();


//
// Create a sphere-matter cube.
//

#define SM_CUBE_TYPE_ROCK		0
#define SM_CUBE_TYPE_JELLY		1
#define SM_CUBE_TYPE_CARDBOARD	2
#define SM_CUBE_TYPE_NUMBER		3

void SM_create_cube(
		std::int32_t x1, std::int32_t y1, std::int32_t z1,
		std::int32_t x2, std::int32_t y2, std::int32_t z2,
		std::int32_t amount_resolution,	// 0 - 256
		std::int32_t amount_density,		// 0 - 256
		std::int32_t amount_jellyness);	// 0 - 256


//
// Processes all the sphere-matter
//

void SM_process();


//
// Drawing the sphere matter just involves drawing lots of spheres.
//

typedef struct
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t radius;
	std::uint32_t colour;
	
} SM_Info;

void SM_get_start();
SM_Info *SM_get_next ();



