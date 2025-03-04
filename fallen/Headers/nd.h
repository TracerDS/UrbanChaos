//
// New indoors.
//

#ifndef _ND_
#define _ND_


//
// The directions leading out of a navsquare.
//

#define ND_DIR_XS 0
#define ND_DIR_XL 1
#define ND_DIR_ZS 2
#define ND_DIR_ZL 3

//
// The navbits. Two bits for each direction.
//

#define ND_NAV_TYPE_WALL	0
#define ND_NAV_TYPE_NORMAL	1
#define ND_NAV_TYPE_DOORWAY	2
#define ND_NAV_TYPE_UNUSED	3


//
// Each storey has a rectangular array of squares- the bounding box
// of its floorplan.
//

typedef struct
{
	std::uint8_t texture;	// Bottom 6 bits are the texture, top 2 bits are the flip.
	std::uint8_t nav;

} ND_Square;

#define ND_MAX_SQUARES 2048

ND_Square ND_square[ND_MAX_SQUARES];
std::int32_t ND_square_upto;


//
// External walls. The first point of the first wall is the (x1,z1) of the floorplan
// bounding box.  This last wall has this point as its endpoint.
//

typedef struct
{
	std::uint8_t x1;	// End point.
	std::uint8_t z1;
	std::uint16_t texture;

} ND_Exwall;

#define ND_MAX_EXWALLS 64

ND_Exwall ND_exwall[ND_MAX_EXWALLS];
std::int32_t ND_exwall_upto;


//
// Internal walls.
//

typedef struct
{
	std::uint8_t x1;
	std::uint8_t z1;
	std::uint8_t x2;
	std::uint8_t z2;

	std::uint16_t texture;

} ND_Inwall;

#define ND_MAX_INWALLS 64

ND_Inwall ND_inwall[ND_MAX_INWALLS];
std::int32_t ND_inwall_upto;


//
// The exits out of a floor.
//

#define ND_EXIT_TYPE_OUTSIDE 0
#define ND_EXIT_TYPE_UP		 1
#define ND_EXIT_TYPE_DOWN    2
#define ND_EXIT_TYPE_ROOF	 3

typedef struct
{
	std::uint8_t type;
	std::uint8_t dir;
	std::uint8_t x;
	std::uint8_t z;

} ND_Exit;

#define ND_MAX_EXITS 256

ND_Exit ND_exit[ND_MAX_EXITS];
std::int32_t ND_exit_upto;


//
// Each storey.
//

typedef struct
{
	//
	// This inclusive bounding box of the floorplan.
	//

	std::uint8_t x1;	// These are squares- not points.
	std::uint8_t z1;
	std::uint8_t x2;
	std::uint8_t z2;

	//
	// The floorplan.
	//

	std::uint8_t size_x;
	std::uint8_t size_z;
	std::uint16_t square;		// Index in the ND_square[] array

	std::uint16_t dbuilding;	// The building whose inside this is.

	std::int8_t alt;			// The altitude of this floor.  In MAV units- i.e. 4 per mapsquare.
	std::uint8_t padding;

	std::uint8_t inwall_num;
	std::uint8_t inwall_index;

	std::uint8_t exwall_num;
	std::uint8_t exwall_index;

	std::uint8_t exit_index;
	std::uint8_t exit_num

} ND_Floor;

#define ND_MAX_FLOORS 64

ND_Floor ND_floor[ND_MAX_FLOORS];
std::int32_t ND_floor_upto;



#endif
