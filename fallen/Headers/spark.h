//
// Electric sparks...
//

#pragma once

//
// Removes all sparks.
//

void SPARK_init();

//
// Creates a new spark between two points...
//

#define SPARK_TYPE_POINT	1
#define SPARK_TYPE_LIMB		2
#define SPARK_TYPE_GROUND	3
#define SPARK_TYPE_CIRCULAR	4

#define SPARK_FLAG_CLAMP_X		(1 << 0)
#define SPARK_FLAG_CLAMP_Y		(1 << 1)
#define SPARK_FLAG_CLAMP_Z		(1 << 2)
#define SPARK_FLAG_FAST			(1 << 3)
#define SPARK_FLAG_SLOW			(1 << 4)
#define SPARK_FLAG_DART_ABOUT	(1 << 5)
#define SPARK_FLAG_STILL		(SPARK_FLAG_CLAMP_X | SPARK_FLAG_CLAMP_Y | SPARK_FLAG_CLAMP_Z)

typedef struct
{
	std::uint8_t type;
	std::uint8_t flag;
	std::uint8_t padding;

	std::uint8_t dist;	// For circular types... the distance from the first point.

	std::uint16_t x;	// Point and ground types...
	std::uint16_t y;
	std::uint16_t z;

	THING_INDEX person;	// The limb to use.
	std::uint16_t		limb;
	
} SPARK_Pinfo;

void SPARK_create(
		SPARK_Pinfo *point1,
		SPARK_Pinfo *point2,
		std::uint8_t        max_life);


//
// Creates a spark somwhere in the given sphere.
//

void SPARK_in_sphere(
		std::int32_t mid_x,
		std::int32_t mid_y,
		std::int32_t mid_z,
		std::int32_t radius,
		std::uint8_t max_life,
		std::uint8_t max_create);


//
// Makes sparks appear on all the electric fences.
//

void SPARK_show_electric_fences();


//
// Process the sparks.
//

void SPARK_process();


//
// Draw the sparks...
//

#define SPARK_MAX_POINTS 5

typedef struct
{
	std::int32_t num_points;
	std::uint32_t colour;
	std::int32_t size;

	std::int32_t x[SPARK_MAX_POINTS];
	std::int32_t y[SPARK_MAX_POINTS];
	std::int32_t z[SPARK_MAX_POINTS];

} SPARK_Info;

void SPARK_get_start(std::uint8_t xmin, std::uint8_t xmax, std::uint8_t z);
SPARK_Info *SPARK_get_next ();	// nullptr => No more sparks...



