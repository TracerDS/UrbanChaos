#ifndef BUILDING_H
#define BUILDING_H 1

// #include	"c:\fallen\headers\memory.h"

// DEFINES

// #define	TEXTURE_PIECE_TOP_LEFT		(0)
// #define	TEXTURE_PIECE_TOP_MIDDLE	(1)
// #define	TEXTURE_PIECE_TOP_RIGHT		(2)

#define TEXTURE_PIECE_LEFT (0)
#define TEXTURE_PIECE_MIDDLE (1)
#define TEXTURE_PIECE_RIGHT (2)
#define TEXTURE_PIECE_MIDDLE1 (3)
#define TEXTURE_PIECE_MIDDLE2 (4)
#define TEXTURE_PIECE_NUMBER (5)

#if defined(PSX) || defined(TARGET_DC)
#define RMAX_PRIM_POINTS 15000
#define MAX_PRIM_FACES3 10000
#define RMAX_PRIM_FACES4 10000
#define MAX_PRIM_OBJECTS 2000
#define MAX_PRIM_MOBJECTS 100

#else
#define RMAX_PRIM_POINTS 65000
#define MAX_PRIM_FACES3 32000
#define RMAX_PRIM_FACES4 32760
#define MAX_PRIM_OBJECTS 2000
#define MAX_PRIM_MOBJECTS 100
#endif

#define MAX_PRIM_POINTS (save_table[SAVE_TABLE_POINTS].Maximum)
#define MAX_PRIM_FACES4 (save_table[SAVE_TABLE_FACE4].Maximum)

#define GRAB_FLOOR (MAX_PRIM_FACES4 + 1)
#define GRAB_SEWERS (MAX_PRIM_FACES4 + 2)

#define MAX_ROOF_BOUND 2000 //(8k)

#define BLOCK_SHIFT (6)
#define BLOCK_SIZE (1 << BLOCK_SHIFT)

#define BUILD_MODE_EDITOR (1)
#define BUILD_MODE_DX (2)
#define BUILD_MODE_SOFT (3)
#define BUILD_MODE_PSX (4)
#define BUILD_MODE_OTHER (5)

#define BUILDING_TYPE_HOUSE 0
#define BUILDING_TYPE_WAREHOUSE 1
#define BUILDING_TYPE_OFFICE 2
#define BUILDING_TYPE_APARTEMENT 3
#define BUILDING_TYPE_CRATE_IN 4
#define BUILDING_TYPE_CRATE_OUT 5

#if defined(PSX) || defined(TARGET_DC)
#define MAX_WINDOWS 1
#define MAX_WALLS 1
#define MAX_STOREYS 1
#define MAX_BUILDINGS 1
#define MAX_BUILDING_FACETS 1
#define MAX_BUILDING_OBJECTS 1
#define MAX_INSIDE_STOREYS (512)

#else
#define MAX_BUILDINGS (500)
#define MAX_STOREYS (MAX_BUILDINGS * 5)
#define MAX_WALLS (MAX_STOREYS * 6)
#define MAX_WINDOWS (50)
#define MAX_INSIDE_STOREYS (1024)

#define MAX_BUILDING_FACETS 4000
#define MAX_BUILDING_OBJECTS 400
#endif

#define FACET_FLAG_NEAR_SORT (1 << 0)
#define FACET_FLAG_ROOF (1 << 1)
#define FACET_FLAG_CABLE (1 << 2)
#define FACET_FLAG_NON_SORT (1 << 3)

#define FACET_FLAG_DONT_CULL (1 << 3)

#define FACET_FLAG_IN_SEWERS (1 << 4)
#define FACET_FLAG_LADDER_LINK (1 << 5) // A ladder you use to go in and out of the sewers.

#define FACET_FLAG_INVISIBLE (1 << 0) // facet is duplicate so mark invisible
#define FACET_FLAG_INSIDE (1 << 3)
#define FACET_FLAG_DLIT (1 << 4)        // Lit with a dynamic light.
#define FACET_FLAG_HUG_FLOOR (1 << 5)   // For fake fences that are normal walls
#define FACET_FLAG_ELECTRIFIED (1 << 6) // For fences...
#define FACET_FLAG_2SIDED (1 << 7)
#define FACET_FLAG_UNCLIMBABLE (1 << 8)
#define FACET_FLAG_ONBUILDING (1 << 9)
#define FACET_FLAG_BARB_TOP (1 << 10)
#define FACET_FLAG_SEETHROUGH (1 << 11)
#define FACET_FLAG_OPEN (1 << 12)     // For OUTSIDE_DOOR facets...
#define FACET_FLAG_90DEGREE (1 << 13) // Some OUTSIDE_DOOR facets open only by 90 degrees...
#define FACET_FLAG_2TEXTURED (1 << 14)
#define FACET_FLAG_FENCE_CUT (1 << 15)

