#ifndef	EMAP_H
#define	EMAP_H	1

#undef ELE_SIZE

//defs
#define	ELE_SHIFT				(8)
#define	ELE_SIZE				(1<<ELE_SHIFT)
#define	HALF_ELE_SIZE			((ELE_SIZE)>>1)
#define	ELE_AND					(0xffffff00)

#define EDIT_MAP_WIDTH	128
//1024
#define EDIT_MAP_HEIGHT	1
#define EDIT_MAP_DEPTH	128
//4

//structs


struct	DepthStrip
{
	std::uint16_t	MapThingIndex;
//	std::uint16_t	Depth[EDIT_MAP_DEPTH];
	std::uint16_t	ColVectHead;
//	std::uint16_t	Dummy1;
	std::uint16_t	Texture;
	std::int16_t	Bright;
	std::uint8_t	Flags;
	std::int8_t	Y;
	std::int16_t	Walkable;
};

//data
extern struct	DepthStrip	edit_map[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH];  //2meg
extern std::int8_t	edit_map_roof_height[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH];
extern std::uint16_t	tex_map[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH];


//code

#endif