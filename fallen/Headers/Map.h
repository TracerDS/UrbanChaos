// Map.h
// Guy Simmons, 22nd October 1997.

#pragma once

#include "light.h"


//---------------------------------------------------------------

#undef ELE_SIZE


#define	MAP_HEIGHT_SHIFT		(7)
#define	MAP_WIDTH_SHIFT			(7)
#define	MAP_HEIGHT				(128)
#define	MAP_WIDTH				(128)
#define	MAP_SIZE				(MAP_HEIGHT*MAP_WIDTH)
#define	MAP_INDEX(x,y)			(((x)<<MAP_WIDTH_SHIFT)|(y))
#define	ELE_SHIFT				(8)
#define	ELE_SIZE				(256)

//---------------------------------------------------------------


//
// Flags... these are mirrored in EDIT.H in the EDITOR project.
//

#define FLOOR_SHADOW_TYPE	7

//#define FLOOR_SHADOW_1		(1<<0)
//#define FLOOR_SHADOW_2		(1<<1)
//#define FLOOR_REFLECTIVE	(1<<2)
#define	FLOOR_HIDDEN		(1<<4)
//#define FLOOR_SINK_SQUARE	(1<<4)	// Lowers the floorsquare to create a curb.
//#define FLOOR_SINK_POINT	(1<<5)	// Transform the point on the lower level.
//#define FLOOR_NOUPPER		(1<<6)	// Don't transform the point on the upper level.
#define	FLOOR_TRENCH		(1<<7)  // trench square
#define	FLOOR_ANIM_TMAP		(1<<8)

#define	FLOOR_IS_ROOF		(1<<6)

#define	FLOOR_HEIGHT_SHIFT	(3)


typedef	struct
{
	LIGHT_Colour	Color;
	std::int8_t			AltNotUsedAnyMore;
	std::uint16_t			FlagsNotUsedAnyMore;
	std::uint16_t			ColVectHead,
					TextureNotUsedAnyMore;
	std::int16_t			Walkable;
	THING_INDEX		MapWho;
}MapElement; //14 bytes for pc

//---------------------------------------------------------------

typedef	struct
{
	std::uint16_t 	X		:	3;
	std::uint16_t 	Y		:	3;
	std::uint16_t	Page	:	4;
	std::uint16_t	Rot		:	2;
	std::uint16_t	Flip	:	2;
	std::uint16_t	Size	:	2;

}CodedTexture;

//---------------------------------------------------------------

void	init_map();

//---------------------------------------------------------------

//
// Functions to give to the light module access to the map.
//

std::int32_t        MAP_light_get_height(std::int32_t x, std::int32_t z);
LIGHT_Colour MAP_light_get_light (std::int32_t x, std::int32_t z);
void         MAP_light_set_light (std::int32_t x, std::int32_t z, LIGHT_Colour colour);

extern LIGHT_Map MAP_light_map;



