//
// The map draw.
//

#ifndef _PLANMAP_
#define _PLANMAP_



//
// Draws the plan view of the city.
// 

void plan_view_shot(std::int32_t wx,std::int32_t wz,std::int32_t pixelw,std::int32_t sx,std::int32_t sy,std::int32_t w,std::int32_t h,std::uint8_t *mem);


//
// Draws a dot on the map.
//

#define BEACON_FLAG_BEACON (1 << 0)	// This is a beacon
#define BEACON_FLAG_POINTY (1 << 1)	// This is a player- takes direction from 'dir'

void map_beacon_draw(
		std::int32_t x,
		std::int32_t z,
		std::uint32_t col,
		std::uint32_t flags,
		std::uint16_t dir);





#endif
