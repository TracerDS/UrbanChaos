//
// Fog!
//

#pragma once




//
// Gets rid of all the fog info and starts afresh.
//

void FOG_init();


//
// Sets the area of focus.  Fog only exists within the area
// of focus.
//

void FOG_set_focus(
		std::int32_t x,
		std::int32_t z,
		std::int32_t radius);


//
// Makes the fog react to a gust of wind. The gust happens
// at (x1,z1).  (x2,z2) gives strength and direction.
//

void FOG_gust(
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);


//
// Process the fog.
//

void FOG_process();


//
// The fog module tells you how to draw each bit of fog.
//

#define FOG_TYPE_TRANS1	 0	// Very transparent.
#define FOG_TYPE_TRANS2	 1
#define FOG_TYPE_TRANS3	 2
#define FOG_TYPE_TRANS4	 3	// Very opaque.
#define FOG_TYPE_NO_MORE 4	// There is no more fog to draw.
#define FOG_TYPE_UNUSED  4

typedef struct
{
	std::uint8_t type;
	std::uint8_t trans;	// Transparency. 0 => more transparent.
	std::uint16_t size;		// Radius.
	std::uint16_t yaw;
	std::uint16_t shit;
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;

} FOG_Info;

void     FOG_get_start();
FOG_Info FOG_get_info ();


