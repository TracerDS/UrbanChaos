//
// Draped cloth and flags.
//

#ifndef _CLOTH_
#define _CLOTH_

//
// Gets rid of all the cloths.
//

void CLOTH_init();

//
// Creates a new piece of cloth.  Returns nullptr on failure.
//

#define CLOTH_TYPE_UNUSED 0
#define CLOTH_TYPE_FLAG 1
#define CLOTH_TYPE_CLOAK 2

std::uint8_t CLOTH_create(
    std::uint8_t type,
    std::int32_t ox,
    std::int32_t oy,
    std::int32_t oz,
    std::int32_t wdx, std::int32_t wdy, std::int32_t wdz,
    std::int32_t hdx, std::int32_t hdy, std::int32_t hdz,
    std::int32_t dist,
    std::uint32_t colour);

//
// All cloth is made up of CLOTH_WIDTH x CLOTH_HEIGHT points.
//

#define CLOTH_WIDTH 6
#define CLOTH_HEIGHT 5

//
// Locks the given point. These are the points of the cloth that are
// attached to somewhere.
//

void CLOTH_point_lock(std::uint8_t cloth, std::uint8_t w, std::uint8_t h);

//
// Moves the given cloth point. Moving locked points is probably
// the best idea.
//

void CLOTH_point_move(std::uint8_t cloth, std::uint8_t w, std::uint8_t h, std::int32_t x, std::int32_t y, std::int32_t z);

//
// Processes all the cloth.
//

void CLOTH_process();

// ========================================================
//
// DRAWING THE CLOTH
//
// ========================================================

//
// Accessing the cloth on the mapwho.
//

std::uint8_t CLOTH_get_first(std::uint8_t lo_map_x, std::uint8_t lo_map_z);

//
// The index of cloth point (w,h)
//

#define CLOTH_INDEX(w, h) ((w) + (h) * CLOTH_WIDTH)

//
// Returns an array of CLOTH_WIDTH x CLOTH_HEIGHT points
// and normals.
//

typedef struct
{
    float x;
    float y;
    float z;
    float nx; // (nx,ny,nz) is normalised
    float ny;
    float nz;

} CLOTH_Drawp;

typedef struct
{
    std::uint8_t type;
    std::uint8_t next; // The next cloth in the linked list hanging of the mapsquare.
    std::uint16_t padding;
    std::uint32_t colour;

    CLOTH_Drawp p[CLOTH_WIDTH * CLOTH_HEIGHT];

} CLOTH_Info;

CLOTH_Info *CLOTH_get_info(std::uint8_t cloth);

#endif
