//
// Puddles!
//

#ifndef _PUDDLE_
#define _PUDDLE_


#ifndef TARGET_DC

//
// Clears out all puddle info.
//

void PUDDLE_init();

//
// Creates a puddle centered at (x,y,z) of the given radius and angle.
//

void PUDDLE_create(
		std::uint16_t x,
		std::int16_t y,
		std::uint16_t z);

//
// Puts down puddle on the edges of buildings and curbs.
//

void PUDDLE_precalculate();


//
// Looks for a puddle under (x,y,z) and splashes it if it find one.
// (x>>8,z>>8) are mapsquare (std::uint8_t) coordinates.
//

void PUDDLE_splash(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z);

//
// Returns true if the given point is in a puddle.
//

std::int32_t PUDDLE_in(
		std::int32_t x,
		std::int32_t z);

//
// Calms down splashes.
//

void PUDDLE_process();

//
// Returns all the puddles on the given z mapsquare line with an 'x'
// in the given range.
//

typedef struct
{
	std::int32_t x1;
	std::int32_t z1;
	std::int32_t u1;		// 0 to 256
	std::int32_t v1;		// 0 to 256

	std::int32_t x2;
	std::int32_t z2;
	std::int32_t u2;
	std::int32_t v2;

	std::int32_t y;

	std::uint8_t puddle_y1;
	std::uint8_t puddle_y2;
	std::uint8_t puddle_g1;
	std::uint8_t puddle_g2;
	std::uint8_t puddle_s1;
	std::uint8_t puddle_s2;
	std::uint16_t rotate_uvs;	// Rotate the uvs relative to the xzs
	
} PUDDLE_Info;

void PUDDLE_get_start(std::uint8_t z_map, std::uint8_t x_map_min, std::uint8_t x_map_max);
PUDDLE_Info *PUDDLE_get_next();


#endif //#ifndef TARGET_DC


#endif
