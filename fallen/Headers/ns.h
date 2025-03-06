//
// A new super duper sewer/cavern system.
//

#ifndef _NS_
#define _NS_

#include "pap.h"

// ========================================================
//
// THE SEWER MAP
//
// ========================================================

//
// The high-res map. Set bot, water and packed. 'top' is calculated
// for you.
//

#define NS_HI_PACKED_TYPE (0x07)
#define NS_HI_PACKED_FLAG (0x38)
#define NS_HI_PACKED_DIR (0xc0)

#define NS_HI_TYPE(nh) ((nh)->packed & NS_HI_PACKED_TYPE)
#define NS_HI_TYPE_SET(nh, t)               \
    {                                       \
        (nh)->packed &= ~NS_HI_PACKED_TYPE; \
        (nh)->packed |= (t);                \
    }

#define NS_HI_TYPE_ROCK 0
#define NS_HI_TYPE_SEWER 1
#define NS_HI_TYPE_STONE 2
#define NS_HI_TYPE_NOTHING 3
#define NS_HI_TYPE_CURVE 4 // Private!

#define NS_HI_FLAG_GRATE (1 << 5)   // A hole through which water can pour.
#define NS_HI_FLAG_LOCKTOP (1 << 6) // Private!
#define NS_HI_FLAG_TOPUSED (1 << 7) // Private!

typedef struct
{
    //
    // In eighth map-squares: From 32 squares below ground...
    // 'bot' should be on map-square boundaries.
    // 'top' is precalculated by NS_precalculate()
    //

    std::uint8_t bot; // bot == 0 => A hole into infinity...
    std::uint8_t top;

    std::uint8_t water; // The height of the water or 0 if there is no water.
                        // No water allowed on rock.

    std::uint8_t packed; // 5:3 FLAGS : TYPE

} NS_Hi;

extern NS_Hi NS_hi[PAP_SIZE_HI][PAP_SIZE_HI];

//
// Each low-res mapsquare has its points and faces pre-calculated
// and cached using the HEAP module. In the sewers, the heap is
// used for the sewer memory not the lighting...
//

typedef struct
{
    std::uint8_t u[4];
    std::uint8_t v[4];

} NS_Texture;

#define NS_MAX_TEXTURES 256

extern NS_Texture NS_texture[NS_MAX_TEXTURES];
extern std::int32_t NS_texture_upto;

//
// The page of a face is an index into this array. It leads to different
// things on the PSX to the PC.
//

#ifdef PSX

//
// The offset into the sewer texture page of each type.
//

typedef struct
{
    std::uint8_t du;
    std::uint8_t dv;

} NS_Page;

#else

//
// The page number to use: the number of .TGA file.
//

typedef struct
{
    std::uint16_t page;

} NS_Page;

#endif

#define NS_PAGE_ROCK 0
#define NS_PAGE_SEWER 1
#define NS_PAGE_STONE 2
#define NS_PAGE_SWALL 3
#define NS_PAGE_GRATE 4
#define NS_PAGE_NUMBER 5

extern NS_Page NS_page[NS_PAGE_NUMBER];

//
// Waterfalls...
//

typedef struct
{
    std::uint8_t x;
    std::uint8_t z;
    std::int8_t dx; // Vector from where the water is coming from.
    std::int8_t dz;
    std::uint8_t top;
    std::uint8_t bot;
    std::uint8_t counter;
    std::uint8_t next;

} NS_Fall;

#define NS_MAX_FALLS 32

extern NS_Fall NS_fall[NS_MAX_FALLS];
extern std::uint8_t NS_fall_free;

typedef struct
{
    std::uint8_t x; // (x << 3, z << 3) relative to the lo-res mapsquare.
    std::uint8_t z;
    std::uint8_t y;      // In eighth map-squares from 32 squares below ground...
    std::uint8_t bright; // No coloured lighting...

} NS_Point;

typedef struct
{
    std::uint8_t p[4];
    std::uint8_t page; // Rock/stone/brick...
    std::uint8_t texture;

} NS_Face;

typedef struct
{
    std::uint8_t next;
    std::uint8_t used;
    std::uint8_t map_x;
    std::uint8_t map_z;
    std::uint8_t *memory;
    std::uint16_t num_points;
    std::uint16_t num_faces; // Face memory starts immediately after point memory.
    std::uint8_t fall;       // Any waterfalls that happen to be in this square.
    std::uint8_t padding;

} NS_Cache;