#define FLAG_ROOF_FLAT (1 << 1)
#define FLAG_ROOF_OVERLAP_SMALL (1 << 2)
#define FLAG_ROOF_OVERLAP_MEDIUM (1 << 3)
#define FLAG_ROOF_WALLED (1 << 4)
#define FLAG_ROOF_RECESSED (1 << 5)

#define FLAG_WALL_USED (1 << 0)
#define FLAG_WALL_AUTO_WINDOWS (1 << 1)
#define FLAG_WALL_FACET_LINKED (1 << 2)
#define FLAG_WALL_FENCE_POST1 (1 << 3)
#define FLAG_WALL_FENCE_POST2 (1 << 4)
// #define	FLAG_WALL_ROOF_RIM		(1<<5)
#define FLAG_WALL_RECESSED (1 << 5)
#define FLAG_WALL_CLIMBABLE (1 << 6)
#define FLAG_WALL_ARCH_TOP (1 << 7)
#define FLAG_WALL_BARB_TOP (1 << 8)

#define FLAG_STOREY_USED (1 << 0)
#define FLAG_STOREY_FLAT_TILED_ROOF (1 << 1)
#define FLAG_STOREY_LEDGE (1 << 2)
#define FLAG_STOREY_ROOF (1 << 3)

#define FLAG_STOREY_FACET_LINKED (1 << 4)
#define FLAG_STOREY_TILED_ROOF (1 << 5)
#define FLAG_STOREY_ROOF_RIM (1 << 6)
#define FLAG_STOREY_ROOF_RIM2 (1 << 7)

#define FLAG_STOREY_UNCLIMBABLE (1 << 10)
#define FLAG_STOREY_ONBUILDING (1 << 11)

#define FLAG_STOREY_EXTRA_UNCLIMBABLE (1 << 0)
#define FLAG_STOREY_EXTRA_ONBUILDING (1 << 1)
#define FLAG_STOREY_EXTRA_90DEGREE (1 << 2)

#define FLAG_ISTOREY_DOOR (1 << 1)

// #define	FLAG_STOREY_ROOF_RIM2		(1<<7)

#define BROWN_BRICK1 1
#define BROWN_BRICK2 2
#define GREY_RIM1 3
#define GREY_RIM2 4
#define RED_WINDOW 5
#define GREY_CORIGATED 6
#define CRATES_SMALL_BROWN 7
#define GREY_POSH 8
#define HOTEL_SIGN1 9
#define HOTEL_SIGN2 10

#define STOREY_TYPE_NORMAL (1)
#define STOREY_TYPE_ROOF (2)
#define STOREY_TYPE_WALL (3)
#define STOREY_TYPE_ROOF_QUAD (4)
#define STOREY_TYPE_FLOOR_POINTS (5)
#define STOREY_TYPE_FIRE_ESCAPE (6)
#define STOREY_TYPE_STAIRCASE (7)
#define STOREY_TYPE_SKYLIGHT (8)
#define STOREY_TYPE_CABLE (9)
#define STOREY_TYPE_FENCE (10)
#define STOREY_TYPE_FENCE_BRICK (11)
#define STOREY_TYPE_LADDER (12)
#define STOREY_TYPE_FENCE_FLAT (13)
#define STOREY_TYPE_TRENCH (14)
#define STOREY_TYPE_JUST_COLLISION (15)
#define STOREY_TYPE_PARTITION (16)
#define STOREY_TYPE_INSIDE (17)
#define STOREY_TYPE_DOOR (18)
#define STOREY_TYPE_INSIDE_DOOR (19)
#define STOREY_TYPE_OINSIDE (20)
#define STOREY_TYPE_OUTSIDE_DOOR (21)

#define STOREY_TYPE_NORMAL_FOUNDATION (100)

