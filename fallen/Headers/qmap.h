//
// A whole new world...
//

#ifndef _QMAP_
#define _QMAP_

//
// Big map!
//

#define QMAP_SIZE 1024

// ========================================================
//
// CREATING THE MAP
//
// ========================================================

void QMAP_init();

void QMAP_add_road(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2);

void QMAP_add_cube(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t height);

void QMAP_add_prim(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t prim,
    std::int32_t yaw);

// ========================================================
//
// Accessing the map.
//
// ========================================================

//
// Calculates the height of the map at (x,z) ignoring the cubes.
// Coordinates are in 8-bit fixed point.  i.e. from 0 << 8 to QMAP_SIZE << 8
//

std::int32_t QMAP_calc_height_at(std::int32_t x, std::int32_t z);

//
// Returns the block coordinates of the given cube.
//

void QMAP_get_cube_coords(
    std::uint16_t cube,
    std::int32_t *x1, std::int32_t *y1, std::int32_t *z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2);

// ========================================================
//
// THE MAP DATA STRUCTURE
//
// ========================================================

//
// The styles.
//

#define QMAP_MAX_TEXTURES 4096

extern std::uint16_t QMAP_texture[QMAP_MAX_TEXTURES];
extern std::int32_t QMAP_texture_upto;

#define QMAP_STYLE_USED (1 << 0)
#define QMAP_STYLE_WRAP_X (1 << 1)
#define QMAP_STYLE_WRAP_Y (1 << 2)

typedef struct
{
    std::uint8_t size_x;
    std::uint8_t size_y;
    std::uint16_t flag;
    std::uint16_t index; // Index into the QMAP_texture array.

} QMAP_Style;

#define QMAP_MAX_STYLES 256

extern QMAP_Style QMAP_style[QMAP_MAX_STYLES];
extern std::int32_t QMAP_style_upto;

//
// The roads.
//

typedef struct
{
    std::uint16_t x;
    std::uint16_t z;
    std::uint8_t size_x;
    std::uint8_t size_z;

} QMAP_Road;

#define QMAP_MAX_ROADS 1024

extern QMAP_Road QMAP_road[QMAP_MAX_ROADS];
extern std::int32_t QMAP_road_upto;

//
// The cubes.
//

typedef struct
{
    std::uint16_t x;
    std::uint16_t z;
    std::uint8_t size_x;
    std::uint8_t size_y;
    std::uint8_t size_z;
    std::uint8_t flag;
    std::uint16_t style[5];

} QMAP_Cube;

#define QMAP_MAX_CUBES 1024

extern QMAP_Cube QMAP_cube[QMAP_MAX_CUBES];
extern std::int32_t QMAP_cube_upto;

//
// The ground textures.
//

typedef struct
{
    std::uint16_t x;
    std::uint16_t z;
    std::uint8_t size_x;
    std::uint8_t size_z;
    std::uint16_t style;

} QMAP_Gtex;

#define QMAP_MAX_GTEXES 4096

extern QMAP_Gtex QMAP_gtex[QMAP_MAX_GTEXES];
extern std::int32_t QMAP_gtex_upto;

//
// The cables.
//

typedef struct
{
    std::uint16_t x1;
    std::uint16_t z1;
    std::uint16_t x2;
    std::uint16_t z2;
    std::uint8_t y1;
    std::uint8_t y2;
    std::uint8_t flag;
    std::uint8_t along;

} QMAP_Cable;

#define QMAP_MAX_CABLES 512

extern QMAP_Cable QMAP_cable[QMAP_MAX_CABLES];
extern std::int32_t QMAP_cable_upto;

//
// The heights used by the height maps.
//

#define QMAP_MAX_HEIGHTS 8192

extern std::int8_t QMAP_height[QMAP_MAX_HEIGHTS];
extern std::int32_t QMAP_height_upto;

//
// The heightmaps.
//

typedef struct
{
    std::uint16_t x;
    std::uint16_t z;
    std::uint8_t size_x;
    std::uint8_t size_z;

    //
    // If the top bit is set then the height map is at a
    // constant height, otherwise the height map defines
    // a bitmap.
    //
    // top bit  one: height - 0xa000 is the signed constant height of the heightmap.
    // top bit zero: height is an index into the QMAP_height array.
    //

    std::uint16_t height;

} QMAP_Hmap;

#define QMAP_MAX_HMAPS 64

extern QMAP_Hmap QMAP_hmap[QMAP_MAX_HMAPS];
extern std::int32_t QMAP_hmap_upto;

//
// Fences/walls...
//

#define QMAP_FENCE_NORMAL 1
#define QMAP_FENCE_BARBED 2
#define QMAP_FENCE_WALL 3

typedef struct
{
    std::uint8_t type;
    std::uint8_t y; // Above the ground.
    std::uint16_t x1;
    std::uint16_t z1;
    std::uint8_t size_x;
    std::uint8_t size_z;

} QMAP_Fence;

#define QMAP_MAX_FENCES 1024

extern QMAP_Fence QMAP_fence[QMAP_MAX_FENCES];
extern std::int32_t QMAP_fence_upto;

//
// Static lights / anti-lights.  Some lights
// are automatically allocted to prims aswell.
//

typedef struct
{
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t z;
    std::int8_t red;
    std::int8_t green;
    std::int8_t blue;
    std::uint8_t range;

} QMAP_Light;

#define QMAP_MAX_LIGHTS 2048

extern QMAP_Light QMAP_light[QMAP_MAX_LIGHTS];
extern std::int32_t QMAP_light_upto;

//
// The prims.
//

