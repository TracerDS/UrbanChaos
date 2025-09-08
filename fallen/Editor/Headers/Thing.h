// Thing.h
// Guy Simmons, 15th October 1997.

#ifndef ETHING_H
#define ETHING_H

#include "anim.h"
#include "DarkCity.h"

//---------------------------------------------------------------
// old style

// defs
#define MAX_MAP_THINGS 2000

#define MAP_THING_TYPE_PRIM 1
#define MAP_THING_TYPE_MULTI_PRIM 2
#define MAP_THING_TYPE_ROT_MULTI 3
#define MAP_THING_TYPE_SPRITE 4
#define MAP_THING_TYPE_AGENT 5
#define MAP_THING_TYPE_LIGHT 6
#define MAP_THING_TYPE_BUILDING 7
#define MAP_THING_TYPE_ANIM_PRIM 8

// Game editor stuff.
#define MAP_THING_TYPE_ED_THING 8

#define TO_MTHING(m) &map_things[(m)]

// structs
struct MapThing {
    std::int32_t X;
    std::int32_t Y;
    std::int32_t Z;
    std::uint16_t MapChild; // mapwho 2 way linked list
    std::uint16_t MapParent;
    std::uint8_t Type;
    std::uint8_t SubType; // Type for lights...
    std::uint32_t Flags;

    union {
        struct
        {
            std::int16_t IndexOther; // Brightness for lights...
            std::uint16_t Width;
            std::uint16_t Height;
            std::uint16_t IndexOrig; // param for lights...
            std::uint16_t AngleX;    // (R,G,B) for lights...
            std::uint16_t AngleY;
            std::uint16_t AngleZ;
            std::uint16_t IndexNext;
            std::int16_t LinkSame;
            std::int16_t OnFace;
            std::int16_t State;
            std::int16_t SubState;
            std::int32_t BuildingList;
            std::uint32_t EditorFlags,
                EditorData;
            std::uint32_t DummyArea[3];
            std::int32_t TweenStage;
            // struct KeyFrameElement		*AnimElements,
            //								*NextAnimElements;
            KeyFrame *CurrentFrame;
            KeyFrame *NextFrame;
        };

        struct
        {
            std::uint16_t CommandRef;
            std::int32_t Class,
                Genus;
            std::int32_t Data[6];
        };
    };
};

struct MapThingPSX {
    std::int32_t X;
    std::int32_t Y;
    std::int32_t Z;
    std::uint16_t MapChild; // mapwho 2 way linked list
    std::uint16_t MapParent;
    std::uint8_t Type;
    std::uint8_t SubType; // Type for lights...
    std::uint32_t Flags;

    std::int16_t IndexOther; // Brightness for lights...
    std::uint16_t Width;
    std::uint16_t Height;
    std::uint16_t IndexOrig; // param for lights...
    std::uint16_t AngleX;    // (R,G,B) for lights...
    std::uint16_t AngleY;
    std::uint16_t AngleZ;
    std::uint16_t IndexNext;
    std::int16_t LinkSame;
    std::int16_t OnFace;
    std::int16_t State;
    std::int16_t SubState;
    std::int32_t BuildingList;
    std::uint32_t EditorFlags,
        EditorData;
    std::uint32_t DummyArea[3];
    std::int32_t TweenStage;
    // struct KeyFrameElement		*AnimElements,
    //								*NextAnimElements;
    KeyFrame *CurrentFrame;
    KeyFrame *NextFrame;
};

// data
extern struct MapThing map_things[MAX_MAP_THINGS];

// code
extern std::uint16_t find_empty_map_thing();
extern void delete_thing_from_edit_map(std::int32_t x, std::int32_t y, std::uint16_t thing);
extern void add_thing_to_edit_map(std::int32_t x, std::int32_t y, std::uint16_t thing);
extern std::int32_t move_thing_on_cells(std::uint16_t thing, std::int32_t x, std::int32_t y, std::int32_t z);
extern void delete_thing(std::int16_t index);
//---------------------------------------------------------------
/*
void			init_things();
THING_INDEX		alloc_primary_thing();
void			free_primary_thing(THING_INDEX thing);
THING_INDEX		alloc_secondary_thing();
void		free_secondary_thing(THING_INDEX thing);
*/
//---------------------------------------------------------------

#endif
