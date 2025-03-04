//
// Fire!
//

#pragma once


//
// Gets rid of all the fire.
// 

void FIRE_init();

//
// Creates a new bit of fire.
//

void FIRE_create(
		std::uint16_t x,
		std::int16_t y,
		std::uint16_t z,
		std::uint8_t size,
		std::uint8_t life);	// 0 => Live for ever, 1 => Live for for ages, 255 => live a very short time.

//
// Processes all the fire.
//

void FIRE_process();

//
// How to draw fire: The fire is given as a series of flames whose
// points are given in the following order.
//
//						8
//					   /|
//					  /	|
//					 /	|
//				   6/	|7
//				   #----#
//				   |	 \
//				  4#------#5
//				  /		 /
//			     /		/
//			   2#------#3
//				|	   |
//				#------#
//			   0		1
//
// In other words, it is a triangle strip!
//

typedef struct
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::uint8_t u;
	std::uint8_t v;
	std::uint8_t alpha;
	std::uint8_t shit;

} FIRE_Point;

typedef struct
{
	std::int32_t       num_points;
	FIRE_Point *point;

} FIRE_Info;

void       FIRE_get_start(std::uint8_t z, std::uint8_t x_min, std::uint8_t x_max);
FIRE_Info *FIRE_get_next ();		// nullptr => No more fire here.