typedef struct
{
    std::uint8_t x;
    std::uint8_t y;
    std::uint8_t z;
    std::uint8_t yaw;
    std::uint8_t prim;

} QMAP_Prim;

#define QMAP_MAX_PRIMS 16384

extern QMAP_Prim QMAP_prim[QMAP_MAX_PRIMS];
extern std::int32_t QMAP_prim_upto;

//
// The map is stored on a high-level 32x32 mapwho basis.
//

#define QMAP_MAPSIZE (QMAP_SIZE / 32)

//
// The all array...
//

#define QMAP_MAX_ALL 32768

extern std::uint16_t QMAP_all[QMAP_MAX_ALL];
extern std::int32_t QMAP_all_upto;

//
// The map.
//

typedef struct
{
    std::uint16_t index_all;
    std::uint16_t index_prim;

    std::uint8_t num_roads; // We don't need a std::uint8_t for each of these.
    std::uint8_t num_cubes;
    std::uint8_t num_gtexes;
    std::uint8_t num_cables;
    std::uint8_t num_hmaps;
    std::uint8_t num_fences;
    std::uint8_t num_lights;

    std::uint8_t num_prims;

} QMAP_Map;

extern QMAP_Map QMAP_map[QMAP_MAPSIZE][QMAP_MAPSIZE];

//
// For getting the index into the all array for each type of thing.
//

#define QMAP_ALL_INDEX_ROADS(qm) ((qm)->index_all)
#define QMAP_ALL_INDEX_CUBES(qm) ((qm)->index_all + (qm)->num_roads)
#define QMAP_ALL_INDEX_GTEXES(qm) ((qm)->index_all + (qm)->num_roads + (qm)->num_cubes)
#define QMAP_ALL_INDEX_CABLES(qm) ((qm)->index_all + (qm)->num_roads + (qm)->num_cubes + (qm)->num_gtexes)
#define QMAP_ALL_INDEX_HMAPS(qm)  ((qm)->index_all + (qm)->num_roads + (qm)->num_cubes + (qm)->num_gtexes) + (qm)->num_cables)
#define QMAP_ALL_INDEX_FENCES(qm) ((qm)->index_all + (qm)->num_roads + (qm)->num_cubes + (qm)->num_gtexes) + (qm)->num_cables + (qm)->num_hmaps)
#define QMAP_ALL_INDEX_LIGHTS(qm) ((qm)->index_all + (qm)->num_roads + (qm)->num_cubes + (qm)->num_gtexes) + (qm)->num_cables + (qm)->num_hmaps + (qm)->num_fences)

//
// The total amount of ALL info used by a mapsquare.
//

#define QMAP_TOTAL_ALL(qm) ((qm)->num_roads + (qm)->num_cubes + (qm)->num_gtexes + (qm)->num_cables + (qm)->num_hmaps + (qm)->num_fences + (qm)->num_lights)

// ========================================================
//
// DRAWING THE MAP DATA-STRUCTURE.
//
// ========================================================

//
// The normals in the points.
//

#define QMAP_POINT_NORMAL_XPOS 0
#define QMAP_POINT_NORMAL_XNEG 1
#define QMAP_POINT_NORMAL_YPOS 2
#define QMAP_POINT_NORMAL_YNEG 3
#define QMAP_POINT_NORMAL_ZPOS 4
#define QMAP_POINT_NORMAL_ZNEG 5
#define QMAP_POINT_NORMAL_NUMBER 6

//
// The points.
//

typedef struct
{
    std::uint16_t x; // Relative to the mapsquare the point is in (8-bit fixed point)
    std::int16_t y;
    std::uint16_t z;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t trans; // Last last GAMETURN this point was transformed.
    std::uint8_t normal;
    std::uint8_t padding;
    std::uint16_t next;

} QMAP_Point;

#define QMAP_MAX_POINTS 2048

extern QMAP_Point QMAP_point[QMAP_MAX_POINTS];
extern std::uint16_t QMAP_point_free;

//
// The faces
//

#define QMAP_FACE_FLAG_SHADOW1 (1 << 0)
#define QMAP_FACE_FLAG_SHADOW2 (1 << 1)

typedef struct
{
    std::uint16_t point[4];
    std::uint16_t texture;
    std::uint16_t flag;
    std::uint16_t next;

#ifdef EDITOR
    std::uint16_t cube;
    std::uint16_t edge;
#endif

} QMAP_Face;

#define QMAP_MAX_FACES 2048

extern QMAP_Face QMAP_face[QMAP_MAX_FACES];
extern std::uint16_t QMAP_face_free;

//
// Each height-field square and height-field point.
//

typedef struct
{
    std::uint16_t texture;
    std::uint16_t flag;
    std::int8_t height;
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;

} QMAP_Hsquare;

//
// This is the structure you use to draw a mapsquare.
//

typedef struct
{
    //
    // A 32x32 height-field. It goes up to 33, so you can
    // have the last row of points available.
    //

    QMAP_Hsquare hf[33][33];
    std::uint8_t map_x;
    std::uint8_t map_z;
    std::uint8_t trans;

    //
    // The points and faces created from the cubes, fences and walls.
    //

    std::uint16_t next_point;
    std::uint16_t next_face;

} QMAP_Draw;

//
// Initialises all the points and faces.
//

void QMAP_draw_init();

//
// Fills in the QMAP_Draw structure for the given mapsquare.
//

void QMAP_create(QMAP_Draw *fill_me_in, std::int32_t map_x, std::int32_t map_z);

//
// Frees all the points and faces from the given QMAP_Draw structure.
//

void QMAP_free(QMAP_Draw *free_me_up);

#endif