#define STOREY_TYPE_NOT_REALLY_A_STOREY_TYPE_BUT_A_VALUE_TO_PUT_IN_THE_PRIM_TYPE_FIELD_OF_COLVECTS_GENERATED_BY_INSIDE_BUILDINGS (254)
#define STOREY_TYPE_NOT_REALLY_A_STOREY_TYPE_AGAIN_THIS_IS_THE_VALUE_PUT_INTO_PRIMTYPE_BY_THE_SEWERS (255)

// STRUCTS

struct BuildingObject {
    std::int16_t FacetHead;
    std::uint16_t StartFace4;
    std::uint16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;
    std::uint16_t StartPoint;
    std::uint16_t EndPoint;
};

struct BuildingFacet {
    std::uint16_t StartFace4;
    std::uint16_t MidFace4;
    std::uint16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;
    std::uint16_t StartPoint;
    std::uint16_t MidPoint;
    std::uint16_t EndPoint;
    std::int16_t NextFacet;
    std::uint16_t FacetFlags;
    std::int16_t ColVect;
};

struct BoundBox {
    std::uint8_t MinX;
    std::uint8_t MaxX;
    std::uint8_t MinZ;
    std::uint8_t MaxZ;
    std::int16_t Y;
};

struct TempBuilding {
    std::uint16_t FacetHead;
    std::uint16_t FacetCount;
};
// 4 bytes

struct TempFacet {
    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;

    std::int16_t Y;
    std::uint16_t PrevFacet;

    std::uint16_t NextFacet;
    std::uint16_t RoofType;

    std::uint16_t StoreyHead;
    std::uint16_t StoreyCount;
};
// 28 bytes

struct TempStorey {
    std::uint16_t StoreyFlags;
    std::uint8_t WallStyle;
    std::uint8_t WindowStyle;
    std::int16_t Height;
    std::int16_t Next;
    std::int16_t Count;
};
// 10 bytes

struct TXTY {
    std::uint8_t Page, Tx, Ty, Flip;
};

struct DXTXTY {
    std::uint16_t Page;
    std::uint16_t Flip;
};

struct TextureInfo {
    std::uint8_t Type;
    std::uint8_t SubType;
};

struct FWindow {
    std::uint16_t Dist;
    std::uint16_t Height;
    std::uint16_t WindowFlags;
    std::uint16_t WindowWidth;
    std::uint16_t WindowHeight;
    std::int16_t Next;
    std::uint16_t Dummy[6];
};

struct FWall {
    std::int16_t DX, DZ;
    std::uint16_t WallFlags;

    std::int16_t TextureStyle2;
    std::uint16_t TextureStyle;

    std::uint16_t Tcount2;
    std::int16_t Next;

    std::uint16_t DY;
    std::uint16_t StoreyHead;

    std::uint8_t *Textures;
    std::uint16_t Tcount;
    std::uint8_t *Textures2;

    std::uint16_t Dummy[1];
};

struct FStorey {
    std::int16_t DX, DY, DZ;
    std::uint8_t StoreyType;
    std::uint8_t StoreyFlags;

    std::uint16_t Height;
    std::int16_t WallHead;

    //	std::uint16_t	WallCount;
    //	std::uint16_t	Roof;

    std::uint16_t ExtraFlags;

    std::uint16_t InsideIDIndex; //*Textures;

    std::int16_t Next;
    std::int16_t Prev;
    std::int16_t Info1;
    std::int16_t Inside;
    std::uint16_t BuildingHead;

    std::uint16_t InsideStorey;
};

#define MAX_ROOMS_PER_FLOOR 16
#define MAX_STAIRS_PER_FLOOR 10

struct RoomID {
    std::uint8_t X[MAX_ROOMS_PER_FLOOR], Y[MAX_ROOMS_PER_FLOOR];
    std::uint8_t Flag[MAX_ROOMS_PER_FLOOR];

    std::uint8_t StairsX[MAX_STAIRS_PER_FLOOR];
    std::uint8_t StairsY[MAX_STAIRS_PER_FLOOR];
    std::uint8_t StairFlags[MAX_STAIRS_PER_FLOOR];

    std::uint8_t FloorType;

    std::uint8_t Dummy[5 * 4];
};

