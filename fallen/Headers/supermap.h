#ifndef SUPERMAP_H
#define SUPERMAP_H 1

#ifdef PSX
//
// PSX include
//
#include "libsn.h"
/*
#define	MFFileHandle	std::int32_t
#define	FILE_OPEN_ERROR	(-1)
#define	SEEK_MODE_CURRENT	(1)

#define	FileOpen(x)		PCopen(x,0,0)
#define	FileClose(x)	PCclose(x)
#define	FileCreate(x,y)	PCopen(x,1,0)
#define	FileRead(h,a,s) PCread(h,(char*)a,s)
#define	FileWrite(h,a,s) PCwrite(h,(char*)a,s)
#define	FileSeek(h,m,o) PClseek(h,o,m)

#endif
*/

#define MFFileHandle std::int32_t
#define FILE_OPEN_ERROR (-1)
#define SEEK_MODE_CURRENT (1)

extern std::int32_t SpecialOpen(char* name);
extern std::int32_t SpecialRead(std::int32_t handle, std::uint8_t* ptr, std::int32_t s1);
extern std::int32_t SpecialSeek(std::int32_t handle, std::int32_t mode, std::int32_t size);

#define FileOpen(x) SpecialOpen(x)
#define FileClose(x) SpecialClose(x)
#define FileCreate(x, y) ASSERT(0)
#define FileRead(h, a, s) SpecialRead(h, (char*) a, s)
#define FileWrite(h, a, s) ASSERT(0)
#define FileSeek(h, m, o) SpecialSeek(h, m, o)

#endif

#define MAX_FACET_LINK 32000

struct DStorey {
    std::uint16_t Style; // replacement style           // maybe this could be a byte
    std::uint16_t Index; // Index to painted info
    std::int8_t Count;   //+ve is a style  //-ve is a  //get rid of this
    std::uint8_t BloodyPadding;
};

struct DFacet {
    std::uint8_t FacetType;
    std::uint8_t Height;
    std::uint8_t x[2]; // these are bytes because they are grid based
    std::int16_t Y[2];
    std::uint8_t z[2]; // these are bytes because they are grid based
    std::uint16_t FacetFlags;
    std::uint16_t StyleIndex;
    std::uint16_t Building;
    std::uint16_t DStorey;
    std::uint8_t FHeight;
    std::uint8_t BlockHeight;
    std::uint8_t Open;    // How open or closed a STOREY_TYPE_OUTSIDE_DOOR is.
    std::uint8_t Dfcache; // Index into NIGHT_dfcache[] or nullptr...
    std::uint8_t Shake;   // When a fence has been hit hard by something.
    std::uint8_t CutHole;
    std::uint8_t Counter[2];
};

struct DBuilding {
    std::int32_t X, Y, Z;
    std::uint16_t StartFacet;
    std::uint16_t EndFacet;
    std::uint16_t Walkable;
    std::uint8_t Counter[2];
    std::uint16_t Padding;
    std::uint8_t Ware; // If this building is a warehouse, this is an index into the WARE_ware[] array
    std::uint8_t Type;
};

struct DWalkable {
    std::uint16_t StartPoint; // Unused nowadays
    std::uint16_t EndPoint;   // Unused nowadays
    std::uint16_t StartFace3; // Unused nowadays
    std::uint16_t EndFace3;   // Unused nowadays

    std::uint16_t StartFace4; // These are indices into the roof faces
    std::uint16_t EndFace4;

    std::uint8_t X1;
    std::uint8_t Z1;
    std::uint8_t X2;
    std::uint8_t Z2;
    std::uint8_t Y;
    std::uint8_t StoreyY;
    std::uint16_t Next;
    std::uint16_t Building;
};

struct DInsideRect {
    std::uint8_t MapX;
    std::uint8_t MapZ;
    std::uint8_t Width;
    std::uint8_t Depth;
    std::uint8_t StoreyY;
    std::uint8_t Flags;     // bound to need flags plus it pads us out nicely
    std::uint16_t BitIndex; // index to block of data for inside buildings
};

// bits 0->3      16 room ID's   0== no entry
// bits 4->5      Direction
// bits 6->7      Type

#define CALC_INSIDE_ID(id) (id & 3)
#define CALC_INSIDE_DIRECTION(id) ((id & 3) << 4)
#define CALC_INSIDE_TYPE(id) ((id & 3) << 6)

#define GET_INSIDE_ID(id) (id & 3)
#define GET_INSIDE_DIRECTION(id) ((id >> 4) & 3)
#define GET_INSIDE_TYPE(id) ((id >> 6) & 3)

extern std::int16_t next_paint_mem;
extern std::int16_t next_dstorey;
extern std::int32_t next_inside_mem;

extern std::int16_t next_facet_link;
extern std::int16_t facet_link_count;

#define MAX_DBUILDINGS 1024
#define MAX_DFACETS 16384
#define MAX_DWALKABLES 2048
#define MAX_DSTYLES 10000
#define MAX_DSTOREYS 10000
#define MAX_PAINTMEM 64000

extern std::int32_t next_dwalkable;
extern std::int32_t next_dbuilding;
extern std::int32_t next_dfacet;
extern std::int32_t next_dstyle;

void save_super_map(MFFileHandle handle);
void load_super_map(MFFileHandle handle, std::int32_t st);

//
// Identifies the subset of the primpoints that are used
// by the walkable faces.
//

extern std::int32_t first_walkable_prim_point;
extern std::int32_t number_of_walkable_prim_points;

extern std::int32_t first_walkable_prim_face4;
extern std::int32_t number_of_walkable_prim_faces4;

//
// Adds a sewer ladder facet.  If (link) then this is a ladder that
// links the sewers to the outside world.
//

void add_sewer_ladder(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t bottom,
    std::int32_t height,
    std::int32_t link);

//
// Finds the nearest electric fence DBUILDING to the given point.
// Returns nullptr if it couldn't find one withing the given range.
//

std::int32_t find_electric_fence_dbuilding(
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z,
    std::int32_t range);

//
// Sets the state of the given electric fence dbuilding. It sets the
// flags in all the facets of the dbuilding.
//

void set_electric_fence_state(std::int32_t dbuilding, std::int32_t onoroff);

// ========================================================
//
// TO THINGS TO DFACETS ONLY ONCE
//
// ========================================================

extern std::uint8_t SUPERMAP_counter[2]; // One for each camera...

//
// This function increases the SUPERMAP_counter to a value that it guarantees
// will not be present in any of the dfacets or dbuildings Counter[] arrays.
//

void SUPERMAP_counter_increase(std::uint8_t which);

// ========================================================
//
// EDITOR HACK
//
//
// This is a hack put it to let the game ENTER, STAIR and ID modules
// work on buildings in the editor.
//
// It converts just the given building into the supermap structure
// and puts it into dbulding[1]
//

void create_super_dbuilding(std::int32_t building);

//
// ========================================================

#endif
