#ifndef BUILDING_H
#define BUILDING_H 1

// DEFINES
#define MAX_BUILDINGS (500)
#define MAX_STOREYS (MAX_BUILDINGS * 5)
#define MAX_WALLS (MAX_STOREYS * 6)
#define MAX_WINDOWS (MAX_WALLS * 4)

#define MAX_BUILDING_FACETS 2000
#define MAX_BUILDING_OBJECTS 200

#define FACET_FLAG_NEAR_SORT (1 << 0)
#define FACET_FLAG_ROOF (1 << 1)
#define FACET_FLAG_CABLE (1 << 2)

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
#define FLAG_WALL_ROOF_RIM (1 << 5)
#define FLAG_WALL_ARCH_SIDE (1 << 6)
#define FLAG_WALL_ARCH_TOP (1 << 7)

#define FLAG_STOREY_USED (1 << 0)
#define FLAG_STOREY_NORMAL (1 << 1)
#define FLAG_STOREY_LEDGE (1 << 2)
#define FLAG_STOREY_ROOF (1 << 3)

#define FLAG_STOREY_FACET_LINKED (1 << 4)
#define FLAG_STOREY_TILED_ROOF (1 << 5)
#define FLAG_STOREY_ROOF_RIM (1 << 6)

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
#define STOREY_TYPE_DRAINPIPE (8)
#define STOREY_TYPE_CABLE (9)
#define STOREY_TYPE_FENCE (10)

#define FACE_TYPE_FIRE_ESCAPE 1

// STRUCTS

struct BuildingObject {
    std::int16_t FacetHead;
    std::int16_t StartFace4;
    std::int16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;
    std::int16_t StartPoint;
    std::int16_t EndPoint;
};

struct BuildingFacet {
    std::int16_t StartFace4;
    std::int16_t MidFace4;
    std::int16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;
    std::int16_t StartPoint;
    std::int16_t MidPoint;
    std::int16_t EndPoint;
    std::int16_t NextFacet;
    std::uint16_t FacetFlags;
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
    std::int16_t WindowHead;
    std::uint16_t TextureStyle;
    std::uint16_t WindowCount;
    std::int16_t Next;
    std::uint16_t DY;
    std::uint16_t StoreyHead;
    std::uint16_t Dummy[6];
};

struct FStorey {
    std::int16_t DX, DY, DZ;
    std::uint8_t StoreyType;
    std::uint8_t StoreyFlags;

    std::uint16_t Height;
    std::int16_t WallHead;

    std::uint16_t WallCount;
    std::uint16_t Roof;

    std::int16_t Next;
    std::int16_t Prev;
    std::int16_t Info1;
    std::int16_t Info2;
    std::uint16_t BuildingHead;

    std::uint16_t Dummy[1];
};

struct FBuilding {
    std::int32_t X, Y, Z;
    std::uint16_t BuildingFlags;
    std::int16_t StoreyHead;
    std::int16_t Angle;
    std::uint16_t StoreyCount;
    char str[20];
    std::uint16_t Dummy[6];
};

// data

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
extern struct TXTY textures_xy[200][8];
extern std::uint8_t textures_flags[200][8];
extern struct TextureInfo texture_info[];

// functions

extern std::int32_t place_building_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);
extern void offset_buildings(std::int32_t x, std::int32_t y, std::int32_t z);
// extern void	calc_buildings_world_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect);
// extern void	calc_buildings_screen_box(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,EdRect *rect);
extern void draw_a_building_at(std::uint16_t building, std::int32_t x, std::int32_t y, std::int32_t z);
extern void create_city();
extern std::int32_t create_building_prim(std::uint16_t building, std::int32_t *small_y);
extern std::int32_t next_connected_face(std::int32_t type, std::int32_t id, std::int32_t count);
extern std::int32_t is_storey_circular(std::int32_t storey);

#endif