#define STAIR_FLAG_UP (1 << 0)
#define STAIR_FLAG_DOWN (1 << 1)
#define STAIR_FLAGS_DIR ((1 << 2) | (1 << 3))
#define SET_STAIR_DIR(x, dir) ((x) = ((x) & ~STAIR_FLAGS_DIR) | (dir << 2))
#define GET_STAIR_DIR(x) (((x) & STAIR_FLAGS_DIR) >> 2)
struct FBuilding {
    std::uint16_t ThingIndex;
    std::uint16_t LastDrawn; // The GAME_TURN this building was last drawn at.
    std::uint8_t Dummy2;
    std::uint8_t Foundation;
    std::int16_t OffsetY;
    std::uint16_t InsideSeed;
    std::uint8_t MinFoundation;
    std::uint8_t ExtraFoundation;
    std::uint16_t BuildingFlags;
    std::int16_t StoreyHead;
    std::int16_t Angle;
    std::uint16_t StoreyCount;
    char str[20];
    std::uint8_t StairSeed;
    std::uint8_t BuildingType;
    std::uint16_t Walkable;
    std::uint16_t Dummy[4];
};

typedef SVector PrimNormal;

// data
#ifndef PSX
extern struct DXTXTY dx_textures_xy[200][5];
#endif

extern std::uint16_t next_roof_bound;

extern std::uint16_t next_prim_point;
extern std::uint16_t next_prim_face4;
extern std::uint16_t next_prim_face3;
extern std::uint16_t next_prim_object;
extern std::uint16_t next_prim_multi_object;

extern std::uint16_t end_prim_point;
extern std::uint16_t end_prim_face4;
extern std::uint16_t end_prim_face3;
extern std::uint16_t end_prim_object;
extern std::uint16_t end_prim_multi_object;

extern std::uint16_t next_building_object;
extern std::uint16_t end_building_object;

extern std::uint16_t next_building_facet;
extern std::uint16_t end_building_facet;

extern struct BuildingObject building_objects[];
extern struct BuildingFacet building_facets[];

extern struct FWindow window_list[MAX_WINDOWS];
extern struct FWall wall_list[MAX_WALLS];
extern struct FStorey storey_list[MAX_STOREYS];
extern struct FBuilding building_list[MAX_BUILDINGS];
extern struct TXTY textures_xy[200][5];
extern std::uint8_t textures_flags[200][5];
extern struct TextureInfo texture_info[];

extern struct RoomID room_ids[MAX_INSIDE_STOREYS];
extern std::int32_t next_inside;

// functions

extern std::int32_t place_building_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);
extern void offset_buildings(std::int32_t x, std::int32_t y, std::int32_t z);
// extern void	calc_buildings_world_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect);
// extern void	calc_buildings_screen_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect);
extern void draw_a_building_at(std::uint16_t building, std::int32_t x, std::int32_t y, std::int32_t z);
extern void create_city(std::uint8_t flag);
extern std::int32_t create_building_prim(std::uint16_t building, std::int32_t *small_y);
extern std::int32_t next_connected_face(std::int32_t type, std::int32_t id, std::int32_t count);
extern std::int32_t is_storey_circular(std::int32_t storey);

//
// Returns the start and end of the given wall.
//

void get_wall_start_and_end(
    std::int32_t wall,

    //
    // These are 16-bit map coordinates...
    //

    std::int32_t *x1, std::int32_t *z1,
    std::int32_t *x2, std::int32_t *z2);

//
// Returns how far (x,z) is along a cable.  Value goes from 0 to
// (1 << CABLE_ALONG_SHIFT)
//

#define CABLE_ALONG_SHIFT 12
#define CABLE_ALONG_MAX (1 << CABLE_ALONG_SHIFT)

std::int32_t get_cable_along(
    std::int32_t storey,
    std::int32_t x,
    std::int32_t z);

//
// Make a cable taut, as if someone is dangling at some point along it.
// It returns the height of the point she is dangling at.
//

//
// If you know how far she is along the cable. This is the percentage
// straight-line distance from (x1,y1,z1) of the cable to (x2,y2,z2).
// 'along' goes from 0 to 256.
//

void make_cable_taut_along(
    std::int32_t along, // 0 - 256
    std::int32_t building,
    std::int32_t *x_hanging_point,
    std::int32_t *y_hanging_point,
    std::int32_t *z_hanging_point);

//
// Returns the cable to being flabby.
//

void make_cable_flabby(std::int32_t building);

#endif