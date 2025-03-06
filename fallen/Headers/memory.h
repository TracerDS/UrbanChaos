#ifndef MEMORY_H
#define MEMORY_H

#include "supermap.h"
#include "inside2.h"
#include "building.h"

//
// The map beacons.
//

typedef struct
{
    std::uint8_t used;
    std::uint8_t counter;
    std::uint16_t track_thing;
    std::uint16_t index;
    std::uint16_t pad;
    //	char* str;
    std::int32_t wx;
    std::int32_t wz;
    std::uint32_t ticks; // The GetTickCount when we created the beacon. LAZY!

} MAP_Beacon;

#define MAP_MAX_BEACONS 32
extern MAP_Beacon *MAP_beacon; //[MAP_MAX_BEACONS];

typedef std::uint16_t PSX_TEX[5];

extern PSX_TEX *psx_textures_xy; //[PAP_SIZE_LO][PAP_SIZE_LO];

extern std::int16_t *facet_links; //[MAX_FACET_LINK];

extern struct DBuilding *dbuildings; //[MAX_DBUILDINGS];
extern struct DFacet *dfacets;       //[MAX_DFACETS	 ];
extern struct DWalkable *dwalkables; //[MAX_DWALKABLES];
extern std::int16_t *dstyles;        //[MAX_DSTYLES	 ];
extern struct DStorey *dstoreys;     //[MAX_DSTOREYS];

extern std::uint8_t *paint_mem; //[MAX_PAINTMEM];

//
// from inside2
//

extern struct InsideStorey *inside_storeys; //[MAX_INSIDE_RECT];
extern struct Staircase *inside_stairs;     //[MAX_INSIDE_STAIRS];
extern std::uint8_t *inside_block;          //[MAX_INSIDE_MEM];
extern std::uint8_t inside_tex[64][16];

//
// from building.cpp
//

extern struct BoundBox *roof_bounds;               //[MAX_ROOF_BOUND];
extern struct PrimPoint *prim_points;              //[MAX_PRIM_POINTS];
extern struct PrimFace4 *prim_faces4;              //[MAX_PRIM_FACES4];
extern struct PrimFace3 *prim_faces3;              //[MAX_PRIM_FACES3];
extern struct PrimObject *prim_objects;            //[MAX_PRIM_OBJECTS];
extern struct PrimMultiObject *prim_multi_objects; //[MAX_PRIM_MOBJECTS];
extern PrimNormal *prim_normal;                    //[MAX_PRIM_POINTS];

extern PrimInfo *prim_info; //[256];//MAX_PRIM_OBJECTS];

// functions
extern void init_memory();

// extern OB_Mapwho OB_mapwho;//[OB_SIZE][OB_SIZE];

#define MAX_ROOF_FACE4 10000
extern std::uint16_t next_roof_face4;
extern struct RoofFace4 *roof_faces4;

#ifndef PSX

//
// Quick load\save
//

void MEMORY_quick_init();

void MEMORY_quick_save();

std::int32_t MEMORY_quick_load_available();
std::int32_t MEMORY_quick_load();

#endif

#endif