#define NS_MAX_CACHES 128

extern NS_Cache NS_cache[NS_MAX_CACHES];
extern std::uint8_t NS_cache_free;

//
// The things in the sewers.
//

#define NS_ST_TYPE_UNUSED 0
#define NS_ST_TYPE_PRIM 1
#define NS_ST_TYPE_LADDER 2
#define NS_ST_TYPE_BRIDGE 3
#define NS_ST_TYPE_PLATFORM 4

typedef struct
{
    std::uint8_t type;
    std::uint8_t next;

    union {
        struct
        {
            std::uint8_t prim;
            std::uint8_t yaw;
            std::uint8_t x; // (x<<3,z<<3) relative to the lo-res mapsquare it is in
            std::uint8_t z;
            std::uint8_t y;

        } prim;

        struct
        {
            std::uint8_t x1; // (x,z) are in hi-res mapsquare coordinates.
            std::uint8_t z1;
            std::uint8_t x2;
            std::uint8_t z2;
            std::uint8_t height;

        } ladder;
    };

} NS_St;

#define NS_MAX_STS 64

extern NS_St NS_st[NS_MAX_STS];
extern std::uint8_t NS_st_free;

//
// Each lo-res mapsquare.
//

typedef struct
{
    std::uint8_t cache;
    std::uint8_t st; // Linked list of sewer things above this mapsquare.

    //
    // The position of the light for this lo-res mapsquare. If y == 0 then
    // there is no light on this mapsquare.
    //

    std::uint8_t light_x; // (x << 3, z << 3) relative to the lo-res mapsquare.
    std::uint8_t light_z; // 0 => No light.
    std::uint8_t light_y; // In eighth map-squares from 32 squares below ground.

} NS_Lo;

extern NS_Lo NS_lo[PAP_SIZE_LO][PAP_SIZE_LO];

// ========================================================
//
// EDITOR FUNCTIONS
//
// ========================================================

//
// Creates a new sewer map.
//

void NS_init();

//
// Calculate the top heights of the rock
//

void NS_precalculate();

//
// Add a ladder to the map.  All arguments are given in hi-res mapsquare
// coordinates. The (x,z)s are given in clockwise order.
//

void NS_add_ladder(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2, std::int32_t height);

//
// Adds a prim to the map. It might not go exactly where you want! The
// coordinates are given in 8-bit fixed point.  (x,z) are hi-res fixed 8
// coords and y is in the sewer system y-coordinates, so world_y = (y << 5) + -32 * 0x100
//

void NS_add_prim(
    std::int32_t prim,
    std::int32_t yaw,
    std::int32_t x,
    std::int32_t y,
    std::int32_t z);

//
// When the sewer map is complete.
//

void NS_save(char *fname);
void NS_load(char *fname);

// ========================================================
//
// SEWER INFO.
//
// ========================================================

//
// Returns the height of the sewer (8-bits per mapsquare)
//

std::int32_t NS_calc_height_at(std::int32_t x, std::int32_t z);

//
// Returns either the height of the sewer or the height of the
// water above the sewer (if there is water at (x,z)).
//

std::int32_t NS_calc_splash_height_at(std::int32_t x, std::int32_t z);

//
// Makes the movement vector slide around the sewer system.
// It takes highest-precision movement in 16-bit fixed point!
//

void NS_slide_along(
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2,
    std::int32_t radius); // radius is only fixed-point 8!

//
// Returns true if the given point is inside.
//

std::int32_t NS_inside(std::int32_t x, std::int32_t y, std::int32_t z);

extern std::int32_t NS_los_fail_x;
extern std::int32_t NS_los_fail_y;
extern std::int32_t NS_los_fail_z;

std::int32_t NS_there_is_a_los(
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2);

// ========================================================
//
// DRAWING THE SEWER MAP
//
// ========================================================

//
// Initialises the HEAP module and marks all lo-res mapsquares
// as uncached.  Builds the linked list of cache entries.
//

void NS_cache_init();

//
// Creates a cache entry for the given mapsquare.
// Destroy the given cache entry.
//

std::int32_t NS_cache_create(std::uint8_t mx, std::uint8_t mz);
void NS_cache_destroy(std::uint8_t cache);

//
// Gets rid of all cached data and reinitialises the heap.
//

void NS_cache_fini();

#endif
