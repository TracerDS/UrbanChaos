#include "Editor.hpp"

#include "BuildTab.hpp"
#include "engine.h"
#include "c:\fallen\headers\enter.h"
#include "c:\fallen\headers\id.h"
#include "extra.h"
#include "c:\fallen\headers\supermap.h"
#include "outline.h"

// #pragma	warning	14 9

// static		counter;
// #define		ShowWorkWindow(x)	{DrawLineC(0+(counter-1)&255,0,WorkWindowWidth-1,WorkWindowHeight-1,0);DrawLineC(0+(counter++)&255,0,WorkWindowWidth-1,WorkWindowHeight-1,255);DrawLineC(0,WorkWindowHeight-1,WorkWindowWidth-1,0,255); ShowWorkWindow(x);}

//---------------------------------------------------------------
// debug stuff
/*
void	cross_work_window()
{
        DrawLineC(0,0,WorkWindowWidth-1,WorkWindowHeight-1,255);
        DrawLineC(0,WorkWindowHeight-1,WorkWindowWidth-1,0,255);

}
*/
//---------------------------------------------------------------

#define CTRL_BUILD_TEXTURE 1
#define CTRL_BUILD_Y_AXIS_FREE 2
#define CTRL_BUILD_Z_AXIS_FREE 3
#define CTRL_BUILD_NEW_BUILDING 4
#define CTRL_BUILD_NEXT_STOREY 5
#define CTRL_BUILD_DUPLICATE_STOREY 6
#define CTRL_TOGGLE_TILED_ROOF 7
#define CTRL_ADD_FLAT_ROOF_QUAD 8
#define CTRL_ADD_FIRE_ESCAPE 9
#define CTRL_ADD_POKEY 10
#define CTRL_DELETE_STOREY 11
#define CTRL_ADD_STAIRCASE 12
#define CTRL_ADD_SKYLIGHT 13
#define CTRL_ADD_CABLE 14
#define CTRL_BUILD_CREATE_BUILDING 15
#define CTRL_ADD_LADDER 16
#define CTRL_DELETE_BUILDING 17
#define CTRL_NEW_FENCE 18
#define CTRL_ANOTHER_INSIDE_SEED 19
#define CTRL_PREV_INSIDE_SEED 20
#define CTRL_ANOTHER_STAIRCASE_SEED 21
#define CTRL_PREV_STAIRCASE_SEED 22
#define CTRL_NEXT_STOREY 23
#define CTRL_PREV_STOREY 24
#define CTRL_BUILDING_TYPE_HOUSE 25
#define CTRL_BUILDING_TYPE_WAREHOUSE 26
#define CTRL_BUILDING_TYPE_OFFICE 27
#define CTRL_BUILDING_TYPE_APARTEMENT 28
#define CTRL_BUILDING_TYPE_CRATE_IN 29
#define CTRL_BUILDING_TYPE_CRATE_OUT 30
#define CTRL_ADD_TRENCH 31
#define CTRL_SET_STOREY_HEIGHT_64 32
#define CTRL_SET_STOREY_HEIGHT_128 33
#define CTRL_SET_STOREY_HEIGHT_196 34
#define CTRL_SET_STOREY_HEIGHT_256 35
#define CTRL_TOGGLE_FLAT_TILED_ROOF 36
#define CTRL_DEL_ALL 37
#define CTRL_LOAD_TEXTURES 38
#define CTRL_CUT_BUILDING 39
#define CTRL_PASTE_BUILDING 40
#define CTRL_SET_STOREY_TYPE_NORMAL 41
#define CTRL_SET_STOREY_TYPE_FENCE 42
#define CTRL_SCROLL_MAP_UP 43
#define CTRL_SCROLL_MAP_DOWN 44
#define CTRL_SCROLL_MAP_LEFT 45
#define CTRL_SCROLL_MAP_RIGHT 46

#define BUILD_MODE_WAIT 1
#define BUILD_MODE_PLACE_STOREY 2

#define BUILD_MODE_CONT_STOREY 3
#define BUILD_MODE_EDIT_STOREY 4

char *storey_name[] =
    {
        "ZERO",
        "NORMAL",
        "ROOF",
        "WALL",
        "ROOF QUAD",
        "FLOOR POINTS",
        "FIRE ESCAPE",
        "STAIRCASE",
        "SKYLIGHT",
        "CABLE",
        "ANGLE FENCE",
        "BRICKWALL",
        "LADDER",
        "FLAT FENCE",
        "TRENCH",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
};

ControlDef build_tab_def[] =
    {
        {CHECK_BOX, 0, "Textures", 180, 200 - 50, 0, 10},
        {CHECK_BOX, 0, "Y Free", 120, 213 - 10, 0, 10},
        {CHECK_BOX, 0, "Z Free", 120, 226 - 10, 0, 10},
        {BUTTON, 0, "New Building", 10, 40, 0, 0},
        {BUTTON, 0, "Next Storey", 10, 80, 0, 0},
        {BUTTON, 0, "Duplicate Storey", 10, 100, 0, 0},
        {BUTTON, 0, "Toggle Tiled Roof", 10, 120, 0, 0},
        {BUTTON, 0, "Add Flat Roof Quad", 10, 140, 0, 0},
        {BUTTON, 0, "Add a FireEscape", 10, 160, 0, 0},
        {BUTTON, 0, "Add a Ledge to Storey", 10, 180, 0, 0},
        {BUTTON, 0, "Delete Storey", 180, 40, 0, 0},
        {BUTTON, 0, "Add a StairCase", 10, 200, 0, 0},
        {BUTTON, 0, "Add a SkyLight", 10, 220, 0, 0},
        {BUTTON, 0, "Add cable", 10, 240, 0, 0},
        {BUTTON, 0, "Create Building", 10, 260, 0, 0},
        {BUTTON, 0, "Add Ladder", 10, 280, 0, 0},
        {BUTTON, 0, "Delete Building", 180, 60, 0, 0},
        {BUTTON, 0, "New Fence", 10, 60, 0, 0},
        {BUTTON, 0, "Another  inside seed", 180, 180, 0, 0},
        {BUTTON, 0, "Previous inside seed", 180, 200, 0, 0},
        {BUTTON, 0, "Another  staircase seed", 180, 220, 0, 0},
        {BUTTON, 0, "Previous staircase seed", 180, 240, 0, 0},
        {BUTTON, 0, "Edit Next Storey", 180, 320, 0, 0},
        {BUTTON, 0, "Edit Prev Storey", 180, 340, 0, 0},
        {CHECK_BOX, 0, "House", 10, 320, 0, 10},
        {CHECK_BOX, 0, "Warehouse", 10, 335, 0, 10},
        {CHECK_BOX, 0, "Office", 10, 350, 0, 10},
        {CHECK_BOX, 0, "Apartement", 10, 365, 0, 10},
        {CHECK_BOX, 0, "Crate Inside", 10, 380, 0, 10},
        {CHECK_BOX, 0, "Crate Outside", 10, 395, 0, 10},
        {BUTTON, 0, "Add a Trench", 180, 160, 0, 0},
        {BUTTON, 0, "Storey Height quart", 180, 360, 0, 0},
        {BUTTON, 0, "Storey Height half", 180, 375, 0, 0},
        {BUTTON, 0, "Storey Height 3quart", 180, 390, 0, 0},
        {BUTTON, 0, "Storey Height normal", 180, 405, 0, 0},
        {BUTTON, 0, "Toggle Flat Tiled Roof", 170, 120, 0, 0},
        {BUTTON, 0, "Delete ALL BUILDS", 180, 80, 0, 0},
        {BUTTON, 0, "Load Textures from map", 180, 100, 0, 0},
        {BUTTON, 0, "Cut Building", 30, 450, 0, 0},
        {BUTTON, 0, "Paste Building", 110, 450, 0, 0},
        {BUTTON, 0, "Storey Type Normal", 180, 425, 0, 0},
        {BUTTON, 0, "Storey Type Fence", 180, 440, 0, 0},
        {BUTTON, 0, "Up", 170, 260, 0, 0},
        {BUTTON, 0, "Down", 160, 300, 0, 0},
        {BUTTON, 0, "Left", 140, 280, 0, 0},
        {BUTTON, 0, "Right", 180, 280, 0, 0},
        {0}
};

BuildTab *the_build;

//
// The building the stairs have been calculated for.
// The storey the inside building stuff has been calculated for.
// true => the inside stuff is valid.
//

std::int32_t inside_building;
std::int32_t inside_storey;
std::int32_t inside_valid;
std::int32_t inside_failure;

#define MAX_SEED_BACKUPS 16

std::int32_t seed_inside[MAX_SEED_BACKUPS];
std::int32_t seed_stairs[MAX_SEED_BACKUPS];

std::int32_t seed_inside_upto;
std::int32_t seed_stairs_upto;

class BuildingBlock {
    std::int32_t WallCount;
    std::int32_t StoreyCount;
    std::int32_t X;
    std::int32_t Z;
    struct FWall *PWall;
    struct FStorey *PStorey;
    struct FBuilding PBuilding;

   public:
    BuildingBlock();
    ~BuildingBlock();
    void Cut(std::int32_t building);
    void Paste(std::int32_t x, std::int32_t z, std::int32_t flags);
    void Allocate(std::int32_t building);
    void Free(std::int32_t clear_textures);
    inline std::int32_t GetX() { return (X); }
    inline std::int32_t GetZ() { return (Z); }
};

BuildingBlock building_block;

BuildingBlock::BuildingBlock() {
    PWall = 0;
    PStorey = 0;
}

BuildingBlock::~BuildingBlock() {
    if (PWall)
        Free(0);
}

void BuildingBlock::Free(std::int32_t clear_textures) {
    if (PWall) {
        std::int32_t c0;
        if (clear_textures)
            for (c0 = 1; c0 < WallCount; c0++) {
                if (PWall[c0].Textures && PWall[c0].Tcount) {
                    MemFree((std::uint8_t *) PWall[c0].Textures);
                }

                if (PWall[c0].Textures2 && PWall[c0].Tcount2) {
                    MemFree((std::uint8_t *) PWall[c0].Textures2);
                }
            }
        MemFree((std::uint8_t *) PWall);
    }
    if (PStorey)
        MemFree((std::uint8_t *) PStorey);

    WallCount = 0;
    StoreyCount = 0;
}

void BuildingBlock::Allocate(std::int32_t building) {
    std::int32_t wall, storey;
    std::int32_t nwall = 1, nstorey = 1;
    Free(1);

    storey = building_list[building].StoreyHead;
    while (storey) {
        wall = storey_list[storey].WallHead;
        while (wall) {
            wall = wall_list[wall].Next;
            nwall++;
        }
        storey = storey_list[storey].Next;
        nstorey++;
    }

    WallCount = nwall;
    StoreyCount = nstorey;
    PWall = (struct FWall *) MemAlloc(nwall * sizeof(struct FWall));
    PStorey = (struct FStorey *) MemAlloc(nstorey * sizeof(struct FStorey));
}

void BuildingBlock::Cut(std::int32_t building) {
    std::int32_t wall, storey;
    std::int32_t nwall = 1, nstorey = 1;
    Allocate(building);

    storey = building_list[building].StoreyHead;
    while (storey) {
        PStorey[nstorey] = storey_list[storey];
        if (storey_list[storey].Next) {
            PStorey[nstorey].Next = nstorey + 1;
        } else {
            PStorey[nstorey].Next = 0;
        }
        if (storey_list[storey].WallHead) {
            PStorey[nstorey].WallHead = nwall;
        }

        wall = storey_list[storey].WallHead;
        while (wall) {
            PWall[nwall] = wall_list[wall];
            if (wall_list[wall].Next) {
                PWall[nwall].Next = nwall + 1;
            } else {
                PWall[nwall].Next = 0;
            }
            if (wall_list[wall].Textures && wall_list[wall].Tcount) {
                PWall[nwall].Textures = (std::uint8_t *) MemAlloc(wall_list[wall].Tcount);
                if (PWall[nwall].Textures)
                    memcpy(PWall[nwall].Textures, wall_list[wall].Textures, wall_list[wall].Tcount);
            } else {
                PWall[nwall].Textures = 0;
                PWall[nwall].Tcount = 0;
            }
            if (wall_list[wall].Textures2 && wall_list[wall].Tcount2 && wall_list[wall].Tcount2 < 64) {
                PWall[nwall].Textures2 = (std::uint8_t *) MemAlloc(wall_list[wall].Tcount2);
                if (PWall[nwall].Textures2)
                    memcpy(PWall[nwall].Textures2, wall_list[wall].Textures2, wall_list[wall].Tcount2);

            } else {
                PWall[nwall].Textures2 = 0;
                PWall[nwall].Tcount2 = 0;
            }

            wall = wall_list[wall].Next;
            nwall++;
        }
        storey = storey_list[storey].Next;
        nstorey++;
    }
    PBuilding = building_list[building];
    PBuilding.StoreyHead = 1;
}

void BuildingBlock::Paste(std::int32_t x, std::int32_t z, std::int32_t flags) {
    std::int32_t storey, wall, nstorey, nwall, nbuilding;
    std::int32_t dx, dz;

    dx = x - PStorey[1].DX;
    dz = z - PStorey[1].DZ;
    if (PWall) {
        nbuilding = get_new_building();
        if (nbuilding) {
            std::int32_t prev_nstorey = 0;

            building_list[nbuilding] = PBuilding;

            storey = 1;
            while (storey) {
                std::int32_t prev_nwall = 0;

                nstorey = get_new_storey();

                if (prev_nstorey) {
                    storey_list[prev_nstorey].Next = nstorey;
                } else {
                    building_list[nbuilding].StoreyHead = nstorey;
                }

                storey_list[nstorey] = PStorey[storey];
                storey_list[nstorey].BuildingHead = nbuilding;
                storey_list[nstorey].DX += dx;
                storey_list[nstorey].DZ += dz;

                wall = PStorey[storey].WallHead;

                while (wall) {
                    nwall = get_new_wall();
                    if (prev_nwall) {
                        wall_list[prev_nwall].Next = nwall;
                    } else {
                        storey_list[nstorey].WallHead = nwall;
                    }

                    wall_list[nwall] = PWall[wall];
                    wall_list[nwall].StoreyHead = nstorey;
                    wall_list[nwall].DX += dx;
                    wall_list[nwall].DZ += dz;

                    if (PWall[wall].Textures && PWall[wall].Tcount) {
                        wall_list[nwall].Textures = (std::uint8_t *) MemAlloc(wall_list[nwall].Tcount);
                        if (wall_list[nwall].Textures)
                            memcpy(wall_list[nwall].Textures, PWall[wall].Textures, wall_list[nwall].Tcount);
                    } else {
                        wall_list[nwall].Textures = 0;
                        wall_list[nwall].Tcount = 0;
                    }

                    if (PWall[wall].Textures2 && PWall[wall].Tcount2) {
                        wall_list[nwall].Textures2 = (std::uint8_t *) MemAlloc(wall_list[nwall].Tcount2);
                        if (wall_list[nwall].Textures2)
                            memcpy(wall_list[nwall].Textures2, PWall[wall].Textures2, wall_list[nwall].Tcount2);
                    } else {
                        wall_list[nwall].Textures2 = 0;
                        wall_list[nwall].Tcount2 = 0;
                    }

                    wall = PWall[wall].Next;

                    prev_nwall = nwall;
                }

                storey = PStorey[storey].Next;
                prev_nstorey = nstorey;
            }
        }
    }
}

//---------------------------------------------------------------
// mapblock cut and paste stuff
//---------------------------------------------------------------
MapBlock::MapBlock() {
    Ptr = 0;
}

MapBlock::~MapBlock() {
    if (Ptr)
        Free();
}

void MapBlock::Free() {
    MemFree((std::uint8_t *) Ptr);
}

void MapBlock::Allocate(std::int32_t w, std::int32_t h) {
    if (Ptr)
        Free();

    Ptr = (struct DepthStrip *) MemAlloc(w * h * sizeof(struct DepthStrip));
}

void MapBlock::Cut(std::int32_t x, std::int32_t z, std::int32_t w, std::int32_t d, std::int32_t mode) {
    std::int32_t mx, mz;
    struct DepthStrip *ptr;
    if (Ptr)
        Free();
    Allocate(w, d);

    ptr = Ptr;
    Width = w;
    Depth = d;
    X = x;
    Z = z;

    if (ptr) {
        for (mz = z; mz < z + d; mz++)
            for (mx = x; mx < x + w; mx++) {
                if (mode) {
                    ptr->Y = edit_map_roof_height[mx][mz];
                    ptr++;
                } else {
                    *ptr++ = edit_map[mx][mz];
                }
            }
    }
}

void MapBlock::Paste(std::int32_t x, std::int32_t z, std::int32_t flags, std::int32_t mode) {
    std::int32_t mx, mz;
    struct DepthStrip *ptr;
    if (Ptr == 0)
        return;

    ptr = Ptr;

    for (mz = z; mz < z + Depth; mz++)
        for (mx = x; mx < x + Width; mx++) {
            if (mx >= 0 && mz >= 0 && mx < EDIT_MAP_WIDTH && mz < EDIT_MAP_DEPTH) {
                if (mode) {
                    if (flags & PASTE_ALTITUDE)
                        edit_map_roof_height[mx][mz] = ptr->Y;
                } else {
                    if (flags & PASTE_TEXTURE)
                        edit_map[mx][mz].Texture = ptr->Texture;
                    if (flags & PASTE_ALTITUDE)
                        edit_map[mx][mz].Y = ptr->Y;
                }
            }
            ptr++;
        }
}

void MapBlock::Rotate(std::int32_t dir) {
    std::int32_t dx, dz;
    std::int32_t temp;
    struct DepthStrip *ptr, *ptr_new;
    if (Ptr == 0)
        return;

    ptr_new = (struct DepthStrip *) MemAlloc(Width * Depth * sizeof(struct DepthStrip));

    ptr = Ptr;

    for (dz = 0; dz < Depth; dz++)
        for (dx = 0; dx < Width; dx++) {
            std::int32_t new_dx, new_dz;
            struct MiniTextureBits *tex;

            tex = (struct MiniTextureBits *) (&edit_map[selected_face.MapX][selected_face.MapZ].Texture);

            new_dx = Depth - dz - 1;
            new_dz = dx;
            ptr_new[new_dx + new_dz * Depth] = *ptr;
            tex = (struct MiniTextureBits *) (&ptr_new[new_dx + new_dz * Depth].Texture);
            tex->Rot--;
            ptr_new[new_dx + new_dz * Depth].Texture = *(std::uint16_t *) tex;

            ptr++;
        }
    temp = Width;
    Width = Depth;
    Depth = temp;
    Free();
    Ptr = ptr_new;
}
//-----------------------------------------------------------------------------

std::int16_t get_new_window() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_WINDOWS; c0++) {
        if (window_list[c0].WindowFlags == 0) {
            return (c0);
        }
    }
    return (0);
}

std::int16_t get_new_wall() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_WALLS; c0++) {
        if (wall_list[c0].WallFlags == 0) {
            wall_list[c0].Next = 0;
            wall_list[c0].Tcount = 0;
            wall_list[c0].Textures = 0;
            return (c0);
        }
    }
    return (0);
}

void free_wall(std::int16_t wall) {
    if (wall_list[wall].Tcount && wall_list[wall].Textures) {
        MemFree((void *) wall_list[wall].Textures);
        wall_list[wall].Textures = 0;
        wall_list[wall].Tcount = 0;
    }
    wall_list[wall].WallFlags = 0;
    wall_list[wall].Next = 0;
}

void delete_all() {
    memset((std::uint8_t *) wall_list, 0, sizeof(struct FWall) * MAX_WALLS);
    memset((std::uint8_t *) storey_list, 0, sizeof(struct FStorey) * MAX_STOREYS);
    memset((std::uint8_t *) building_list, 0, sizeof(struct FBuilding) * MAX_BUILDINGS);
}

std::int16_t get_new_storey() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_STOREYS; c0++) {
        if (storey_list[c0].StoreyFlags == 0) {
            storey_list[c0].InsideStorey = 0;
            storey_list[c0].InsideIDIndex = 0;
            storey_list[c0].WallHead = 0;
            storey_list[c0].Next = 0;
            return (c0);
        }
    }
    return (0);
}

void free_storey(std::int16_t storey) {
    storey_list[storey].StoreyFlags = 0;
    storey_list[storey].WallHead = 0;
    storey_list[storey].Next = 0;
}

std::int16_t get_new_building() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_BUILDINGS; c0++) {
        if (building_list[c0].BuildingFlags == 0) {
            building_list[c0].StoreyHead = 0;
            building_list[c0].StoreyCount = 0;
            return (c0);
        }
    }
    return (0);
}

void clear_build_stuff() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_BUILDINGS; c0++) {
        building_list[c0].BuildingFlags = 0;
    }
    for (c0 = 1; c0 < MAX_STOREYS; c0++) {
        storey_list[c0].StoreyFlags = 0;
    }
    for (c0 = 1; c0 < MAX_WALLS; c0++) {
        wall_list[c0].WallFlags = 0;
    }
}

BuildTab::BuildTab(EditorModule *parent) {
    Parent = parent;

    InitControlSet(build_tab_def);
    AxisMode = 3;

    //	SetControlState(CTRL_BUILD_X_AXIS_FREE,CTRL_SELECTED);
    SetControlState(CTRL_BUILD_Y_AXIS_FREE, CTRL_SELECTED);
    SetControlState(CTRL_BUILD_Z_AXIS_FREE, CTRL_SELECTED);
    //	((CVSlider*)GetControlPtr(CTRL_PRIM_V_SLIDE_PRIM))->SetUpdateFunction(redraw_all_prims);

    Axis = X_AXIS | Y_AXIS | Z_AXIS;
    Mode = 0;
    ViewSize = 16;
    ViewX = 0 + (64 << ELE_SHIFT);
    ViewY = 0;
    ViewZ = 0 + (64 << ELE_SHIFT);
    GridFlag = 0;
    Texture = 0;
    EditBuilding = 0;
    EditStorey = 0;
    EditY = 0;
    EditWall = 0;

    inside_building = 0;
    inside_storey = 0;
    inside_valid = false;
    inside_failure = false;

    the_build = this;
    ResetBuildTab();
}

BuildTab::~BuildTab() {
}

void BuildTab::ResetBuildTab() {
    std::int32_t c0, storey;

    Mode = BUILD_MODE_WAIT;
    EditStorey = 0;
    EditWall = 0;
    EditWindow = 0;
    EditY = 0;

    for (c0 = 1; c0 < MAX_BUILDINGS; c0++) {
        if (building_list[c0].BuildingFlags & 1) {
            EditBuilding = c0;
            storey = building_list[c0].StoreyHead;
            if (storey) {
                ViewX = storey_list[storey].DX;
                ViewZ = storey_list[storey].DZ;
            }
        }
    }
}

void BuildTab::Clear() {
    //	clear_all_col_info();
}

void BuildTab::DrawTabContent() {
    //
    // Make sure the building type buttons look proper...
    //

    SetControlState(CTRL_BUILDING_TYPE_HOUSE, CTRL_DESELECTED);
    SetControlState(CTRL_BUILDING_TYPE_WAREHOUSE, CTRL_DESELECTED);
    SetControlState(CTRL_BUILDING_TYPE_OFFICE, CTRL_DESELECTED);
    SetControlState(CTRL_BUILDING_TYPE_APARTEMENT, CTRL_DESELECTED);
    SetControlState(CTRL_BUILDING_TYPE_CRATE_IN, CTRL_DESELECTED);
    SetControlState(CTRL_BUILDING_TYPE_CRATE_OUT, CTRL_DESELECTED);

    if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
        switch (building_list[EditBuilding].BuildingType) {
        case BUILDING_TYPE_HOUSE: SetControlState(CTRL_BUILDING_TYPE_HOUSE, CTRL_SELECTED); break;
        case BUILDING_TYPE_WAREHOUSE: SetControlState(CTRL_BUILDING_TYPE_WAREHOUSE, CTRL_SELECTED); break;
        case BUILDING_TYPE_OFFICE: SetControlState(CTRL_BUILDING_TYPE_OFFICE, CTRL_SELECTED); break;
        case BUILDING_TYPE_APARTEMENT: SetControlState(CTRL_BUILDING_TYPE_APARTEMENT, CTRL_SELECTED); break;
        case BUILDING_TYPE_CRATE_IN: SetControlState(CTRL_BUILDING_TYPE_CRATE_IN, CTRL_SELECTED); break;
        case BUILDING_TYPE_CRATE_OUT: SetControlState(CTRL_BUILDING_TYPE_CRATE_OUT, CTRL_SELECTED); break;

        default:
            ASSERT(0);
            break;
        }
    }

    EdRect content_rect;

    content_rect = ContentRect;
    content_rect.ShrinkRect(1, 1);
    content_rect.FillRect(CONTENT_COL);

    SetWorkWindowBounds(ContentLeft() + 1, ContentTop() + 1, ContentWidth() - 1, ContentHeight() - 1);
    DrawControlSet();
    ShowWorkWindow(0);
}

//---------------------------------------------------------------
extern void hilight_col_info();

void BuildTab::AddHeightOffset(std::int32_t *x, std::int32_t *y) {
    if (Texture & (6))
        return;
    //	*x-=((EditY-CurrentY)*ViewSize)/(BLOCK_SIZE<<4);
    //	*y-=-((EditY-CurrentY)*ViewSize)/(BLOCK_SIZE<<4);

    *x -= ((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
    *y -= -((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
}

std::int32_t BuildTab::GetHeightColour(std::int32_t storey) {
    if (Texture == 2)
        return (WHITE_COL);
    if (storey == EditStorey)
        return (GREEN_COL);

    if (CurrentY == EditY)
        return (0);
    else
        return (GREY_COL);
}

void BuildTab::HighlightVertexes(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    std::int32_t mx, mz, rect_size;
    EdRect rect;
    std::int32_t roof_flag = 0;
    std::int32_t building;

    mx = ViewX >> ELE_SHIFT;
    mz = ViewZ >> ELE_SHIFT;

    rect_size = ViewSize >> 2;

    //	if(0)
    if (Mode == BUILD_MODE_WAIT) {
        for (building = 1; building < MAX_BUILDINGS; building++) {
            std::int32_t x1, y1, z1, x2, y2, z2, index;
            MFPoint mouse_point;
            std::int32_t storey_index, wall;
            char str[100];
            std::int32_t ploty = 20, c0;

            storey_index = building_list[building].StoreyHead;

            while (storey_index) {
                /*
                                        sprintf(str," storey %d  roof %d next %d prev %d",storey_index,storey_list[storey_index].Roof,storey_list[storey_index].Next,storey_list[storey_index].Prev);
                                        QuickText(20,ploty,str,0);
                                        ploty+=20;
                */

                x1 = storey_list[storey_index].DX;
                y1 = storey_list[storey_index].DY;
                z1 = storey_list[storey_index].DZ;

                switch (storey_list[storey_index].StoreyType) {
                case STOREY_TYPE_FIRE_ESCAPE:

                {
                    char str[20];

                    //							x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                    //							z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                    z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                    if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                        sprintf(str, "%d", storey_list[storey_index].Height);
                        rect.SetRect(10, z1 + 2, 12, 12);
                        rect.OutlineRect(GetHeightColour(storey_index));
                        QuickTextC(12, z1 + 4, str, 0);
                    }
                } break;
                case STOREY_TYPE_LADDER: {
                    {
                        std::int32_t px, pz, qx, qz, wall, tempy, c0;
                        // draw ladder

                        px = x1;
                        pz = z1;

                        wall = storey_list[storey_index].WallHead;

                        qx = wall_list[wall].DX;
                        qz = wall_list[wall].DZ;

                        tempy = CurrentY;
                        CurrentY = storey_list[storey_index].DY;
                        for (c0 = 0; c0 < storey_list[storey_index].Height; c0++) {
                            DrawContentLine(px, pz, qx, qz, GetHeightColour(storey_index));
                            CurrentY += 64;
                        }
                        CurrentY = tempy;
                    }
                    char str[20];

                    //							x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                    //							z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                    z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                    if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                        sprintf(str, "%d", storey_list[storey_index].Height);
                        rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                        rect.OutlineRect(RED_COL);
                        QuickTextC(x1 - ViewSize * 2, z1 + 4, str, 0);
                    }
                }

                break;
                case STOREY_TYPE_STAIRCASE:
                    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                    z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                    if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                        sprintf(str, "%d", storey_list[storey_index].Info1);
                        rect.SetRect(10, z1 + 2, 12, 12);
                        rect.OutlineRect(GetHeightColour(storey_index));
                        QuickTextC(12, z1 + 4, str, 0);
                    }
                    break;
                case STOREY_TYPE_CABLE:

                    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                    z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                    if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                        sprintf(str, "%d", y1 >> 6);
                        rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                        rect.OutlineRect(0); // GetHeightColour(storey_index));
                        QuickTextC(x1 + 2 - ViewSize * 2, z1 + 4, str, 0);
                    }

                    CurrentY = 0;
                    for (c0 = 0; c0 < storey_list[storey_index].DY / 64; c0++) {
                        std::int32_t px, pz;

                        px = storey_list[storey_index].DX;
                        pz = storey_list[storey_index].DZ;
                        DrawContentLine(px - 40, pz - 40, px + 40, pz + 40, GetHeightColour(storey_index));
                        DrawContentLine(px + 40, pz - 40, px - 40, pz + 40, GetHeightColour(storey_index));
                        CurrentY += 64;
                    }
                    //							CurrentY=tempy;

                    wall = storey_list[storey_index].WallHead;
                    while (wall) {
                        x1 = wall_list[wall].DX;
                        y1 = wall_list[wall].DY;
                        z1 = wall_list[wall].DZ;
                        x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                        z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                        if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                            sprintf(str, "%d", y1 >> 6);
                            rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                            rect.OutlineRect(0); // GetHeightColour(storey_index));
                            QuickTextC(x1 + 2 - ViewSize * 2, z1 + 4, str, 0);

                            sprintf(str, "%d", wall_list[wall].TextureStyle2);
                            rect.SetRect(x1 - ViewSize * 2 + 14, z1 + 2, 12, 12);
                            rect.OutlineRect(GetHeightColour(storey_index));
                            QuickTextC(x1 + 2 - ViewSize * 2 + 14, z1 + 4, str, 0);
                        }

                        CurrentY = 0;
                        for (c0 = 0; c0 < wall_list[wall].DY / 64; c0++) {
                            std::int32_t px, pz;

                            px = wall_list[wall].DX;
                            pz = wall_list[wall].DZ;
                            DrawContentLine(px - 40, pz - 40, px + 40, pz + 40, GetHeightColour(storey_index));
                            DrawContentLine(px + 40, pz - 40, px - 40, pz + 40, GetHeightColour(storey_index));
                            CurrentY += 64;
                        }

                        wall = wall_list[wall].Next;
                    }
                    break;
                }

                x1 = storey_list[storey_index].DX;
                z1 = storey_list[storey_index].DZ;
                if (!Keys[KB_T]) {
                    if (Texture & 2) {
                    } else {
                        if (storey_list[storey_index].StoreyFlags & (FLAG_STOREY_TILED_ROOF | FLAG_STOREY_FLAT_TILED_ROOF)) {
                            std::int32_t tx, tz;

                            tx = ((((x1 + 100) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                            tz = ((((z1 + 100) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);
                            AddHeightOffset(&tx, &tz);
                            if (storey_list[storey_index].StoreyFlags & (FLAG_STOREY_FLAT_TILED_ROOF))
                                QuickTextC(tx, tz, "FRoof", 0);
                            else
                                QuickTextC(tx, tz, "TRoof", 0);
                        }
                        if (storey_list[storey_index].StoreyType == STOREY_TYPE_SKYLIGHT) {
                            std::int32_t tx, tz;

                            tx = ((((x1 + 100) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                            tz = ((((z1 + 100) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);
                            AddHeightOffset(&tx, &tz);
                            QuickTextC(tx, tz, "SKYLight", 0);
                        }
                    }
                }

                CurrentY = storey_list[storey_index].DY;
                index = storey_list[storey_index].WallHead;

                //				x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                //				z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);
                AddHeightOffset(&x1, &z1);
                //				if(0)
                if (index) {
                    rect.SetRect(x1 - rect_size, z1 - rect_size, rect_size << 1, rect_size << 1);
                    rect.OutlineRect(GetHeightColour(storey_index));

                    while (index) {
                        x1 = wall_list[index].DX;
                        z1 = wall_list[index].DZ;

                        //						x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                        //						z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                        x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                        z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                        AddHeightOffset(&x1, &z1);
                        rect.SetRect(x1 - rect_size, z1 - rect_size, rect_size << 1, rect_size << 1);
                        rect.OutlineRect(GetHeightColour(storey_index));

                        index = wall_list[index].Next;
                    }
                }
                //				if(roof_flag)
                //				{
                //					storey_index=storey_list[storey_list[storey_index].Prev].Next;
                //					roof_flag=0;
                //				}
                //				else
                {
                    storey_index = storey_list[storey_index].Next;
                }
                //			storey_index=storey_list[storey_index].Next;
            }
        }
    }
}

std::int32_t BuildTab::ClickInVertexStoreyList(std::int32_t building, std::int32_t storey_index, std::int32_t w, std::int32_t h, MFPoint *mouse_point, std::int32_t flags) {
    std::int32_t roof_flag = 0;
    EdRect rect;
    std::int32_t mx, mz, rect_size;
    std::int32_t x1, y1, z1, x2, y2, z2, index;

    std::int32_t ret_building = 0, ret_storey, ret_wall, wall;

    mx = ViewX >> ELE_SHIFT;
    mz = ViewZ >> ELE_SHIFT;

    rect_size = ViewSize >> 2;
    while (storey_index > 0) {
        x1 = storey_list[storey_index].DX;
        z1 = storey_list[storey_index].DZ;

        switch (storey_list[storey_index].StoreyType) {
        case STOREY_TYPE_FIRE_ESCAPE: {
            char str[20];

            x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
            z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
            if (x1 > 0 && x1 < w && z1 > 0 && z1 < h) {
                rect.SetRect(10, z1 + 2, 12, 12);
                if (rect.PointInRect(mouse_point)) {
                    if (storey_index == EditStorey) {
                        EditBuilding = building;
                        EditStorey = storey_index;
                        EditY = 0; // storey_list[storey_index].DY;
                        EditWall = -1;
                        return (2);
                    } else {
                        ret_building = building;
                        ret_storey = storey_index;
                        ret_wall = -1;
                    }
                }
            }
        } break;
        case STOREY_TYPE_LADDER: {
            char str[20];

            x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
            z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
            if (x1 > 0 && x1 < w && z1 > 0 && z1 < h) {
                rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                if (rect.PointInRect(mouse_point)) {
                    if (storey_index == EditStorey) {
                        EditBuilding = building;
                        EditStorey = storey_index;
                        EditY = 0; // storey_list[storey_index].DY;
                        EditWall = -1;
                        return (2);
                    } else {
                        ret_building = building;
                        ret_storey = storey_index;
                        ret_wall = -1;
                    }
                }
            }
        } break;

        case STOREY_TYPE_STAIRCASE:
            x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
            z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);

            if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                rect.SetRect(10, z1 + 2, 12, 12);
                if (rect.PointInRect(mouse_point)) {
                    if (storey_index == EditStorey) {
                        EditBuilding = building;
                        EditStorey = storey_index;
                        EditY = 0; // storey_list[storey_index].DY;
                        EditWall = -1;
                        return (2);
                    } else {
                        ret_building = building;
                        ret_storey = storey_index;
                        ret_wall = -1;
                    }
                }
            }
            break;
        case STOREY_TYPE_CABLE:
            x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
            z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);

            if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                if (rect.PointInRect(mouse_point)) {
                    if (storey_index == EditStorey) {
                        EditBuilding = building;
                        EditStorey = storey_index;
                        EditY = 0; // storey_list[storey_index].DY;
                        EditWall = 99999;
                        return (2);
                    } else {
                        ret_building = building;
                        ret_storey = storey_index;
                        ret_wall = 99999;
                    }
                }
            }

            wall = storey_list[storey_index].WallHead;
            while (wall) {
                x1 = wall_list[wall].DX;
                z1 = wall_list[wall].DZ;
                x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);
                if (x1 > 0 && x1 < WorkWindowRect.Width && z1 > 0 && z1 < WorkWindowRect.Height) {
                    rect.SetRect(x1 - ViewSize * 2, z1 + 2, 12, 12);
                    if (rect.PointInRect(mouse_point)) {
                        if (storey_index == EditStorey) {
                            EditBuilding = building;
                            EditStorey = storey_index;
                            EditY = 0; // y_list[storey_index].DY;
                            EditWall = -wall;
                            return (2);
                        } else {
                            ret_building = building;
                            ret_storey = storey_index;
                            ret_wall = -wall;
                        }
                    }
                    rect.SetRect(x1 - ViewSize * 2 + 14, z1 + 2, 12, 12);
                    if (rect.PointInRect(mouse_point)) {
                        if (flags == LEFT_CLICK) {
                            wall_list[wall].TextureStyle2++;
                        } else if (flags == RIGHT_CLICK) {
                            wall_list[wall].TextureStyle2--;
                        }
                        // RequestUpdate();
                        // DrawTabContent();
                        return (-1);
                    }
                }

                wall = wall_list[wall].Next;
            }

            break;
        }

        x1 = storey_list[storey_index].DX;
        z1 = storey_list[storey_index].DZ;

        CurrentY = storey_list[storey_index].DY;
        index = storey_list[storey_index].WallHead;

        //		x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
        //		z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);

        x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
        z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
        AddHeightOffset(&x1, &z1);
        if (index) {
            rect.SetRect(x1 - rect_size, z1 - rect_size, rect_size << 1, rect_size << 1);
            if (rect.PointInRect(mouse_point)) {
                if (storey_index == EditStorey) {
                    EditBuilding = building;
                    EditStorey = storey_index;
                    EditY = 0; // storey_list[EditStorey].DY;
                    EditWall = 0;
                    return (2);
                } else {
                    ret_building = building;
                    ret_storey = storey_index;
                    ret_wall = 0;
                }
            }

            while (index) {
                x1 = wall_list[index].DX;
                z1 = wall_list[index].DZ;

                //				x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                //				z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
                z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
                AddHeightOffset(&x1, &z1);

                rect.SetRect(x1 - rect_size, z1 - rect_size, rect_size << 1, rect_size << 1);
                if (rect.PointInRect(mouse_point)) {
                    if (storey_index == EditStorey) {
                        EditBuilding = building;
                        EditStorey = storey_index;
                        EditY = 0; // storey_list[EditStorey].DY;
                        EditWall = index;
                        return (2);
                    } else {
                        ret_building = building;
                        ret_storey = storey_index;
                        ret_wall = index;
                    }
                }

                index = wall_list[index].Next;
            }
        }
        if (roof_flag) {
            storey_index = storey_list[storey_list[storey_index].Prev].Next;
            roof_flag = 0;
        } else {
            /*
                                    std::int32_t	temp_index;

                                    temp_index=storey_list[storey_index].Roof;
                                    if(temp_index)
                                    {
                                            storey_index=temp_index;
                                            roof_flag=1;

                                    }
                                    else
            */
            {
                storey_index = storey_list[storey_index].Next;
                roof_flag = 0;
            }
        }
    }

    if (ret_building) {
        EditBuilding = ret_building;
        EditStorey = ret_storey;
        EditY = 0; // storey_list[EditStorey].DY;
        EditWall = ret_wall;

        DrawTabContent();

        return (1);
    }
    return (0);
}

std::int32_t BuildTab::ClickInVertex(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, MFPoint *mouse_point, std::int32_t flags) {
    std::int32_t storey_index, found;
    std::int32_t building;
    std::int32_t found_one = 0;

    // if(EditBuilding)
    {
        for (building = 1; building < MAX_BUILDINGS; building++) {
            if (building_list[building].BuildingFlags) {
                storey_index = building_list[building].StoreyHead;
                if (storey_index) {
                    found = ClickInVertexStoreyList(building, storey_index, w, h, mouse_point, flags);
                    if (found < 0)
                        return (found);
                    if (found == 2)
                        return (1);
                    if (found == 1)
                        found_one = 1;
                    //		if(FloorHead)
                    //			ClickInVertexStoreyList(FloorHead,w,h,mouse_point);
                }
            }
        }
    }
    return (found_one);
}

std::int32_t BuildTab::DrawWindow(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2, std::int32_t dx, std::int32_t dz) {
    std::int32_t pdx, pdz;

    pdx = dz;
    pdz = -dx;

    pdx = (pdx * 20) >> 10;
    pdz = (pdz * 20) >> 10;

    DrawContentLine(x1 + (pdx >> 1), z1 + (pdz >> 1), x1 - (pdx >> 1), z1 - (pdz >> 1), GetHeightColour(0));
    DrawContentLine(x2 + (pdx >> 1), z2 + (pdz >> 1), x2 - (pdx >> 1), z2 - (pdz >> 1), GetHeightColour(0));

    DrawContentLine(x1 + (pdx >> 1), z1 + (pdz >> 1), x2 + (pdx >> 1), z2 + (pdz >> 1), GetHeightColour(0));
    DrawContentLine(x1 - (pdx >> 1), z1 - (pdz >> 1), x2 - (pdx >> 1), z2 - (pdz >> 1), GetHeightColour(0));
    return (0);
}

std::int32_t BuildTab::DrawWall(std::int32_t px, std::int32_t pz, std::int32_t x1, std::int32_t z1, std::int32_t index, std::int32_t storey) {
    std::int32_t wcount, wwidth, wwidth_perc, wallwidth, wallwidth_perc, dx, dz, dist;
    std::int32_t prev_x, prev_z;

    dx = abs(px - x1);
    dz = abs(pz - z1);

    dist = sqrl(SDIST2(dx, dz));
    if (dist == 0)
        return (0);

    if (wall_list[index].WallFlags & FLAG_WALL_AUTO_WINDOWS) {
        wcount = dist / (BLOCK_SIZE * 3);
        wwidth = dist / (wcount * 2 + 1);

    } else {
        wcount = 0; // wall_list[index].WindowCount;
        wwidth = BLOCK_SIZE;
    }

    dx = (px - x1);
    dz = (pz - z1);

    if (wcount < 0)
        return (0);

    wallwidth = (dist - (wcount * wwidth)) / (wcount + 1);

    dx = (dx << 10) / dist;
    dz = (dz << 10) / dist;

    prev_x = x1;
    prev_z = z1;

    while (wcount) {
        x1 = prev_x + ((dx * wallwidth) >> 10);
        z1 = prev_z + ((dz * wallwidth) >> 10);

        //		DrawLineC(prev_x,prev_z,x1,z1,1); //wall
        DrawContentLine(prev_x, prev_z, x1, z1, GetHeightColour(storey));

        prev_x = x1;
        prev_z = z1;

        x1 = prev_x + ((dx * wwidth) >> 10);
        z1 = prev_z + ((dz * wwidth) >> 10);

        DrawWindow(prev_x, prev_z, x1, z1, dx, dz);
        //		DrawContentLine(prev_x,prev_z,x1,z1,0);

        prev_x = x1;
        prev_z = z1;
        wcount--;
    }
    x1 = prev_x + ((dx * wallwidth) >> 10);
    z1 = prev_z + ((dz * wallwidth) >> 10);

    //		DrawLineC(prev_x,prev_z,x1,z1,1); //wall

    if (storey_list[storey].StoreyType == STOREY_TYPE_FENCE) {
        DrawContentLine(prev_x, prev_z, x1, z1, YELLOW_COL);

    } else {
        DrawContentLine(prev_x, prev_z, x1, z1, GetHeightColour(storey));
    }
    return (0);
}

void BuildTab::DrawContentLine(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t col) {
    /*
            x1=((x1>>ELE_SHIFT)-(ViewX>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Width>>1);
            y1=((y1>>ELE_SHIFT)-(ViewZ>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Height>>1);

            x2=((x2>>ELE_SHIFT)-(ViewX>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Width>>1);
            y2=((y2>>ELE_SHIFT)-(ViewZ>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Height>>1);
    */

    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
    y1 = ((((y1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

    x2 = ((((x2) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
    y2 = ((((y2) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

    AddHeightOffset(&x1, &y1);
    AddHeightOffset(&x2, &y2);

    DrawLineC(x1, y1, x2, y2, col);
}

void BuildTab::DrawContentLineY(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t col) {
    std::int32_t temp;
    /*
            x1=((x1>>ELE_SHIFT)-(ViewX>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Width>>1);
            y1=((y1>>ELE_SHIFT)-(ViewZ>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Height>>1);

            x2=((x2>>ELE_SHIFT)-(ViewX>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Width>>1);
            y2=((y2>>ELE_SHIFT)-(ViewZ>>ELE_SHIFT))*ViewSize+(WorkWindowRect.Height>>1);
    */

    temp = CurrentY;
    x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
    z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

    x2 = ((((x2) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
    z2 = ((((z2) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

    CurrentY = y1;
    AddHeightOffset(&x1, &z1);
    CurrentY = y2;
    AddHeightOffset(&x2, &z2);
    CurrentY = temp;

    DrawLineC(x1, z1, x2, z2, col);
}
void BuildTab::DrawContentRect(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2, std::int32_t col) {
    DrawContentLine(x1, z1, x2, z1, col);
    DrawContentLine(x2, z1, x2, z2, col);
    DrawContentLine(x2, z2, x1, z2, col);
    DrawContentLine(x1, z2, x1, z1, col);
}

std::int32_t find_nearest_point(std::int32_t x, std::int32_t z, std::int32_t index, std::int32_t *rx, std::int32_t *rz) {
    std::int32_t best, best_dist = 0x7fffffff, dx, dz, dist;

    dx = (storey_list[index].DX - x);
    dz = (storey_list[index].DZ - z);

    dist = SDIST2(dx, dz);
    best_dist = dist;
    best = -index;

    index = storey_list[index].WallHead;
    while (index) {
        dx = (wall_list[index].DX - x);
        dz = (wall_list[index].DZ - z);
        dist = SDIST2(dx, dz);
        if (dist < best_dist) {
            best_dist = dist;
            best = index;
        }
        index = wall_list[index].Next;
    }
    if (best < 0) {
        *rx = storey_list[-best].DX;
        *rz = storey_list[-best].DZ;
    } else {
        *rx = (wall_list[best].DX);
        *rz = (wall_list[best].DZ);
    }
    return (1);
}

void BuildTab::DrawRoofFaces(std::int32_t roof, std::int32_t storey) {
    std::int32_t wall, index;
    std::int32_t rx, rz;

    wall = storey_list[storey].WallHead;
    while (wall) {
        std::int32_t x1, z1;

        x1 = wall_list[wall].DX;
        z1 = wall_list[wall].DZ;

        if (find_nearest_point(x1, z1, roof, &rx, &rz)) {
            CurrentY = storey_list[storey].DY;
            AddHeightOffset(&x1, &z1);
            CurrentY = storey_list[roof].DY;
            AddHeightOffset(&rx, &rz);

            DrawContentLine(x1, z1, rx, rz, RED_COL);
        }
        wall = wall_list[wall].Next;
    }
}

void BuildTab::DrawFloorFaces(std::int32_t floor_head) {
    std::int32_t wall, index;
    std::int32_t rx, rz;
    std::int32_t building;

    CurrentY = 0;

    for (building = 1; building < MAX_BUILDINGS; building++) {
        if (building_list[building].BuildingFlags & 1) {
            wall = storey_list[building_list[building].StoreyHead].WallHead;
            while (wall) {
                std::int32_t x1, z1;

                x1 = wall_list[wall].DX;
                z1 = wall_list[wall].DZ;

                if (find_nearest_point(x1, z1, floor_head, &rx, &rz)) {
                    AddHeightOffset(&x1, &z1);
                    AddHeightOffset(&rx, &rz);

                    DrawContentLine(x1, z1, rx, rz, GREEN_COL);
                }
                wall = wall_list[wall].Next;
            }
        }
    }
}

void BuildTab::DrawFloorTextures(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    std::int32_t mx, my, mz;
    std::int32_t dx, dy, dz, width, height, count_across, count_high;
    std::uint16_t texture;

    mx = ViewX >> ELE_SHIFT;
    my = ViewY;
    mz = ViewZ >> ELE_SHIFT;

    width = ViewSize;

    count_across = ((w / (width)) >> 1) + 1;
    count_high = (h / (width)) >> 1;

    for (dx = -count_across; dx <= count_across; dx++)
        for (dz = -count_high; dz <= count_high; dz++) {
            std::int32_t x1, y1, x2, y2;

            x1 = (w >> 1) + (dx) * (width);
            x2 = (w >> 1) + (dx) * (width) + width;

            y1 = (h >> 1) + dz * (width);
            y2 = (h >> 1) + dz * (width) + width;
            if (edit_info.RoofTex) {
                texture = tex_map[mx + dx][mz + dz];
            } else {
                texture = edit_map[mx + dx][mz + dz].Texture;
            }
            if (mx + dx >= 0 && mx + dx < EDIT_MAP_WIDTH && mz + dz >= 0 && mz + dz < EDIT_MAP_WIDTH)
                if (add_floor_face_to_bucket(x1, y1, 0, x2, y1, 0, x1, y2, 0, x2, y2, 0, &edit_map[mx + dx][mz + dz], 128, 128, 128, 128, texture)) {
                    selected_prim_xyz.X = dx + mx;
                    selected_prim_xyz.Y = 0;
                    selected_prim_xyz.Z = dz + mz;

                    hilited_face.MapX = dx + mx;
                    hilited_face.MapY = 0; // dx+mx-1;
                    hilited_face.MapZ = dz + mz;
                }
        }
    render_view(0);
    for (dx = -count_across; dx <= count_across; dx++)
        for (dz = -count_high; dz <= count_high; dz++) {
            std::int32_t ox, oz, y;

            y = edit_map[mx + dx][mz + dz].Y;
            for (ox = -1; ox <= 1; ox++)
                for (oz = -1; oz <= 1; oz++) {
                    if (mx + dx + ox >= 0 && mx + dx + ox < EDIT_MAP_WIDTH && mz + dz + oz >= 0 && mz + dz + oz < EDIT_MAP_WIDTH)
                        if ((ox || oz) && y)
                            if (y == edit_map[mx + dx + ox][mz + dz + oz].Y) {
                                DrawContentLine((mx + dx) << ELE_SHIFT, (mz + dz) << ELE_SHIFT, (mx + dx + ox) << ELE_SHIFT, (mz + dz + oz) << ELE_SHIFT, 0);
                                //				goto done;
                            }
                }
        done:;
        }
}

void BuildTab::DrawFloorLabels(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    std::int32_t mx, my, mz;
    std::int32_t dx, dy, dz, width, height, count_across, count_high;
    mx = ViewX >> ELE_SHIFT;
    my = ViewY;
    mz = ViewZ >> ELE_SHIFT;

    width = ViewSize;

    count_across = ((w / (width)) >> 1) + 1;
    count_high = ((h / (width)) >> 1) - 1;

    for (dx = -count_across; dx <= count_across; dx++)
        for (dz = -count_high; dz <= count_high; dz++) {
            std::int32_t x1, y1, x2, y2, alt, salt;
            char str[100];

            x1 = (w >> 1) + (dx) * (width);
            y1 = (h >> 1) + dz * (width);
            if (RoofTop) {
                salt = edit_map_roof_height[mx + dx][mz + dz];
            } else {
                salt = edit_map[mx + dx][mz + dz].Y;
            }
            alt = abs(salt);

            sprintf(str, "%d", alt);
            QuickTextC(x1 + 1, y1 + 1, str, WHITE_COL);
            QuickTextC(x1 - 1, y1 - 1, str, WHITE_COL);
            if (salt < 0)
                QuickTextC(x1, y1, str, RED_COL);
            else
                QuickTextC(x1, y1, str, 0);

            //		x2=(w>>1)+(dx)*(width)+width;
            //		y2=(h>>1)+dz*(width)+width;
            /*
                            if(add_floor_face_to_bucket(x1,y1,0,x2,y1,0,x1,y2,0,x2,y2,0,&edit_map[mx+dx][mz+dz],128,128,128,128))
                            {
                                    selected_prim_xyz.X	=	dx+mx;
                                    selected_prim_xyz.Y	=	0;
                                    selected_prim_xyz.Z	=	dz+mz;

                                    hilited_face.MapX=dx+mx;
                                    hilited_face.MapY=0; //dx+mx-1;
                                    hilited_face.MapZ=dz+mz;
                            }
            */
        }
    render_view(0);
}

void draw_status_line(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, char *str) {
    EdRect rect;
    rect.SetRect(x, y, w, h);
    rect.FillRect(CONTENT_COL);
    rect.HiliteRect(HILITE_COL, LOLITE_COL);
    QuickTextC(x + 4, y + 2, str, 0);
}

//
// Returns true if the given storey can have an inside generated for it.
//

std::int32_t is_storey_habitable(std::int32_t storey) {
    ASSERT(WITHIN(storey, 1, MAX_STOREYS - 1));

    //
    // It must be normal and circular.
    //

    if ((storey_list[storey].StoreyType == STOREY_TYPE_NORMAL) && is_storey_circular(storey)) {
        return true;
    } else {
        return false;
    }
}

std::int32_t identical_storey(std::int32_t px, std::int32_t pz, std::int32_t x1, std::int32_t z1, std::int32_t storey) {
    storey = storey_list[storey].Next;
    std::int32_t sx, sz, nx, nz, index;

    while (storey) {
        sx = storey_list[storey].DX;
        sz = storey_list[storey].DZ;
        index = storey_list[storey].WallHead;

        while (index) {
            nx = wall_list[index].DX;
            nz = wall_list[index].DZ;

            if (px == sx && pz == sz && x1 == nx && z1 == nz)
                return (1);

            sx = nx;
            sz = nz;
            index = wall_list[index].Next;
        }

        storey = storey_list[storey].Next;
    }
    return (0);
}

void BuildTab::DrawModuleContent(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    std::int32_t wwx, wwy, www, wwh;
    EdRect drawrect;

    std::int32_t dx, dy, dz, width, height, count_across, count_high;
    std::int32_t c0, c1;
    std::int32_t mx, my, mz;
    std::int32_t index;
    struct EditMapElement *p_ele;
    std::int32_t roof_flag = 0;
    std::int32_t building;
    char str[100];
    std::int32_t storey_height;

    //	my=((CVSlider*)GetControlPtr(CTRL_BUILD_V_SLIDE_LEVEL))->GetCurrentValue();
    //	my=((CVSlider*)GetControlPtr(CTRL_BUILD_V_SLIDE_LEVEL))->GetCurrentValue();

    mx = ViewX >> ELE_SHIFT;
    my = ViewY;
    mz = ViewZ >> ELE_SHIFT;

    RedrawModuleContent = 0;
    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;
    SetWorkWindowBounds(x, y, w - 1, h - 1);
    drawrect.SetRect(0, 0, w - 1, h - 1);
    drawrect.FillRect(CONTENT_COL_BR);

    width = ViewSize;

    count_across = ((w / (width)) >> 1) + 1;
    count_high = (h / (width)) >> 1;

    //
    // Recalculate the insides all the time!
    //

    inside_valid = false;
    inside_failure = false;

    //
    // Shall we recalculate the insides?
    //

    if (EditBuilding &&
        EditStorey) {
        if (inside_building != EditBuilding ||
            inside_storey != EditStorey) {
            //
            // The inside info is for the wrong place.
            //

            inside_building = EditBuilding;
            inside_storey = EditStorey;
            inside_valid = false;
        }

        if (inside_valid) {
            //
            // Everything okay.
            //
        } else {
            inside_valid = false;

            /*

            //
            // Is this a valid storey and building?
            //

            if (is_storey_habitable(inside_storey))
            {
                    //
                    // The inside stuff in the game work off the supermap structure,
                    // so convert this building into the supermap structure.
                    //

                    create_super_dbuilding(inside_building);

                    //
                    // Calculate the height of this storey.
                    //

                    storey_height = storey_list[inside_storey].DY;

                    //
                    // Calculate the insides for the storey.  We use dbuilding 1 because
                    // create_super_dbuilding() always puts the building into dbuilding 1.
                    //

                    inside_valid = ENTER_setup(1, storey_height, false, true);

                    if (!inside_valid)
                    {
                            //
                            // We can't go inside this building.
                            //

                            inside_failure = true;
                    }
                    else
                    {
                            //
                            // Remember the seed used.
                            //

                            building_list[EditBuilding].InsideSeed = dbuildings[1].SeedInside;
                    }
            }

            */
        }
    } else {
        inside_valid = false;
    }

    /*
            for(dx=-count_across;dx<count_across;dx++)
            for(dz=-count_high;dz<count_high;dz++)
            {
                    index=edit_map[(dx+mx)][(my)].Depth[(dz+mz)];
                    if(index)
                    {
                                    p_ele=&edit_map_eles[index];
                                    index=p_ele->CubeType.Prim;
                                    DrawBoxC(0+(w>>1)+dx*(width+1),0+(h>>1)+dz*(width+1),width,width,0);
                    }
                    else
                            DrawBoxC(0+(w>>1)+dx*(width+1),0+(h>>1)+dz*(width+1),width,width,1);

                    if(my<127)
                    {
                            index=edit_map[(dx+mx)][(my+1)].Depth[(dz+mz)];
                            if(index)
                            {
                                            p_ele=&edit_map_eles[index];
                                            index=p_ele->CubeType.Prim;
                                            DrawVLineC((w>>1)+dx*(width+1)-1,(h>>1)+dz*(width+1)-1,(h>>1)+(dz+1)*(width+1),2);
                                            DrawVLineC((w>>1)+(dx+1)*(width+1),(h>>1)+dz*(width+1)-1,(h>>1)+(dz+1)*(width+1),2);
                                            DrawHLineC((w>>1)+dx*(width+1)-1,(w>>1)+(dx+1)*(width+1),(h>>1)+dz*(width+1)-1,2);
                                            DrawHLineC((w>>1)+dx*(width+1)-1,(w>>1)+(dx+1)*(width+1),(h>>1)+(dz+1)*(width+1),2);
                            }
                    }

                    if(my>0)
                    {
                            index=edit_map[(dx+mx)][(my-1)].Depth[(dz+mz)];
                            if(index)
                            {
                                            p_ele=&edit_map_eles[index];
                                            index=p_ele->CubeType.Prim;
                                            DrawPixelC(0+(w>>1)+dx*(width+1)+(width>>1),0+(h>>1)+dz*(width+1)+(width>>1),2);
                            }
                    }
            }
    */

    if (!Keys[KB_T]) {
        if (Texture & 2)
            DrawFloorTextures(x, y, w, h);
        else {
            for (dx = -count_across; dx <= count_across; dx++) {
                DrawVLineC((w >> 1) + (dx) * (width), 0, h, RGB_TO_565(100, 100, 100));
            }
            for (dz = -count_high; dz <= count_high; dz++) {
                DrawHLineC(0, w, (h >> 1) + dz * (width), RGB_TO_565(100, 100, 100));
            }
        }
        if (Texture & 4)
            DrawFloorLabels(x, y, w, h);

        {
            sprintf(str, " engine (%d,%d,%d)  view %d %d %d ", engine.X >> 8, engine.Y >> 8, engine.Z >> 8, ViewX, ViewY, ViewZ);
            QuickTextC(3, 3, str, 0);
            QuickTextC(4, 4, str, 255);
        }
    }

    //
    // Draw all the extra things.
    //

    if (0) {
        std::int32_t i;
        std::int32_t j;

        std::int32_t x;
        std::int32_t z;

        std::int32_t angle;
        std::int32_t angle1;
        std::int32_t angle2;
        std::int32_t x1;
        std::int32_t x2;
        std::int32_t z1;
        std::int32_t z2;
        std::int32_t y1, y2;

        EXTRA_Thing *et;

        std::int32_t old_current_y;

        old_current_y = CurrentY;
        CurrentY = 0;

        for (i = 0; i < EXTRA_MAX_THINGS; i++) {
            et = &EXTRA_thing[i];

            switch (et->type) {
            case EXTRA_TYPE_NONE:
                break;

            case EXTRA_TYPE_PUDDLE:

#define NUM_EXTRA_PUDDLE_STEPS 16

                for (angle = 0; angle < 2048; angle += 2048 / NUM_EXTRA_PUDDLE_STEPS) {
                    angle1 = angle;
                    angle2 = angle + 1024;

                    angle1 &= 2047;
                    angle2 &= 2047;

                    x1 = et->x + (SIN(angle1) * et->radius >> 16);
                    z1 = et->z + (COS(angle1) * et->radius >> 16);

                    x2 = et->x + (SIN(angle2) * et->radius >> 16);
                    z2 = et->z + (COS(angle2) * et->radius >> 16);

                    DrawContentLine(
                        x1, z1,
                        x2, z2,
                        BLUE_COL);
                }

                break;

            case EXTRA_TYPE_MIST:

                x1 = et->x - et->radius;
                z1 = et->z - et->radius;

                x2 = et->x + et->radius;
                z2 = et->z + et->radius;

#define NUM_EXTRA_MIST_STEPS 25

                for (j = 0; j < NUM_EXTRA_MIST_STEPS; j++) {
                    x = x1 + ((x2 - x1) * j) / NUM_EXTRA_MIST_STEPS;
                    z = z1 + ((z2 - z1) * j) / NUM_EXTRA_MIST_STEPS;

                    DrawContentLine(
                        x, z1,
                        x, z2,
                        GREY_COL);

                    DrawContentLine(
                        x1, z,
                        x2, z,
                        GREY_COL);
                }
                break;

            default:
                ASSERT(0);
                break;
            }

            if (et->type != EXTRA_TYPE_NONE) {
                //
                // Always draw a little nodule that you can select.
                //

#define NUM_EXTRA_NODE_STEPS 8

                for (angle = 0; angle < 2048; angle += 2048 / NUM_EXTRA_NODE_STEPS) {
                    angle1 = angle;
                    angle2 = angle + 2048 / NUM_EXTRA_NODE_STEPS;

                    angle1 &= 2047;
                    angle2 &= 2047;

                    x1 = et->x + (SIN(angle1) * EXTRA_SELECT_DIST >> 17);
                    z1 = et->z + (COS(angle1) * EXTRA_SELECT_DIST >> 17);

                    x2 = et->x + (SIN(angle2) * EXTRA_SELECT_DIST >> 17);
                    z2 = et->z + (COS(angle2) * EXTRA_SELECT_DIST >> 17);

                    DrawContentLine(
                        x1, z1,
                        x2, z2,
                        YELLOW_COL);
                }
            }
        }

        CurrentY = old_current_y;
    }

    //	if(EditBuilding)
    //	if(0)
    for (building = 1; building < MAX_BUILDINGS; building++)
        if (building_list[building].BuildingFlags & 1) {
            std::int32_t i, tx, tz, x1, y1, z1, x2, y2, z2, index, px, py, pz, fx, fy, fz, dx, dz, doorx1, doorx2, doorz1, doorz2;
            MFPoint mouse_point;
            std::int32_t storey_index;
            char ploty = 30;

            storey_index = building_list[building].StoreyHead;

            while (storey_index) {
                std::uint8_t drawn_normal = 0;

                //			if(storey_index==337 || storey_index==24)
                {
                    if (Keys[KB_D]) {
                        sprintf(str, " storey %d  next %d prev %d dy %d height %d wallhead %d ->[%d] ->[%d]", storey_index, storey_list[storey_index].Next, storey_list[storey_index].Prev, storey_list[storey_index].DY, storey_list[storey_index].Height, storey_list[storey_index].WallHead, wall_list[storey_list[storey_index].WallHead].Next, wall_list[wall_list[storey_list[storey_index].WallHead].Next].Next);
                        QuickText(20, ploty, str, 0);
                        ploty += 20;
                    }

                    mouse_point.X = MouseX;
                    mouse_point.Y = MouseY;
                    Parent->GlobalToLocal(&mouse_point);

                    x1 = storey_list[storey_index].DX;
                    y1 = storey_list[storey_index].DY;
                    z1 = storey_list[storey_index].DZ;

                    CurrentY = storey_list[storey_index].DY;
                    index = storey_list[storey_index].WallHead;
                    switch (storey_list[storey_index].StoreyType) {
                    case STOREY_TYPE_NORMAL:
                        drawn_normal = 1;
                        break;
                    }

                    if (Mode == BUILD_MODE_CONT_STOREY) {
                        CurrentY = storey_list[EditStorey].DY;
                        mouse_point.X += ((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
                        mouse_point.Y += -((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);

                        CalcMapCoord(&fx, &fy, &fz, x, y, w, h, &mouse_point);
                        CurrentY = storey_list[storey_index].DY;
                    }

                    if (roof_flag && storey_list[storey_index].StoreyType == STOREY_TYPE_ROOF_QUAD && index) {
                        std::int32_t x[4], z[4];
                        std::int32_t wall, c0;

                        x[0] = x1;
                        z[0] = z1;
                        wall = index;
                        for (c0 = 1; c0 < 4 && wall; c0++) {
                            x[c0] = wall_list[wall].DX;
                            z[c0] = wall_list[wall].DZ;

                            wall = wall_list[wall].Next;
                        }
                        if (c0 > 3) {
                            DrawContentLine(x[0], z[0], x[2], z[2], GetHeightColour(storey_index));
                            DrawContentLine(x[1], z[1], x[3], z[3], GetHeightColour(storey_index));
                        }
                    }

                    if (index == 0) {
                        if (Mode == BUILD_MODE_CONT_STOREY && storey_index == EditStorey)
                            DrawContentLine(x1, z1, fx, fz, GetHeightColour(storey_index));
                    } else {
                        /*
                                                        if(Mode==BUILD_MODE_WAIT)
                                                        {
                                                                if(storey_list[storey_index].StoreyType==STOREY_TYPE_ROOF_QUAD)
                                                                {
                                                                        CurrentY+=BLOCK_SIZE;
                                                                }

                                                        }
                        */

                        px = x1;
                        py = y1;
                        pz = z1;
                        while (index) {
                            x1 = wall_list[index].DX;
                            y1 = wall_list[index].DY;
                            z1 = wall_list[index].DZ;

                            //					if(!identical_storey(px,pz,x1,z1,storey_index))
                            {
                                if (roof_flag)
                                    DrawContentLine(px, pz, x1, z1, GetHeightColour(storey_index));
                                else {
                                    if (storey_list[storey_index].StoreyType == STOREY_TYPE_CABLE) {
                                        DrawContentLineY(px, py, pz, x1, y1, z1, RED_COL); // otline tiled roofs
                                    } else {
                                        DrawWall(px, pz, x1, z1, index, storey_index);

                                        {
                                            std::int32_t x1, y1, x2, y2;
                                            x1 = px;
                                            y1 = pz;
                                            x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                                            y1 = ((((y1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);

                                            x2 = x1;
                                            y2 = y1;

                                            AddHeightOffset(&x1, &y1);
                                            CurrentY += storey_list[storey_index].Height;
                                            AddHeightOffset(&x2, &y2);
                                            CurrentY -= storey_list[storey_index].Height;

                                            DrawLineC(x1, y1, x2, y2, 0);
                                        }
                                    }
                                }

                                if (storey_list[storey_index].StoreyFlags & (FLAG_STOREY_TILED_ROOF | FLAG_STOREY_FLAT_TILED_ROOF)) {
                                    CurrentY += storey_list[storey_index].Height;
                                    DrawContentLine(px, pz, x1, z1, 0); // otline tiled roofs
                                    CurrentY -= storey_list[storey_index].Height;
                                }
                            }
                            px = x1;
                            py = y1;
                            pz = z1;
                            index = wall_list[index].Next;
                        }
                        if (Mode == BUILD_MODE_CONT_STOREY && storey_index == EditStorey)
                            DrawContentLine(px, pz, fx, fz, GetHeightColour(storey_index));
                    }

                    //
                    // If this is the current storey, then draw the insides of this building.
                    //

                    if (storey_index == inside_storey &&
                        building == inside_building) {
                        //
                        // Have we got generated the insides for this storey?
                        //
#ifdef DOG_POO
                        if (inside_valid && 0) {
                            ID_Roominfo ri;
                            ID_Wallinfo wi;
                            ID_Stairinfo si;

                            //
                            // Draw the room layout.
                            //

                            ID_editor_start_get_rooms();
                            ID_editor_start_get_walls();
                            ID_editor_start_get_stairs();

                            while (ID_editor_get_wall(&wi)) {
                                //
                                // Draw the walls.
                                //

                                x1 = wi.x1 << ELE_SHIFT;
                                z1 = wi.z1 << ELE_SHIFT;
                                x2 = wi.x2 << ELE_SHIFT;
                                z2 = wi.z2 << ELE_SHIFT;

                                DrawContentLine(
                                    x1, z1,
                                    x2, z2,
                                    RED_COL);

                                //
                                // Draw the doors.
                                //

                                for (i = 0; i < 4; i++) {
                                    dx = x2 - x1;
                                    dz = z2 - z1;

                                    dx = SIGN(dx) << ELE_SHIFT;
                                    dz = SIGN(dz) << ELE_SHIFT;

                                    if (wi.door[i] != 255) {
                                        doorx1 = x1 + dx * wi.door[i];
                                        doorz1 = z1 + dz * wi.door[i];

                                        doorx2 = doorx1 + dx;
                                        doorz2 = doorz1 + dz;

                                        DrawContentLine(
                                            doorx1, doorz1,
                                            doorx2, doorz2,
                                            0);
                                    }
                                }
                            }

                            //
                            // Draw the room names.
                            //

                            while (ID_editor_get_room(&ri)) {
                                tx = (((((ri.x << ELE_SHIFT) + 100) - (ViewX)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Width >> 1);
                                tz = (((((ri.z << ELE_SHIFT) + 100) - (ViewZ)) * ViewSize) / ELE_SIZE) + (WorkWindowRect.Height >> 1);
                                AddHeightOffset(&tx, &tz);

                                QuickTextC(tx, tz, ri.what, WHITE_COL);
                            }

                            while (ID_editor_get_stair(&si)) {
                                dx = si.x2 - si.x1;
                                dz = si.z2 - si.z1;

#define NUM_DRAW_STEPS 8

                                for (i = 0; i < NUM_DRAW_STEPS; i++) {
                                    x1 = (si.x1 << ELE_SHIFT) + (1 << (ELE_SHIFT - 1));
                                    z1 = (si.z1 << ELE_SHIFT) + (1 << (ELE_SHIFT - 1));

                                    x1 += i * dx * (256 / NUM_DRAW_STEPS);
                                    z1 += i * dz * (256 / NUM_DRAW_STEPS);

                                    x2 = x1 + (dz * 128);
                                    z2 = z1 - (dx * 128);

                                    DrawContentLine(
                                        x1, z1,
                                        x2, z2,
                                        0);
                                }
                            }
                        }
#endif
                    }
                }

                if (roof_flag) {
                    storey_index = storey_list[storey_list[storey_index].Prev].Next;
                    roof_flag = 0;
                } else {
                    /*
                    std::int32_t	temp_index;
                    temp_index=storey_list[storey_index].Roof;
                    if(temp_index)
                    {
                            storey_index=temp_index;
                            roof_flag=1;

                    }
                    else
                    */
                    {
                        storey_index = storey_list[storey_index].Next;
                        roof_flag = 0;
                    }
                }
                if (Texture == 2 && drawn_normal)
                    storey_index = 0;
            }
        }

    //
    // Draw the edge of the map...
    //

    CurrentY = 0;

#define EDGE_COLOUR RED_COL

    for (std::int32_t i = 0; i < 128; i++) {
        DrawContentLine(i << 8, 0 << 8, i - 1 << 8, -1 << 8, EDGE_COLOUR);
        DrawContentLine(i << 8, 128 << 8, i + 1 << 8, 129 << 8, EDGE_COLOUR);

        DrawContentLine(0 << 8, i << 8, -1 << 8, i - 1 << 8, EDGE_COLOUR);
        DrawContentLine(128 << 8, i << 8, 129 << 8, i + 1 << 8, EDGE_COLOUR);
    }

    /*
                            if(FloorHead)
                            {
                                    index=FloorHead;
                                    px=storey_list[index].DX;
                                    pz=storey_list[index].DZ;
                                    CurrentY=0;
                                    index=storey_list[index].WallHead;

                                    while(index)
                                    {
                                            x1=wall_list[index].DX;
                                            z1=wall_list[index].DZ;

                                            DrawContentLine(px,pz,x1,z1,GetHeightColour());
                                            px=x1;
                                            pz=z1;
                                            index=wall_list[index].Next;
                                    }
                                    if(Mode==BUILD_MODE_CONT_STOREY)
                                            DrawContentLine(px,pz,fx,fz,GetHeightColour());

                            }
    */

    if (!Keys[KB_T]) {
        HighlightVertexes(x, y, w, h);
    }

    //	if(0)
    if (EditStorey && storey_list[EditStorey].StoreyType == STOREY_TYPE_ROOF) {
        DrawRoofFaces(EditStorey, storey_list[EditStorey].Prev);
    }

    // 	if(FloorHead)
    //		DrawFloorFaces(FloorHead);

    /*
            {

                    char	str[100];
                    sprintf(str," build %d storey %d wall %d edity %d",EditBuilding,EditStorey,EditWall,EditY);
                    QuickTextC(20,20,str,0);
            }
            switch(Mode)
            {
                            break;
            }
    */

    sprintf(str, " Building: %d Storey %d (%s) wall %d xyz (%d,%d,%d) height %d sty1 %d sty2 %d", EditBuilding, EditStorey, storey_name[storey_list[EditStorey].StoreyType], EditWall, wall_list[EditWall].DX >> 8, storey_list[EditStorey].DY, wall_list[EditWall].DZ >> 8, storey_list[EditStorey].Height, wall_list[EditWall].TextureStyle, wall_list[EditWall].TextureStyle2);

    draw_status_line(0, h - 14, w, 14, str);
    SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT

    //
    // Create a warning if the storey can't have an inside.
    //

    if (inside_failure) {
        QuickTextC(3, 40, "CANNOT GENERATE AN INSIDE FOR THE STOREY: YOU WON'T BE ABLE TO GO INTO THE BUILDING.", RED_COL);
        QuickTextC(4, 41, "CANNOT GENERATE AN INSIDE FOR THE STOREY: YOU WON'T BE ABLE TO GO INTO THE BUILDING.", WHITE_COL);
    }
}

//---------------------------------------------------------------

void BuildTab::HandleTab(MFPoint *current_point) {
    std::int32_t update = 0;

    ModeTab::HandleTab(current_point);
    KeyboardInterface();
}

inline std::int32_t is_point_in_box(std::int32_t x, std::int32_t y, std::int32_t left, std::int32_t top, std::int32_t w, std::int32_t h) {
    if (x > left && x < left + w && y > top && y < top + h)
        return (1);
    else
        return (0);
}
//---------------------------------------------------------------

std::int32_t BuildTab::KeyboardInterface() {
    if (Keys[KB_TAB]) {
        Keys[KB_TAB] = 0;
        AxisMode++;
        if (AxisMode > 3)
            AxisMode = 0;
        switch (AxisMode) {
        case 0:
            //				SetControlState(CTRL_BUILD_X_AXIS_FREE,CTRL_SELECTED);
            SetControlState(CTRL_BUILD_Y_AXIS_FREE, CTRL_DESELECTED);
            SetControlState(CTRL_BUILD_Z_AXIS_FREE, CTRL_DESELECTED);
            Axis = X_AXIS;
            break;
        case 1:
            //				SetControlState(CTRL_BUILD_X_AXIS_FREE,CTRL_DESELECTED);
            SetControlState(CTRL_BUILD_Y_AXIS_FREE, CTRL_SELECTED);
            SetControlState(CTRL_BUILD_Z_AXIS_FREE, CTRL_DESELECTED);
            Axis = Y_AXIS;
            break;
        case 2:
            //				SetControlState(CTRL_BUILD_X_AXIS_FREE,CTRL_DESELECTED);
            SetControlState(CTRL_BUILD_Y_AXIS_FREE, CTRL_DESELECTED);
            SetControlState(CTRL_BUILD_Z_AXIS_FREE, CTRL_SELECTED);
            Axis = Z_AXIS;
            break;
        case 3:
            //				SetControlState(CTRL_BUILD_X_AXIS_FREE,CTRL_SELECTED);
            SetControlState(CTRL_BUILD_Y_AXIS_FREE, CTRL_SELECTED);
            SetControlState(CTRL_BUILD_Z_AXIS_FREE, CTRL_SELECTED);
            Axis = X_AXIS | Y_AXIS | Z_AXIS;
            break;
        }

        SetWorkWindowBounds(ContentLeft() + 1, ContentTop() + 1, ContentWidth() - 1, ContentHeight() - 1);
        DrawControlSet();
        ShowWorkWindow(0);
    }

    if (Keys[KB_U]) {
        Keys[KB_U] = 0;
        HandleControl(CTRL_NEXT_STOREY);
    }
    if (Keys[KB_D]) {
        Keys[KB_D] = 0;
        HandleControl(CTRL_PREV_STOREY);
    }

    if (Keys[KB_M] ||
        Keys[KB_P]) {
        //
        // Where is the mouse in the world?
        //

        std::int32_t x;
        std::int32_t y;
        std::int32_t w;
        std::int32_t h;

        std::int32_t mx;
        std::int32_t my;
        std::int32_t mz;

        MFPoint mouse_point;

        mouse_point.X = MouseX;
        mouse_point.Y = MouseY;

        Parent->GlobalToLocal(&mouse_point);

        x = Parent->ContentLeft();
        y = Parent->ContentTop();
        w = Parent->ContentWidth();
        h = Parent->ContentHeight();

        CalcMapCoord(
            &mx,
            &my,
            &mz,
            x, y, w, h,
            &mouse_point);

        if (Keys[KB_M]) {
            Keys[KB_M] = 0;
            EXTRA_create_or_delete(EXTRA_TYPE_MIST, mx, mz);
        }
        if (Keys[KB_P]) {
            Keys[KB_P] = 0;
            EXTRA_create_or_delete(EXTRA_TYPE_PUDDLE, mx, mz);
        }

        RequestUpdate();
        SetWorkWindowBounds(ContentLeft() + 1, ContentTop() + 1, ContentWidth() - 1, ContentHeight() - 1);
        DrawControlSet();
        ShowWorkWindow(0);
    }

    return (0);
}

// #define	QDIST3(x,y,z)	(x>y ? (x>z ? x+(y>>2)+(z>>2) : z+(x>>2)+(y>>2)) : (y>z ? (y+(x>>2)+(z>>2) : z+(x>>2)+(y>>2) ))
// #define	QDIST3(x,y,z)	(x>y ? (x>z ? x+(y>>2)+(z>>2) : z+(x>>2)+(y>>2)) : (y>z ? (y+(x>>2)+(z>>2) : z+(x>>2)+(y>>2) ))

std::int32_t BuildTab::DragEngine(std::uint8_t flags, MFPoint *clicked_point) {
    std::int32_t wwx, wwy, www, wwh;
    std::int32_t screen_change = 0;
    std::int32_t last_world_mouse;

    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;

    {
        std::int32_t start_x = 0, start_y = 0, start_z = 0, flag = 0;
        std::int32_t old_x, old_y, old_z;
        std::int32_t nx, ny, nz;

        old_x = nx = engine.X;
        old_y = ny = engine.Y;
        old_z = nz = engine.Z;

        while (SHELL_ACTIVE && MiddleButton) {
            last_world_mouse = SetWorldMouse(0);
            if (last_world_mouse) {
                if (!flag) {
                    flag = 1;
                    start_x = engine.MousePosX << 8;
                    start_y = engine.MousePosY << 8;
                    start_z = engine.MousePosZ << 8;
                }

                nx = engine.MousePosX << 8;
                ny = engine.MousePosY << 8;
                nz = engine.MousePosZ << 8;

                engine.X = (old_x + (-nx + start_x));
                engine.Y = (old_y + (-ny + start_y));
                engine.Z = (old_z + (-nz + start_z));

                //				engine.Z=nz<<8;

                DrawModuleContent(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
                SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
                ShowWorkWindow(0);
                screen_change = 1;

                engine.X = old_x;
                engine.Y = old_y;
                engine.Z = old_z;
            }
        }
        if (flag) {
            engine.X = (old_x + (-nx + start_x));
            engine.Y = (old_y + (-ny + start_y));
            engine.Z = (old_z + (-nz + start_z));
        }
    }
    return (screen_change);
}

std::int32_t BuildTab::CalcMapCoord(std::int32_t *mapx, std::int32_t *mapy, std::int32_t *mapz, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, MFPoint *clicked_point) {
    std::int32_t width, count_across, count_high;
    std::int32_t mx, my, mz;
    std::int32_t dx, dy, dz;
    /*
            my=(engine.Y>>8)>>ELE_SHIFT;
            mx=(engine.X>>8)>>ELE_SHIFT;
            mz=(engine.Z>>8)>>ELE_SHIFT;

            width=((16<<5)*engine.Scale)>>16;
            LogText(" w %d h %d  click xy %d,%d  width %d res x %d res y %d \n",w,h,clicked_point->X,clicked_point->Y,width,(clicked_point->X-(w>>1))/(width+1),(clicked_point->Y-(h>>1))/(width+1));

            dx=(clicked_point->X-(w>>1));
            dy=(clicked_point->Y-(h>>1));

            if(dx>0)
                    *mapx=dx/(width+1)+mx;
            else
                    *mapx=((dx)/(width+1))+mx-1;


            if(dy>0)
                    *mapz=dy/(width+1)+mz;
            else
                    *mapz=((dy)/(width+1))+mz-1;

            *mapy=my;
    */

    mx = (ViewX >> (ELE_SHIFT));
    my = (ViewY >> (ELE_SHIFT));
    mz = (ViewZ >> (ELE_SHIFT));

    dx = (clicked_point->X - (w >> 1));
    dz = (clicked_point->Y - (h >> 1));
    if (dx < 0)
        dx = ((dx - (ViewSize >> 1)) << GridFlag) / ViewSize + (mx << GridFlag);
    else
        dx = ((dx + (ViewSize >> 1)) << GridFlag) / ViewSize + (mx << GridFlag);

    if (dz < 0)
        dz = ((dz - (ViewSize >> 1)) << GridFlag) / ViewSize + (mz << GridFlag);
    else
        dz = ((dz + (ViewSize >> 1)) << GridFlag) / ViewSize + (mz << GridFlag);

    *mapx = dx << (ELE_SHIFT - GridFlag);
    *mapy = 0;
    *mapz = dz << (ELE_SHIFT - GridFlag);

    return (1);
}

extern void insert_cube(std::int16_t x, std::int16_t y, std::int16_t z);
extern void remove_cube(std::int32_t x, std::int32_t y, std::int32_t z);

std::int32_t BuildTab::MouseInContent() {
    if (Mode == BUILD_MODE_CONT_STOREY) {
        std::int32_t x, y, w, h;
        std::int32_t wwx, wwy, www, wwh;

        wwx = WorkWindowRect.Left;
        wwy = WorkWindowRect.Top;
        www = WorkWindowRect.Width;
        wwh = WorkWindowRect.Height;

        x = Parent->ContentLeft();
        y = Parent->ContentTop();
        w = Parent->ContentWidth();
        h = Parent->ContentHeight();

        SetWorkWindowBounds(x, y, w - 1, h - 1);

        DrawModuleContent(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        ShowWorkWindow(0);

        SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT
    }
    return (0);
}

std::int32_t BuildTab::DragPaint(std::uint8_t flags) {
    /*
            std::int32_t	x,y,w,h;
            std::int32_t	wwx,wwy,www,wwh;
            std::int32_t	col;
            std::int32_t	screen_change=0;
            MFPoint		mouse_point;

            wwx=WorkWindowRect.Left;
            wwy=WorkWindowRect.Top;
            www=WorkWindowRect.Width;
            wwh=WorkWindowRect.Height;

            x=Parent->ContentLeft();
            y=Parent->ContentTop();
            w=Parent->ContentWidth();
            h=Parent->ContentHeight();

            col++;
            SetWorkWindowBounds(x,y,w-1,h-1);

                    while(SHELL_ACTIVE && LeftButton)
                    {
                            std::int32_t	mx,my,mz,index;

                            mouse_point.X	=	MouseX;
                            mouse_point.Y	=	MouseY;
                            Parent->GlobalToLocal(&mouse_point);

                            CalcMapCoord(&mx,&my,&mz,x,y,w,h,&mouse_point);

                            index=edit_map[(mx)][(my)].Depth[(mz)];
                            if(!index)
                                    insert_cube(mx,my,mz);



                            DrawModuleContent(Parent->ContentLeft()+1,Parent->ContentTop()+1,Parent->ContentWidth(),Parent->ContentHeight());
                            SetWorkWindowBounds(Parent->ContentLeft()+1,Parent->ContentTop()+1,Parent->ContentWidth(),Parent->ContentHeight());
                            ShowWorkWindow(0);
                            screen_change=1;
    //			editor_user_interface();
    //			KeyboardInterface();
                    }

            SetWorkWindowBounds(wwx,wwy,www,wwh); //RESTORE CLIP RECT
            return(screen_change);
    */
    return (0);
}

std::int32_t BuildTab::DragMark(std::uint8_t flags) {
    std::int32_t x, y, w, h;
    std::int32_t wwx, wwy, www, wwh;
    std::int32_t col = 0;
    std::int32_t screen_change = 0;
    std::int32_t mx, my, mz, index;

    MFPoint mouse_point;

    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;

    x = Parent->ContentLeft();
    y = Parent->ContentTop();
    w = Parent->ContentWidth();
    h = Parent->ContentHeight();

    col++;
    SetWorkWindowBounds(x, y, w - 1, h - 1);

    mouse_point.X = MouseX;
    mouse_point.Y = MouseY;
    Parent->GlobalToLocal(&mouse_point);

    CalcMapCoord(&mx, &my, &mz, x, y, w, h, &mouse_point);
    X1 = mx;
    Y1 = my;
    Z1 = mz;

    while (SHELL_ACTIVE && LeftButton) {
        mouse_point.X = MouseX;
        mouse_point.Y = MouseY;
        Parent->GlobalToLocal(&mouse_point);

        CalcMapCoord(&mx, &my, &mz, x, y, w, h, &mouse_point);
        X2 = mx;
        Y2 = my;
        Z2 = mz;

        DrawModuleContent(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        ShowWorkWindow(0);
        screen_change = 1;
    }

    SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT

    return (screen_change);
}

void move_insides(std::int32_t inside, std::int32_t dx, std::int32_t dy, std::int32_t dz) {
    std::int32_t c0;
    for (c0 = 0; c0 < MAX_STAIRS_PER_FLOOR; c0++) {
        if (room_ids[inside].StairFlags[c0]) {
            room_ids[inside].StairsX[c0] += dx / 256;
            room_ids[inside].StairsY[c0] += dz / 256;
        }
    }
    for (c0 = 0; c0 < MAX_ROOMS_PER_FLOOR; c0++) {
        if (room_ids[inside].Flag[c0]) {
            room_ids[inside].X[c0] += dx / 256;
            room_ids[inside].Y[c0] += dz / 256;
        }
    }
}

void move_storey(std::int32_t storey, std::int32_t dx, std::int32_t dy, std::int32_t dz) {
    std::int32_t wall;
    while (storey) {
        // LogText(" move storey %d \n",storey);
        storey_list[storey].DX += dx;
        storey_list[storey].DY += dy;
        storey_list[storey].DZ += dz;

        wall = storey_list[storey].WallHead;
        while (wall) {
            wall_list[wall].DX += dx;
            wall_list[wall].DY += dy;
            wall_list[wall].DZ += dz;
            wall = wall_list[wall].Next;
        }
        //		if(storey_list[storey].Roof)
        //			move_storey(storey_list[storey].Roof,dx,dy,dz);
        if (storey_list[storey].InsideStorey)
            move_storey(storey_list[storey].InsideStorey, dx, dy, dz);
        if (storey_list[storey].InsideIDIndex)
            move_insides(storey_list[storey].InsideIDIndex, dx, dy, dz);

        storey = storey_list[storey].Next;
    }
}

void move_building(std::int32_t building, std::int32_t dx, std::int32_t dy, std::int32_t dz) {
    std::int32_t storey, wall;

    if (building_list[building].BuildingFlags) {
        storey = building_list[building].StoreyHead;
        move_storey(storey, dx, dy, dz);
    }
}

extern std::int32_t get_new_inside_id();

std::int32_t copy_insides(std::int32_t insideid) {
    std::int32_t new_room;

    new_room = get_new_inside_id();
    if (new_room)
        room_ids[new_room] = room_ids[insideid];
    return (new_room);
}
std::int32_t duplicate_storey_list(std::int32_t storey, std::int32_t bh) {
    std::int32_t new_storey, prev_storey, first_storey = 0;
    std::int32_t wall, prev_wall, new_wall;

    prev_storey = 0;

    while (storey) {
        new_storey = get_new_storey();
        storey_list[new_storey] = storey_list[storey];

        if (storey_list[new_storey].InsideStorey) {
            storey_list[new_storey].InsideStorey = duplicate_storey_list(storey_list[new_storey].InsideStorey, bh);
        }
        if (storey_list[new_storey].InsideIDIndex) {
            storey_list[new_storey].InsideIDIndex = copy_insides(storey_list[new_storey].InsideIDIndex);
        }
        //		storey_list[new_storey].Roof=0;
        storey_list[new_storey].BuildingHead = bh;
        if (the_build->EditStorey == storey)
            the_build->EditStorey = new_storey;

        if (first_storey == 0)
            first_storey = new_storey;

        wall = storey_list[storey].WallHead;
        prev_wall = 0;
        while (wall) {
            new_wall = get_new_wall();
            wall_list[new_wall] = wall_list[wall];
            if (the_build->EditWall == wall)
                the_build->EditWall = new_wall;

            if (prev_wall)
                wall_list[prev_wall].Next = new_wall;
            else
                storey_list[new_storey].WallHead = new_wall;

            wall_list[new_wall].StoreyHead = new_storey;

            prev_wall = new_wall;
            wall = wall_list[wall].Next;
        }
        storey_list[new_storey].Prev = prev_storey;
        storey_list[prev_storey].Next = new_storey;

        /*
                        if(storey_list[storey].Roof)
                        {
                                std::int32_t	new_roof;
                                new_roof=duplicate_storey_list(storey_list[storey].Roof,bh);
                                storey_list[new_storey].Roof=new_roof;
                                storey_list[new_roof].Prev=new_storey;

                        }
                        */
        prev_storey = new_storey;
        storey = storey_list[storey].Next;
    }
    return (first_storey);
}

std::int32_t duplicate_building(std::int32_t building) {
    std::int32_t storey, wall;
    std::int32_t new_building, new_storey;

    new_building = get_new_building();
    building_list[new_building] = building_list[building];
    storey = building_list[building].StoreyHead;

    building_list[new_building].StoreyHead = duplicate_storey_list(storey, new_building);
    return (new_building);
}

std::int32_t BuildTab::DragBuilding(std::uint8_t flags, std::uint8_t type) {
    std::int32_t x, y, w, h;
    std::int32_t wwx, wwy, www, wwh;
    std::int32_t col = 0;
    std::int32_t mx, my, mz, index;

    MFPoint mouse_point;

    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;

    x = Parent->ContentLeft();
    y = Parent->ContentTop();
    w = Parent->ContentWidth();
    h = Parent->ContentHeight();

    col++;
    SetWorkWindowBounds(x, y, w - 1, h - 1);

    LogText("before EditBuilding %d  EditStorey %d EditWall %d \n", EditBuilding, EditStorey, EditWall);

    if (type == 1)
        EditBuilding = duplicate_building(EditBuilding);

    // LogText("after EditBuilding %d  EditStorey %d EditWall %d \n",EditBuilding,EditStorey,EditWall);

    while (SHELL_ACTIVE && (flags == LEFT_CLICK && LeftButton) || (flags == RIGHT_CLICK && RightButton)) {
        std::int32_t dx, dz;

        mouse_point.X = MouseX;
        mouse_point.Y = MouseY;
        Parent->GlobalToLocal(&mouse_point);

        CalcMapCoord(&mx, &my, &mz, x, y, w, h, &mouse_point);

        if (EditWall) {
            //			LogText(" editwall %d pos (%d,%d) \n",EditWall,wall_list[EditWall].DX,wall_list[EditWall].DZ);
            dx = mx - wall_list[EditWall].DX;
            dz = mz - wall_list[EditWall].DZ;
        } else {
            dx = mx - storey_list[EditStorey].DX;
            dz = mz - storey_list[EditStorey].DZ;
        }
        move_building(EditBuilding, dx, 0, dz);

        DrawModuleContent(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        ShowWorkWindow(0);
    }

    SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT
    if (mouse_point.Y > h)
        return (0);
    else
        return (1);
}

//
// moves all vertices above map co-ord map_x,map_z to mx,mz
//
void move_all_vertices(std::int32_t map_x, std::int32_t map_z, std::int32_t mx, std::int32_t mz) {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_STOREYS; c0++) {
        //		if(storey_list[c0].StoreyFlags)
        {
            if (storey_list[c0].DX == map_x && storey_list[c0].DZ == map_z) {
                storey_list[c0].DX = mx;
                storey_list[c0].DZ = mz;
            }
        }
    }

    for (c0 = 1; c0 < MAX_WALLS; c0++) {
        //		if(wall_list[c0].WallFlags)
        {
            if (wall_list[c0].DX == map_x && wall_list[c0].DZ == map_z) {
                wall_list[c0].DX = mx;
                wall_list[c0].DZ = mz;
            }
        }
    }
}

std::int32_t BuildTab::DragVertex(std::uint8_t flags) {
    std::int32_t x, y, w, h;
    std::int32_t wwx, wwy, www, wwh;
    std::int32_t col = 0;
    std::int32_t mx, my, mz, index;

    MFPoint mouse_point;
    std::int32_t mouse_screen_y = 0;

    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;

    x = Parent->ContentLeft();
    y = Parent->ContentTop();
    w = Parent->ContentWidth();
    h = Parent->ContentHeight();

    col++;
    SetWorkWindowBounds(x, y, w - 1, h - 1);

    //	LogText("before EditBuilding %d  EditStorey %d EditWall %d \n",EditBuilding,EditStorey,EditWall);

    // LogText("after EditBuilding %d  EditStorey %d EditWall %d \n",EditBuilding,EditStorey,EditWall);

    while (SHELL_ACTIVE && (flags == LEFT_CLICK && LeftButton) || (flags == RIGHT_CLICK && RightButton)) {
        mouse_point.X = MouseX;
        mouse_point.Y = MouseY;
        Parent->GlobalToLocal(&mouse_point);

        CurrentY = storey_list[EditStorey].DY;

        mouse_screen_y = mouse_point.Y;

        //
        // This corrects the mouse offset problem
        //
        mouse_point.X += ((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
        mouse_point.Y += -((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);

        CalcMapCoord(&mx, &my, &mz, x, y, w, h, &mouse_point);

        if (ShiftFlag) {
            std::int32_t dx, dz;

            if (EditWall) {
                //			LogText(" editwall %d pos (%d,%d) \n",EditWall,wall_list[EditWall].DX,wall_list[EditWall].DZ);
                dx = mx - wall_list[EditWall].DX;
                dz = mz - wall_list[EditWall].DZ;
            } else {
                dx = mx - storey_list[EditStorey].DX;
                dz = mz - storey_list[EditStorey].DZ;
            }
            move_building(EditBuilding, dx, 0, dz);

        } else if (Keys[KB_A]) {
            std::int32_t map_x, map_z;

            if (EditWall) {
                map_x = wall_list[EditWall].DX;
                map_z = wall_list[EditWall].DZ;
            } else {
                map_x = storey_list[EditStorey].DX;
                map_z = storey_list[EditStorey].DZ;
            }
            move_all_vertices(map_x, map_z, mx, mz);

        } else {
            if (EditWall) {
                wall_list[EditWall].DX = mx;
                wall_list[EditWall].DZ = mz;
            } else {
                storey_list[EditStorey].DX = mx;
                storey_list[EditStorey].DZ = mz;
            }
        }

        DrawModuleContent(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth(), Parent->ContentHeight());
        ShowWorkWindow(0);
    }

    SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT
    if (mouse_screen_y > h)
        return (0);
    else
        return (1);
}

std::int32_t find_previous_wall(std::int32_t edit_storey, std::int32_t wall) {
    std::int32_t index;
    std::int32_t prev = 0;
    index = storey_list[edit_storey].WallHead;
    while (index) {
        if (index == wall)
            return (prev);

        prev = index;
        index = wall_list[index].Next;
    }
    return (-1);
}

void BuildTab::DeleteVertex() {
    std::int32_t prev;
    if (EditWall) {
        prev = find_previous_wall(EditStorey, EditWall);
        if (prev > 0) {
            wall_list[prev].Next = wall_list[EditWall].Next;
            free_wall(EditWall);
            EditWall = 0;
        } else { // prev is a storeyhead
            storey_list[EditStorey].WallHead = wall_list[EditWall].Next;
            free_wall(EditWall);
            EditWall = 0;
        }
    } else {
        std::int32_t next;
        next = storey_list[EditStorey].WallHead;
        if (next) {
            storey_list[EditStorey].DX = wall_list[next].DX;
            storey_list[EditStorey].DZ = wall_list[next].DZ;
            storey_list[EditStorey].WallHead = wall_list[next].Next;
            free_wall(next);
        }
    }
}

std::int32_t BuildTab::ClickNearWall(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, MFPoint *mouse_point) {
    std::int32_t mx, mz, rect_size;
    EdRect rect;
    std::int32_t best_building, best_storey = 0, best_wall = 0, best_dist = 0x7fffffff, dist;
    std::int32_t roof_flag = 0, building;

    mx = ViewX >> ELE_SHIFT;
    mz = ViewZ >> ELE_SHIFT;

    rect_size = ViewSize >> 2;

    for (building = 0; building < MAX_BUILDINGS; building++) {
        if (building_list[building].BuildingFlags & 1) {
            std::int32_t x1, y1, z1, x2, y2, z2, px, py, pz, index;
            std::int32_t storey_index;

            storey_index = building_list[building].StoreyHead;

            while (storey_index) {
                x1 = storey_list[storey_index].DX;
                z1 = storey_list[storey_index].DZ;
                CurrentY = storey_list[storey_index].DY;
                index = storey_list[storey_index].WallHead;

                //			x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                //			z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);

                x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
                z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
                AddHeightOffset(&x1, &z1);
                if (index) {
                    px = x1;
                    pz = z1;

                    while (index) {
                        x1 = wall_list[index].DX;
                        z1 = wall_list[index].DZ;

                        //					x1=((x1>>ELE_SHIFT)-mx)*ViewSize+(w>>1);
                        //					z1=((z1>>ELE_SHIFT)-mz)*ViewSize+(h>>1);
                        x1 = ((((x1) - (ViewX)) * ViewSize) / ELE_SIZE) + (w >> 1);
                        z1 = ((((z1) - (ViewZ)) * ViewSize) / ELE_SIZE) + (h >> 1);
                        AddHeightOffset(&x1, &z1);

                        //					dist=dist_between_vertex_and_vector(px,pz,x1,z1,mouse_point->X,mouse_point->Y);
                        dist = dist_to_line(px, pz, x1, z1, mouse_point->X, mouse_point->Y);
                        if (dist < best_dist) {
                            best_building = building;
                            best_storey = storey_index;
                            best_wall = index;
                            best_dist = dist;
                        }
                        index = wall_list[index].Next;
                        px = x1;
                        pz = z1;
                    }
                }
                //			storey_index=storey_list[storey_index].Next;
                if (roof_flag) {
                    storey_index = storey_list[storey_list[storey_index].Prev].Next;
                    roof_flag = 0;
                } else {
                    /*
                    std::int32_t	temp_index;
                    temp_index=storey_list[storey_index].Roof;
                    if(temp_index)
                    {
                            storey_index=temp_index;
                            roof_flag=1;

                    }
                    else
                    */
                    {
                        storey_index = storey_list[storey_index].Next;
                        roof_flag = 0;
                    }
                }
            }
        }
    }
    if (best_storey) {
        EditBuilding = best_building;
        EditStorey = best_storey;
        EditY = 0; // storey_list[EditStorey].DY;
        EditWall = best_wall;

        DrawTabContent();

        return (1);
    }
    return (0);
}

static ControlDef popup_def = {POPUP_MENU, 0, ""};
MenuDef2 wall_popup[] =
    {
        {"~Poly Windows",  0},
        {"~Fence Post1",   0},
        {"~Roof Rim2",     0},
        {"~Recessed Door", 0},
        {"~Roof has Rim",  0},
        {"~CLIMBABLE",     0},
        {"~Barbed Top",    0},

        //	{	"~Archside",	0	},
        //	{	"~ArchTop",	0	},
        {"!",              0}
};

std::int32_t BuildTab::WallOptions() {
    std::uint8_t flags;
    std::uint32_t c0,
        control_id;
    CPopUp *the_control = 0;
    MFPoint local_point;
    std::uint8_t old_flags;

    local_point.X = MouseX;
    local_point.Y = MouseY;

    //	SetWorkWindowBounds(0,0,0,0);
    Parent->GlobalToLocal(&local_point);

    popup_def.ControlLeft = local_point.X + 4;
    popup_def.ControlTop = local_point.Y - 4;

    flags = 0; // wall_list[EditWall].WallFlags;

    old_flags = flags;
    wall_popup[7].ItemFlags = 0;
    for (c0 = 0; c0 < 7; c0++) {
        wall_popup[c0].ItemFlags = 0;
        wall_popup[c0].MutualExclusiveID = 0;
        if (flags & (1 << c0))
            wall_popup[c0].ItemFlags |= MENU_CHECK_MASK;
    }
    if (wall_list[EditWall].WallFlags & FLAG_WALL_AUTO_WINDOWS)
        wall_popup[0].ItemFlags |= MENU_CHECK_MASK;

    if (wall_list[EditWall].WallFlags & FLAG_WALL_FENCE_POST1)
        wall_popup[1].ItemFlags |= MENU_CHECK_MASK;

    //	if(wall_list[EditWall].WallFlags&FLAG_WALL_FENCE_POST2)
    //		wall_popup[2].ItemFlags	|=	MENU_CHECK_MASK;

    if (wall_list[EditWall].WallFlags & FLAG_WALL_RECESSED)
        wall_popup[3].ItemFlags |= MENU_CHECK_MASK;

    if (storey_list[wall_list[EditWall].StoreyHead].StoreyFlags & FLAG_STOREY_ROOF_RIM2)
        wall_popup[2].ItemFlags |= MENU_CHECK_MASK;

    if (storey_list[wall_list[EditWall].StoreyHead].StoreyFlags & FLAG_STOREY_ROOF_RIM)
        wall_popup[4].ItemFlags |= MENU_CHECK_MASK;

    if (wall_list[EditWall].WallFlags & FLAG_WALL_CLIMBABLE)
        wall_popup[5].ItemFlags |= MENU_CHECK_MASK;

    if (wall_list[EditWall].WallFlags & FLAG_WALL_BARB_TOP)
        wall_popup[6].ItemFlags |= MENU_CHECK_MASK;

    /*
            if(wall_list[EditWall].WallFlags&FLAG_WALL_ARCH_TOP)
                    wall_popup[6].ItemFlags	|=	MENU_CHECK_MASK;
    */

    wall_popup[1].MutualExclusiveID = 1;
    wall_popup[2].MutualExclusiveID = 1;

    popup_def.TheMenuDef = wall_popup;
    the_control = new CPopUp(&popup_def);
    //	ShowWorkScreen(1);
    control_id = the_control->TrackControl(&local_point);
    //	while(RightButton&&SHELL_ACTIVE);

    flags = 0;

    //	wall_list[EditWall].WindowCount=0;

    if (wall_popup[0].ItemFlags & MENU_CHECK_MASK)
        wall_list[EditWall].WallFlags |= FLAG_WALL_AUTO_WINDOWS;
    else
        wall_list[EditWall].WallFlags &= ~FLAG_WALL_AUTO_WINDOWS;

    if (wall_popup[1].ItemFlags & MENU_CHECK_MASK)
        wall_list[EditWall].WallFlags |= FLAG_WALL_FENCE_POST1;
    else
        wall_list[EditWall].WallFlags &= ~FLAG_WALL_FENCE_POST1;

    if (wall_popup[3].ItemFlags & MENU_CHECK_MASK)
        wall_list[EditWall].WallFlags |= FLAG_WALL_RECESSED;
    else
        wall_list[EditWall].WallFlags &= ~FLAG_WALL_RECESSED;

    //	if(wall_popup[2].ItemFlags&MENU_CHECK_MASK)
    //		wall_list[EditWall].WallFlags|=FLAG_WALL_FENCE_POST2;
    //	else
    //		wall_list[EditWall].WallFlags&=~FLAG_WALL_FENCE_POST2;

    if (wall_popup[2].ItemFlags & MENU_CHECK_MASK)
        storey_list[wall_list[EditWall].StoreyHead].StoreyFlags |= FLAG_STOREY_ROOF_RIM2;
    else
        storey_list[wall_list[EditWall].StoreyHead].StoreyFlags &= ~FLAG_STOREY_ROOF_RIM2;

    if (wall_popup[4].ItemFlags & MENU_CHECK_MASK)
        storey_list[wall_list[EditWall].StoreyHead].StoreyFlags |= FLAG_STOREY_ROOF_RIM;
    else
        storey_list[wall_list[EditWall].StoreyHead].StoreyFlags &= ~FLAG_STOREY_ROOF_RIM;

    if (wall_popup[5].ItemFlags & MENU_CHECK_MASK)
        wall_list[EditWall].WallFlags |= FLAG_WALL_CLIMBABLE;
    else
        wall_list[EditWall].WallFlags &= ~FLAG_WALL_CLIMBABLE;

    if (wall_popup[6].ItemFlags & MENU_CHECK_MASK)
        wall_list[EditWall].WallFlags |= FLAG_WALL_BARB_TOP;
    else
        wall_list[EditWall].WallFlags &= ~FLAG_WALL_BARB_TOP;
    /*
            if(wall_popup[6].ItemFlags&MENU_CHECK_MASK)
                    wall_list[EditWall].WallFlags|=FLAG_WALL_ARCH_TOP;
            else
                    wall_list[EditWall].WallFlags&=~FLAG_WALL_ARCH_TOP;
    */
    if (the_control) {
        delete the_control;
    }
    return (1);
}

MenuDef2 roof_popup[] =
    {
        {"~Flat Roof",      0},
        {"~Overlap Small",  0},
        {"~Overlap Medium", 0},
        {"~Walled",         0},
        {"~Reccesed",       0},
        {"!",               0}
};

std::int32_t BuildTab::RoofOptions() {
    std::uint32_t flags = 0;
    std::uint32_t c0,
        control_id;
    CPopUp *the_control = 0;
    MFPoint local_point;

    local_point.X = MouseX;
    local_point.Y = MouseY;

    Parent->GlobalToLocal(&local_point);

    popup_def.ControlLeft = local_point.X + 4;
    popup_def.ControlTop = local_point.Y - 4;

    flags = storey_list[EditStorey].StoreyFlags;
    roof_popup[5].ItemFlags = 0;
    for (c0 = 0; c0 < 5; c0++) {
        roof_popup[c0].ItemFlags = 0;
        roof_popup[c0].MutualExclusiveID = 0;
        if (flags & (1 << (c0 + 1)))
            roof_popup[c0].ItemFlags |= MENU_CHECK_MASK;
        else
            roof_popup[c0].ItemFlags &= ~MENU_CHECK_MASK;
    }

    for (c0 = 1; c0 < 4; c0++) {
        //		roof_popup[c0].MutualExclusiveID=1;
    }

    popup_def.TheMenuDef = roof_popup;
    the_control = new CPopUp(&popup_def);
    control_id = the_control->TrackControl(&local_point);
    flags = 0;

    //	storey_list[EditStorey].StoreyFlags&=~0x1f;
    for (c0 = 0; c0 < 5; c0++) {
        if (roof_popup[c0].ItemFlags & MENU_CHECK_MASK)
            storey_list[EditStorey].StoreyFlags |= 1 << (c0 + 1);
        else
            storey_list[EditStorey].StoreyFlags &= ~(1 << (c0 + 1));
    }

    if (the_control) {
        delete the_control;
    }
    return (1);
}

MenuDef2 fence_popup[] =
    {
        {"~100% Angle topped fence"},
        {"~200% Barbed wire"},
        {"~100% Barbed wire"},
        {"~200% Chain fence"},
        {"~100% Chain fence"},
        {"~ 75% Chain fence"},
        {"~ 33% Chain fence"},

        {"~100% Unclimbable fence"},
        {"~200% Unclimbable fence"},
        {"~300% Unclimbable fence"},

        {"~200% Door 180 degree swivel"},
        {"~200% Door  90 degree swivel"},

        {"!"}};

std::int32_t BuildTab::FenceOptions() {
    std::uint32_t flags = 0;
    std::uint32_t c0,
        control_id;
    CPopUp *the_control = 0;
    MFPoint local_point;
    char str[100];

    local_point.X = MouseX;
    local_point.Y = MouseY;

    Parent->GlobalToLocal(&local_point);

    popup_def.ControlLeft = local_point.X + 4;
    popup_def.ControlTop = local_point.Y - 4;

    if (storey_list[EditStorey].ExtraFlags & FLAG_STOREY_EXTRA_UNCLIMBABLE) {
        //
        // Might be a door because all doors are unclimbable.
        //

        if (storey_list[EditStorey].StoreyType == STOREY_TYPE_OUTSIDE_DOOR) {
            if (storey_list[EditStorey].ExtraFlags & FLAG_STOREY_EXTRA_90DEGREE) {
                //
                // 90 degree door.
                //

                flags = 1 << 12;
            } else {
                //
                // 180 degree door.
                //

                flags = 1 << 11;
            }
        } else {
            switch (storey_list[EditStorey].Height) {
            case 256: flags = 1 << 8; break;
            case 512: flags = 1 << 9; break;
            case 786: flags = 1 << 10; break;
            }
        }
    } else {
        switch (storey_list[EditStorey].StoreyType) {
        case STOREY_TYPE_FENCE:
            flags = 1 << 1;
            break;
        case STOREY_TYPE_FENCE_BRICK:

            if (storey_list[EditStorey].Height == 256) {
                flags = 1 << 3;
            } else {
                flags = 1 << 2;
            }
            break;
        case STOREY_TYPE_FENCE_FLAT:
            sprintf(str, "HEIGHT %d \n", storey_list[EditStorey].Height);
            QuickText(0, 0, str, 100);
            QuickText(100, 100, str, 100);
            QuickText(200, 200, str, 100);

            switch (storey_list[EditStorey].Height) {
            case (512):
                flags = 1 << 4;
                break;
            case (256):
                flags = 1 << 5;
                break;
            case (256 - 64):
                flags = 1 << 6;
                break;
            case (256 - 128):
                flags = 1 << 7;
                break;
            }

            break;
        }
    }

    fence_popup[6].ItemFlags = 0;
    for (c0 = 0; c0 < 12; c0++) {
        fence_popup[c0].ItemFlags = 0;
        fence_popup[c0].MutualExclusiveID = 1;
        if (flags & (1 << (c0 + 1)))
            fence_popup[c0].ItemFlags |= MENU_CHECK_MASK;
        else
            fence_popup[c0].ItemFlags &= ~MENU_CHECK_MASK;
    }

    popup_def.TheMenuDef = fence_popup;
    the_control = new CPopUp(&popup_def);
    control_id = the_control->TrackControl(&local_point);
    flags = 0;

    //	storey_list[EditStorey].StoreyFlags&=~0x1f;

    for (c0 = 0; c0 < 12; c0++) {
        if (fence_popup[c0].ItemFlags & MENU_CHECK_MASK) {
            switch (c0) {
            case 0:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                storey_list[EditStorey].Height = 256;
                break;

            case 1:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_BRICK;
                storey_list[EditStorey].Height = 512;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;

            case 2:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_BRICK;
                storey_list[EditStorey].Height = 256;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;

            case 3:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 512;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;
            case 4:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 256;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;
            case 5:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 256 - 64;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;
            case 6:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 128;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;

            case 7:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 256;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;
            case 8:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 512;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;
            case 9:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].Height = 768;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_UNCLIMBABLE;
                break;

            case 10:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_OUTSIDE_DOOR;
                storey_list[EditStorey].Height = 512;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_UNCLIMBABLE;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_90DEGREE;
                break;

            case 11:
                storey_list[EditStorey].StoreyType = STOREY_TYPE_OUTSIDE_DOOR;
                storey_list[EditStorey].Height = 512;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_UNCLIMBABLE;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_90DEGREE;
                break;
            }

            break;
        }
        /*
                                storey_list[EditStorey].StoreyFlags|=1<<(c0+1);
                        else
                                storey_list[EditStorey].StoreyFlags&=~(1<<(c0+1));
        */
    }

    if (the_control) {
        delete the_control;
    }
    return (1);
}

std::int32_t count_wall_size(std::uint16_t storey) {
    std::int32_t count, index;
    index = storey_list[storey].WallHead;
    count = 0;

    while (index && count < 1000) {
        count++;
        index = wall_list[index].Next;
        //		if(count<10)
        //		LogText(" size count %d index %d \n",count,index);
    }
    return (count);
}

std::int32_t find_n_from_end(std::int32_t n, std::uint16_t storey) {
    std::int32_t count, index;

    count = count_wall_size(storey);
    //	LogText(" find n %d from end count %d \n",n,count);

    count = count - n;
    if (count < 0)
        return (-1);

    index = storey_list[storey].WallHead;
    count--;
    if (count < 0)
        return (0);
    while (index && count) {
        count--;
        index = wall_list[index].Next;
        //	LogText(" find count %d index %d \n",count,index);
    }
    return (index);
}

void show_storey(std::uint16_t index) {
    std::int32_t count = 0;
    LogText("[%d,%d] ->", storey_list[index].DX, storey_list[index].DZ);
    index = storey_list[index].WallHead;
    while (index && count++ < 10) {
        LogText("%d(%d,%d) ->", index, wall_list[index].DX, wall_list[index].DZ);
        index = wall_list[index].Next;
    }
    LogText("\n");
}

void flip_storey(std::uint16_t storey) {
    std::int32_t c0, index, end_index, end_index_next;
    std::int32_t size, prev = 0;
    std::int32_t end_x, end_z;
    std::int32_t new_storey;

    index = find_n_from_end(0, storey);

    end_x = storey_list[storey].DX;
    end_z = storey_list[storey].DZ;

    new_storey = get_new_storey();
    //	storey_list[new_storey].StoreyFlags|=1;
    storey_list[new_storey] = storey_list[storey];
    storey_list[new_storey].WallHead = 0;

    storey_list[new_storey].DX = wall_list[index].DX;
    storey_list[new_storey].DZ = wall_list[index].DZ;

    size = count_wall_size(storey);
    //	LogText(" count wall size = %d \n",size);

    for (c0 = 0; c0 < size - 1; c0++) {
        //		LogText(" flip part %d \n",c0);
        index = get_new_wall();
        wall_list[index].WallFlags |= 1; //|FLAG_WALL_AUTO_WINDOWS;
        if (prev)
            wall_list[prev].Next = index;
        else
            storey_list[new_storey].WallHead = index;

        end_index = find_n_from_end(1, storey);
        //		if(end_index==0)
        //			end_index_next=storey_list[storey].WallHead;
        //		else
        end_index_next = wall_list[end_index].Next;
        //		LogText(" end_index %d ei next %d\n",end_index,end_index_next);

        wall_list[index] = wall_list[end_index_next];
        wall_list[index].DX = wall_list[end_index].DX;
        wall_list[index].DZ = wall_list[end_index].DZ;

        wall_list[end_index].Next = 0;
        wall_list[end_index_next].WallFlags = 0;
        //		show_storey(storey);

        prev = index;
    }
    index = get_new_wall();
    wall_list[index].WallFlags |= 1; //|FLAG_WALL_AUTO_WINDOWS;
    end_index = find_n_from_end(0, storey);

    wall_list[index] = wall_list[end_index];
    wall_list[index].DX = end_x;
    wall_list[index].DZ = end_z;
    if (prev)
        wall_list[prev].Next = index;
    else
        storey_list[new_storey].WallHead = index;

    storey_list[storey] = storey_list[new_storey];
}

void BuildTab::CheckStoreyIntegrity(std::uint16_t storey) {
    std::int32_t x1, z1, x2, z2, x3, z3;
    std::int32_t wall;

    if (storey_list[storey].WallHead && wall_list[storey_list[storey].WallHead].Next) {
        x1 = storey_list[storey].DX;
        z1 = storey_list[storey].DZ;

        wall = storey_list[storey].WallHead;

        x2 = wall_list[wall].DX;
        z2 = wall_list[wall].DZ;

        wall = wall_list[wall].Next;

        x3 = wall_list[wall].DX;
        z3 = wall_list[wall].DZ;

        /*
                        if(!is_it_clockwise_xy(x1,z1,x2,z2,x3,z3))
                        {
                                flip_storey(storey);

                        }
        */
    }
}

std::int32_t BuildTab::HandleModuleContentClick(MFPoint *clicked_point, std::uint8_t flags, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h) {
    std::int16_t thing;
    std::int32_t index;
    std::int16_t bright;
    std::int32_t mx, my, mz;
    std::int32_t ret;
    switch (Mode) {
    case BUILD_MODE_WAIT:
        switch (flags) {
        case LEFT_CLICK:
            if (ret = ClickInVertex(x, y, w, h, clicked_point, flags)) {
                if (ret > 0) {
                    switch (storey_list[EditStorey].StoreyType) {
                    case STOREY_TYPE_FIRE_ESCAPE: {
                        if (EditWall < 0) {
                            EditWall = 0;
                            storey_list[EditStorey].Height++;
                            return (1);
                        }
                    } break;
                    case STOREY_TYPE_LADDER:
                        if (EditWall < 0) {
                            std::int32_t size = 4;
                            if (ShiftFlag)
                                size = 1;

                            EditWall = 0;
                            storey_list[EditStorey].Height += size;
                            return (1);
                        }
                        break;
                    case STOREY_TYPE_STAIRCASE:
                        if (EditWall < 0) {
                            EditWall = 0;
                            storey_list[EditStorey].Info1++;
                            return (1);
                        }

                        break;
                    case STOREY_TYPE_CABLE:
                        if (EditWall == 99999) {
                            EditWall = 0;
                            storey_list[EditStorey].DY += 64;
                            return (1);
                        } else if (EditWall < 0) {
                            wall_list[-EditWall].DY += 64;
                            return (1);
                        }
                        break;
                    }

                    LogText(" dragging editwall %d\n", EditWall);
                    if (ShiftFlag) {
                        DragBuilding(flags, 0);
                    } else if (ControlFlag) {
                        DragBuilding(flags, 1);
                    } else if (DragVertex(flags) == 0) {
                        DeleteVertex();
                        return (1);
                    }
                } else {
                    return (1);
                }
            }
            // drag vertex
            break;
        case RIGHT_CLICK:
            if (ret = ClickInVertex(x, y, w, h, clicked_point, flags)) {
                if (ret > 0) {
                    switch (storey_list[EditStorey].StoreyType) {
                    case STOREY_TYPE_FIRE_ESCAPE: {
                        if (EditWall < 0) {
                            EditWall = 0;
                            storey_list[EditStorey].Height--;
                            return (1);
                        }
                    } break;
                    case STOREY_TYPE_LADDER:
                        if (EditWall < 0) {
                            std::int32_t size = 4;
                            if (ShiftFlag)
                                size = 1;

                            EditWall = 0;
                            storey_list[EditStorey].Height -= size;
                            return (1);
                        }
                        break;

                    case STOREY_TYPE_STAIRCASE:
                        if (EditWall < 0) {
                            EditWall = 0;
                            storey_list[EditStorey].Info1--;
                            return (1);
                        }
                        break;
                    case STOREY_TYPE_CABLE:
                        if (EditWall == 99999) {
                            EditWall = 0;
                            storey_list[EditStorey].DY -= 64;
                            return (1);
                        } else if (EditWall < 0) {
                            wall_list[-EditWall].DY -= 64;
                            return (1);
                        }
                        break;
                    }
                    if (EditWall > 0) {
                        std::int32_t temp_next;
                        temp_next = wall_list[EditWall].Next;
                        index = get_new_wall();
                        wall_list[index].StoreyHead = EditStorey;
                        wall_list[index].WallFlags = 1; //|FLAG_WALL_AUTO_WINDOWS;

                        wall_list[EditWall].Next = index;

                        wall_list[index].DX = wall_list[EditWall].DX;
                        wall_list[index].DZ = wall_list[EditWall].DZ;
                        wall_list[index].Next = temp_next;
                        //								wall_list[index].WindowCount=0;
                        EditWall = index;
                        if (DragVertex(flags) == 0) {
                            DeleteVertex();
                            return (1);
                        }
                    } else {
                        // trying to drag one of the root
                    }
                } else {
                    return (1);
                }

            } else {
                if (ClickNearWall(x, y, w, h, clicked_point)) {
                    SetWorkWindowBounds(x, y, w - 1, h - 1);
                    switch (storey_list[EditStorey].StoreyType) {
                    case STOREY_TYPE_ROOF:
                        RoofOptions();
                        break;
                    case STOREY_TYPE_NORMAL:
                        WallOptions();
                        break;
                    case STOREY_TYPE_FENCE:
                    case STOREY_TYPE_FENCE_BRICK:
                    case STOREY_TYPE_FENCE_FLAT:
                    case STOREY_TYPE_OUTSIDE_DOOR:
                        FenceOptions();
                        break;
                    default:
                        WallOptions();
                        break;
                    }
                    return (1);
                }
            }
            // delete vertex
            break;
        }
        break;

    case BUILD_MODE_PLACE_STOREY:
        switch (flags) {
        case LEFT_CLICK:
            //
            // This corrects the mouse offset problem
            //
            CurrentY = storey_list[EditStorey].DY;
            clicked_point->X += ((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
            clicked_point->Y += -((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);

            CalcMapCoord(&mx, &my, &mz, x, y, w, h, clicked_point);
            storey_list[EditStorey].DX = mx;
            //				storey_list[EditStorey].DY=my;
            storey_list[EditStorey].DZ = mz;
            Mode = BUILD_MODE_CONT_STOREY;
            return (1);
        case RIGHT_CLICK:
            Mode = BUILD_MODE_WAIT;
            break;
        }
        break;

    case BUILD_MODE_CONT_STOREY:
        switch (flags) {
        case LEFT_CLICK:
            //
            // This corrects the mouse offset problem
            //
            CurrentY = storey_list[EditStorey].DY;
            clicked_point->X += ((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);
            clicked_point->Y += -((-CurrentY) * (ViewSize + 3)) / (BLOCK_SIZE << 3);

            CalcMapCoord(&mx, &my, &mz, x, y, w, h, clicked_point);
            index = get_new_wall();
            wall_list[index].StoreyHead = EditStorey;
            wall_list[index].WallFlags = 1; //|FLAG_WALL_AUTO_WINDOWS;
                                            //						wall_list[index].WindowCount=0;
            if (EditWall) {
                wall_list[EditWall].Next = index;
            } else {
                storey_list[EditStorey].WallHead = index;
            }

            EditWall = index;

            wall_list[EditWall].DX = mx;
            if (EditStorey > 0 && storey_list[EditStorey].StoreyType == STOREY_TYPE_CABLE) {
                wall_list[EditWall].DY = storey_list[EditStorey].DY;
                wall_list[EditWall].TextureStyle2 = 4;
            }

            wall_list[EditWall].DZ = mz;
            wall_list[EditWall].Next = 0;
            if (storey_list[EditStorey].StoreyType == STOREY_TYPE_LADDER) {
                Mode = BUILD_MODE_WAIT;
            }

            break;
        case RIGHT_CLICK:
            Mode = BUILD_MODE_WAIT;
            CheckStoreyIntegrity(EditStorey);
            switch (storey_list[EditStorey].StoreyType) {
            case STOREY_TYPE_FIRE_ESCAPE:

                break;
            }

            return (1);
        }
        break;
    }
    return (0);
}

std::uint16_t BuildTab::HandleTabClick(std::uint8_t flags, MFPoint *clicked_point) {
    std::uint16_t control_id;
    Control *current_control;
    MFPoint local_point;

    // This is a fudge to update the front screen buffer.
    ShowWorkScreen(0);

    switch (flags) {
    case NO_CLICK:
        break;
    case LEFT_CLICK:
        SetWorkWindowBounds(ContentLeft() + 1, ContentTop() + 1, ContentWidth() - 1, ContentHeight() - 1);
        local_point = *clicked_point;
        GlobalToLocal(&local_point);
        {
            current_control = GetControlList();
            while (current_control) {
                if (!(current_control->GetFlags() & CONTROL_INACTIVE) && current_control->PointInControl(&local_point)) {
                    // Handle control.
                    control_id = current_control->TrackControl(&local_point);
                    HandleControl(control_id);

                    // Tidy up display.
                    if (LockWorkScreen()) {
                        DrawTab();
                        UnlockWorkScreen();
                    }
                    ShowWorkWindow(0);

                    return control_id;
                }
                current_control = current_control->GetNextControl();
            }
        }

        break;
    case RIGHT_CLICK:
        SetWorkWindowBounds(ContentLeft() + 1, ContentTop() + 1, ContentWidth() - 1, ContentHeight() - 1);
        local_point = *clicked_point;
        GlobalToLocal(&local_point);
        break;
    }
    return 0;
}

//---------------------------------------------------------------

std::int32_t BuildTab::DoZoom() {
    std::int32_t update = 0;
    if (Keys[KB_I]) {
        ViewSize++;
        if (ViewSize > 100)
            ViewSize = 100;
        update = 2;
    }

    if (Keys[KB_O]) {
        ViewSize--;
        if (ViewSize < 3)
            ViewSize = 3;
        update = 2;
    }
    return (update);
}

std::int32_t BuildTab::DoKeys() {
    std::int32_t update = 0;
    std::int32_t scroll_step;

    scroll_step = 110 / (ViewSize + 39);
    if (scroll_step < 1)
        scroll_step = 1;

    scroll_step <<= ELE_SHIFT;

    update = DoZoom();
    if (Keys[KB_LEFT]) {
        ViewX -= scroll_step;
        update = 2;
    }
    if (Keys[KB_RIGHT]) {
        ViewX += scroll_step;
        update = 2;
    }
    if (!ShiftFlag) {
        if (Keys[KB_UP]) {
            ViewZ -= scroll_step;
            update = 2;
        }
        if (Keys[KB_DOWN]) {
            ViewZ += scroll_step;
            update = 2;
        }
    } else {
        if (Keys[KB_UP]) {
            if (storey_list[EditStorey].Next) {
                Keys[KB_UP] = 0;
                EditStorey = storey_list[EditStorey].Next;
                EditY = 0; // storey_list[EditStorey].DY;
            }
            update = 2;
        }
        if (Keys[KB_DOWN]) {
            if (storey_list[EditStorey].Prev) {
                Keys[KB_DOWN] = 0;
                EditStorey = storey_list[EditStorey].Prev;
                EditY = 0; // storey_list[EditStorey].DY;
            }
            update = 2;
        }
    }

    return (update);
}

std::int32_t BuildTab::SetWorldMouse(std::uint32_t flag) {
    MFPoint mouse_point;
    MFPoint local_point;
    SVector point, out;
    std::int32_t wwx, wwy, www, wwh;
    std::int32_t temp;

    temp = engine.ClipFlag;
    engine.ClipFlag = 0;

    wwx = WorkWindowRect.Left;
    wwy = WorkWindowRect.Top;
    www = WorkWindowRect.Width;
    wwh = WorkWindowRect.Height;

    mouse_point.X = MouseX;
    mouse_point.Y = MouseY;

    local_point = mouse_point;
    Parent->GlobalToLocal(&local_point);
    if (is_point_in_box(local_point.X, local_point.Y, 0, 0, Parent->ContentWidth() - 1, Parent->ContentHeight() / 2)) {
        SetWorkWindowBounds(Parent->ContentLeft() + 1, Parent->ContentTop() + 1, Parent->ContentWidth() - 1, Parent->ContentHeight() / 2 - 3);
        set_camera_plan();
        calc_world_pos_plan(local_point.X, local_point.Y);
        if (flag)
            engine.MousePosY = engine.Y >> 8;
        SetWorkWindowBounds(wwx, wwy, www, wwh); // RESTORE CLIP RECT
        engine.ClipFlag = temp;
        return (1);
    } else {
        engine.ClipFlag = temp;
        return (0);
    }
}

void free_walls(std::int32_t wall) {
    std::int32_t next;
    while (wall) {
        next = wall_list[wall].Next;
        wall_list[wall].Next = 0;
        wall_list[wall].WallFlags = 0;
        wall = next;
    }
}

//
// Returns the bounding box of the storey- The coordinates returned
// are the mapsquares not the bottom left of them.
//

void get_storey_bbox(
    std::int32_t storey,
    std::int32_t *x1,
    std::int32_t *z1,
    std::int32_t *x2,
    std::int32_t *z2) {
    std::int32_t next;
    std::int32_t wall;

    *x1 = storey_list[storey].DX;
    *z1 = storey_list[storey].DZ;
    *x2 = storey_list[storey].DX;
    *z2 = storey_list[storey].DZ;

    for (wall = storey_list[storey].WallHead; wall; wall = next) {
        next = wall_list[wall].Next;

        if ((wall_list[wall].DX >> 8) < *x1) {
            *x1 = wall_list[wall].DX;
        }
        if ((wall_list[wall].DZ >> 8) < *z1) {
            *z1 = wall_list[wall].DZ;
        }
        if ((wall_list[wall].DX >> 8) > *x2) {
            *x2 = wall_list[wall].DX;
        }
        if ((wall_list[wall].DZ >> 8) > *z2) {
            *z2 = wall_list[wall].DZ;
        }
    }

    *x2 -= 1;
    *z2 -= 1;
}

// the thing that points to this storey needs to remove its own link
void delete_storey_list(std::int16_t storey) {
    while (storey) {
        if (storey_list[storey].WallHead) {
            free_walls(storey_list[storey].WallHead);
            storey_list[storey].WallHead = 0;
        }
        /*
                        if(storey_list[storey].Roof)
                        {
                                delete_storey_list(storey_list[storey].Roof);
                                storey_list[storey].Roof=0;
                        }
                        */

        storey_list[storey].StoreyFlags = 0;
        storey = storey_list[storey].Next;
    }
}

void delete_building(std::uint16_t building) {
    std::int32_t storey, wall;

    storey = building_list[building].StoreyHead;
    if (storey)
        delete_storey_list(storey);

    building_list[building].StoreyHead = 0;
    building_list[building].BuildingFlags = 0;
}

//
// Returns the OUTLINE_Outline of the given storey.  Returns nullptr if
// the storey is not circular.  Non-circular storeys are not defined.
//

OUTLINE_Outline *get_storey_outline(std::int32_t storey) {
    OUTLINE_Outline *oo;

    std::int32_t wall;
    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;

    if (!is_storey_circular(storey)) {
        //
        // Not circular stories don't have outlines.
        //

        return nullptr;
    }

    oo = OUTLINE_create(128);

    x1 = storey_list[storey].DX >> 8;
    z1 = storey_list[storey].DZ >> 8;

    wall = storey_list[storey].WallHead;

    while (wall) {
        x2 = wall_list[wall].DX >> 8;
        z2 = wall_list[wall].DZ >> 8;

        OUTLINE_add_line(oo, x1, z1, x2, z2);

        x1 = x2;
        z1 = z2;

        wall = wall_list[wall].Next;
    }

    return oo;
}

std::int32_t do_storeys_overlap(std::int32_t s1, std::int32_t s2) {
    OUTLINE_Outline *oos;
    OUTLINE_Outline *ool;
    oos = get_storey_outline(s1);
    if (oos == nullptr)
        return (0);
    ool = get_storey_outline(s2);
    if (ool == nullptr)
        return (0);

    if (OUTLINE_overlap(oos, ool)) {
        return (1);
    } else {
        return (0);
    }
}

std::int32_t set_storey_height(std::int32_t building, std::int32_t storey, std::int32_t height) {
    std::int32_t link;
    std::int32_t y, offset_dy;

    OUTLINE_Outline *oos;
    OUTLINE_Outline *ool;

    if (storey == 0 || building == 0) {
        return (0);
    }

    //
    // Find the outline of this storey.
    //

    oos = get_storey_outline(storey);

    if (oos) {
        offset_dy = height - storey_list[storey].Height;
        y = storey_list[storey].DY;
        link = building_list[building].StoreyHead;

        while (link) {
            if (storey_list[link].DY > y) {
                switch (storey_list[link].StoreyType) {
                default:
                case STOREY_TYPE_NORMAL:

                    ool = get_storey_outline(link);

                    if (!ool) {
                    } else {
                        //
                        // If this storey overlaps the one whose height we have changed, then
                        // update the height of this storey.
                        //

                        if (OUTLINE_overlap(oos, ool)) {
                            storey_list[link].DY += offset_dy;
                            ASSERT(storey_list[link].DY >= 0);
                        }

                        OUTLINE_free(ool);
                    }
                    break;

                    break;

                case STOREY_TYPE_FENCE:

                    //
                    // Does this storey overlap the storey whose fence has changed.
                    //

                    {
                        std::int32_t x1;
                        std::int32_t z1;
                        std::int32_t x2;
                        std::int32_t z2;
                        std::int32_t wall;

                        wall = storey_list[link].WallHead;

                        x1 = storey_list[link].DX >> 8;
                        z1 = storey_list[link].DZ >> 8;

                        while (wall) {
                            x2 = wall_list[wall].DX >> 8;
                            z2 = wall_list[wall].DZ >> 8;

                            if (OUTLINE_intersects(
                                    oos,
                                    x1, z1,
                                    x2, z2)) {
                                //
                                // The fence overlaps the storey whose height has changed.
                                //

                                storey_list[link].DY += offset_dy;
                                ASSERT(storey_list[link].DY >= 0);

                                break;
                            }

                            x1 = x2;
                            z1 = z2;

                            wall = wall_list[wall].Next;
                        }
                    }

                    break;
                }
            }

            link = storey_list[link].Next;
        }

        storey_list[storey].Height = height;

        OUTLINE_free(oos);
        return (1);
    }
    return (0);
}

void load_textures_from_map(char *name) {
    std::uint16_t temp_end_prim_point;
    std::uint16_t temp_end_prim_face4;
    std::uint16_t temp_end_prim_face3;
    std::uint16_t temp_end_prim_object;

    std::uint16_t no_prim_point;
    std::uint16_t no_prim_face4;
    std::uint16_t no_prim_face3;
    std::uint16_t no_prim_object;
    std::int32_t save_type = 1;
    std::uint16_t temp[4];
    std::int32_t c0;
    std::int32_t size = 0;
    struct TinyStrip {
        std::uint16_t MapThingIndex;
        //	std::uint16_t	Depth[EDIT_MAP_DEPTH];
        std::uint16_t ColVectHead;
        //	std::uint16_t	Dummy1;
        std::uint16_t Texture;
        std::int16_t Bright;
    } tinyfloor;

    MFFileHandle handle = FILE_OPEN_ERROR;
    handle = FileOpen(name);
    if (handle != FILE_OPEN_ERROR) {
        std::int32_t dx, dz;

        LogText(" load map %s \n", name);
        FileRead(handle, (std::uint8_t *) &save_type, 4);

        if (save_type <= 8) {
            for (dx = 0; dx < EDIT_MAP_WIDTH; dx++) {
                for (dz = 0; dz < EDIT_MAP_DEPTH; dz++) {
                    size += FileRead(handle, (std::uint8_t *) &tinyfloor, sizeof(struct TinyStrip));
                    edit_map[dx][dz].MapThingIndex = tinyfloor.MapThingIndex;
                    edit_map[dx][dz].ColVectHead = tinyfloor.ColVectHead;
                    edit_map[dx][dz].Texture = tinyfloor.Texture;
                    edit_map[dx][dz].Bright = tinyfloor.Bright;
                }
            }
        } else {
            size += FileRead(handle, (std::uint8_t *) edit_map, sizeof(struct DepthStrip) * EDIT_MAP_WIDTH * EDIT_MAP_DEPTH);
        }
        FileClose(handle);
    }
}

void offset_buildings(std::int32_t x, std::int32_t y, std::int32_t z);

void BuildTab::HandleControl(std::uint16_t control_id) {
    switch (control_id & 0xff) {
    case CTRL_BUILD_TEXTURE:
        ToggleControlSelectedState(CTRL_BUILD_TEXTURE);
        if (Texture)
            Texture = 0;
        else
            Texture = 2;
        break;
    case CTRL_BUILD_Y_AXIS_FREE:
        ToggleControlSelectedState(CTRL_BUILD_Y_AXIS_FREE);
        if (Axis & Y_AXIS)
            Axis &= ~Y_AXIS;
        else
            Axis |= Y_AXIS;
        break;
    case CTRL_BUILD_Z_AXIS_FREE:
        ToggleControlSelectedState(CTRL_BUILD_Z_AXIS_FREE);
        if (Axis & Z_AXIS)
            Axis &= ~Z_AXIS;
        else
            Axis |= Z_AXIS;
        break;
    case CTRL_BUILD_CREATE_BUILDING:
        std::int32_t y;
        create_building_prim(EditBuilding, &y);
        break;
    case CTRL_BUILD_NEW_BUILDING:
        if (Mode == BUILD_MODE_WAIT || Mode == 0) {
            std::uint16_t building, storey;
            building = get_new_building();
            if (building) {
                EditBuilding = building;
                building_list[building].BuildingFlags |= 1;
                storey = get_new_storey();
                storey_list[storey].BuildingHead = EditBuilding;
                storey_list[storey].StoreyFlags = 1;
                storey_list[storey].DY = 0;
                storey_list[storey].Height = BLOCK_SIZE * 4;
                storey_list[storey].StoreyType = STOREY_TYPE_NORMAL;
                storey_list[storey].Prev = 0;
                storey_list[storey].ExtraFlags = 0;
                //					EditY=storey_list[storey].DY;
                if (storey) {
                    building_list[building].StoreyHead = storey;
                    building_list[building].StoreyCount = 1;
                    EditStorey = storey;
                }
                EditWall = 0;
                /*
                                                        wall=get_new_wall();
                                                        if(wall)
                                                        {
                                                                storey_list[storey].WallHead=wall;
                                                                EditWall=wall;
                                                        }
                */
            }
            Mode = BUILD_MODE_PLACE_STOREY;
        }
        break;
    case CTRL_NEW_FENCE:
        if (Mode == BUILD_MODE_WAIT || Mode == 0) {
            std::uint16_t building, storey;
            building = get_new_building();
            if (building) {
                EditBuilding = building;
                building_list[building].BuildingFlags |= 1;
                storey = get_new_storey();
                storey_list[storey].BuildingHead = EditBuilding;
                storey_list[storey].StoreyFlags = 1;
                storey_list[storey].DY = 0;
                storey_list[storey].Height = BLOCK_SIZE * 4;
                storey_list[storey].StoreyType = STOREY_TYPE_FENCE;
                storey_list[storey].Prev = 0;
                storey_list[storey].ExtraFlags = 0;
                //					EditY=storey_list[storey].DY;
                if (storey) {
                    building_list[building].StoreyHead = storey;
                    building_list[building].StoreyCount = 1;
                    EditStorey = storey;
                }
                EditWall = 0;
            }
            Mode = BUILD_MODE_PLACE_STOREY;
        }
        break;
    case CTRL_DELETE_STOREY:
        if (Mode == BUILD_MODE_WAIT)
            if (EditStorey) {
                //
                // Before we delete this storey, we set its height to zero so
                // that all the storeys above it move down.
                //

                set_storey_height(EditBuilding, EditStorey, 0);

                std::int32_t storey, prev, index;
                storey = EditStorey;
                storey_list[storey].StoreyFlags = 0;
                EditStorey = storey_list[storey].Next;
                //					EditY=storey_list[EditStorey].DY;
                if (storey_list[storey].WallHead)
                    free_walls(storey_list[storey].WallHead);
                /*
                                                        if(storey_list[storey].Roof)
                                                        {
                                                                delete_storey_list(storey_list[storey].Roof);
                                                                storey_list[storey].Roof=0;
                                                        }
                */
                EditWall = 0;
                LogText(" delete storey %d next %d prev %d \n", storey, EditStorey, storey_list[storey].Prev);
                prev = storey_list[storey].Prev;
                if (prev && (storey_list[prev].Next == storey)) {
                    storey_list[prev].Next = EditStorey;
                    if (EditStorey == 0) {
                        LogText(" next==0, prev=%d \n", prev);
                        EditStorey = prev;
                        //							EditY=storey_list[EditStorey].DY;
                        RequestUpdate();
                        return;
                    } else {
                        LogText("p2 next==%d, prev=%d \n", EditStorey, prev);
                        storey_list[EditStorey].Prev = prev;
                    }

                } else {
                    LogText("parent is building %d next= %d \n", EditBuilding, EditStorey);
                    building_list[EditBuilding].StoreyHead = EditStorey;
                    building_list[EditBuilding].StoreyCount--;
                    storey_list[EditStorey].Prev = 0;
                }

                /*

                switch(storey_list[storey].StoreyType)
                {
                        case	STOREY_TYPE_FIRE_ESCAPE:
                        case	STOREY_TYPE_LADDER:
                        case	STOREY_TYPE_ROOF_QUAD:
                        case	STOREY_TYPE_CABLE:
                        case	STOREY_TYPE_TRENCH:
                                break;
                        default:

                                index=EditStorey;
                                while(index)
                                {
                                        storey_list[index].DY-=storey_list[storey].Height;
//								if(storey_list[index].Roof)
//									storey_list[storey_list[index].Roof].DY-=BLOCK_SIZE*4;

                                        index=storey_list[index].Next;
                                }
                }

                */

                RequestUpdate();
            }
        break;
    case CTRL_SET_STOREY_HEIGHT_64:
        set_storey_height(EditBuilding, EditStorey, 64);
        break;
    case CTRL_SET_STOREY_HEIGHT_128:
        set_storey_height(EditBuilding, EditStorey, 128);
        break;
    case CTRL_SET_STOREY_HEIGHT_196:
        set_storey_height(EditBuilding, EditStorey, 192);
        break;
    case CTRL_SET_STOREY_HEIGHT_256:
        set_storey_height(EditBuilding, EditStorey, 256);
        break;

    case CTRL_ADD_TRENCH:
    case CTRL_BUILD_NEXT_STOREY:

        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                if (storey_list[EditStorey].StoreyType != STOREY_TYPE_NORMAL && storey_list[EditStorey].StoreyType != STOREY_TYPE_FENCE) {
                    Alert *quit_alert;

                    quit_alert = new Alert;
                    quit_alert->HandleAlert("Can't add next storey to current floor (roof/firescape?) ", nullptr);
                    delete quit_alert;
                    //`						RequestUpdate();
                } else {
                    std::uint16_t storey;
                    storey = get_new_storey();
                    if (storey) {
                        storey_list[storey].BuildingHead = EditBuilding;
                        storey_list[storey].StoreyFlags = 1;
                        storey_list[storey].Prev = EditStorey;
                        switch (control_id & 0xff) {
                        case CTRL_BUILD_NEXT_STOREY:

                            storey_list[storey].StoreyType = STOREY_TYPE_NORMAL;
                            storey_list[storey].DY = storey_list[EditStorey].DY + storey_list[EditStorey].Height;
                            //									EditY=storey_list[storey].DY;
                            break;

                        case CTRL_ADD_TRENCH:
                            storey_list[storey].StoreyType = STOREY_TYPE_TRENCH;
                            storey_list[storey].DY = 0; // storey_list[EditStorey].DY+BLOCK_SIZE*4; //BLOCK_SIZE*5;
                            //									EditY=storey_list[storey].DY;
                            break;
                        }
                        if (building_list[EditBuilding].StoreyHead) {
                            if (storey_list[EditStorey].Next) {
                                std::int32_t next;
                                next = storey_list[EditStorey].Next;
                                storey_list[storey].Next = next;
                                storey_list[next].Prev = storey;
                            }
                            storey_list[EditStorey].Next = storey;
                            building_list[EditBuilding].StoreyCount++;
                        } else {
                            building_list[EditBuilding].StoreyHead = storey;
                            building_list[EditBuilding].StoreyCount = 1;
                        }
                        EditStorey = storey;
                        storey_list[EditStorey].WallHead = 0;
                        //							storey_list[EditStorey].Roof=0;
                        storey_list[EditStorey].Height = BLOCK_SIZE * 4;
                        EditWall = 0;
                        Mode = BUILD_MODE_PLACE_STOREY;
                    }
                }
                RequestUpdate();
            }
        break;
    case CTRL_LOAD_TEXTURES: {
        FileRequester *fr;
        char fname[100];
        fr = new FileRequester("data\\", "*.map", "Load A MAP", "temp.map");
        if (fr->Draw()) {
            strcpy(fname, fr->Path);
            strcat(fname, fr->FileName);
            strcpy(edit_info.MapName, fr->FileName);
            load_textures_from_map(fname);
            //					the_leveleditor->BuildMode->ResetBuildTab();
        }

        delete fr;
    }
        RequestUpdate();

        break;

    case CTRL_CUT_BUILDING:
        if (EditBuilding) {
            building_block.Cut(EditBuilding);
        }
        break;
    case CTRL_SCROLL_MAP_UP:
        offset_buildings(0, 0, -256);
        RequestUpdate();

        break;
    case CTRL_SCROLL_MAP_DOWN:
        offset_buildings(0, 0, 256);
        RequestUpdate();
        break;
    case CTRL_SCROLL_MAP_LEFT:
        offset_buildings(-256, 0, 0);
        RequestUpdate();
        break;
    case CTRL_SCROLL_MAP_RIGHT:
        offset_buildings(256, 0, 0);
        RequestUpdate();
        break;
    case CTRL_PASTE_BUILDING:
        building_block.Paste(ViewX, ViewZ, 0);
        RequestUpdate();

        break;
    case CTRL_DEL_ALL: {
        std::int32_t res;
        Alert *quit_alert;

        quit_alert = new Alert;
        res = quit_alert->HandleAlert("Delete ALL BUILDINGS you crazy fucker? ", nullptr);
        delete quit_alert;

        if (res == 1)
            delete_all();
    }
        RequestUpdate();
        break;

    case CTRL_TOGGLE_FLAT_TILED_ROOF:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding && building_list[EditBuilding].StoreyHead) {
                if (EditStorey) {
                    storey_list[EditStorey].StoreyFlags &= ~(FLAG_STOREY_TILED_ROOF);

                    if (storey_list[EditStorey].StoreyFlags & FLAG_STOREY_FLAT_TILED_ROOF)
                        storey_list[EditStorey].StoreyFlags &= ~(FLAG_STOREY_FLAT_TILED_ROOF);
                    else
                        storey_list[EditStorey].StoreyFlags |= (FLAG_STOREY_FLAT_TILED_ROOF);
                    RequestUpdate();
                }
            }
        break;
    case CTRL_TOGGLE_TILED_ROOF:

        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding && building_list[EditBuilding].StoreyHead) {
                if (EditStorey) {
                    storey_list[EditStorey].StoreyFlags &= ~(FLAG_STOREY_FLAT_TILED_ROOF);

                    if (storey_list[EditStorey].StoreyFlags & FLAG_STOREY_TILED_ROOF)
                        storey_list[EditStorey].StoreyFlags &= ~(FLAG_STOREY_TILED_ROOF);
                    else
                        storey_list[EditStorey].StoreyFlags |= (FLAG_STOREY_TILED_ROOF);
                    RequestUpdate();
                }
            }
        break;
    case CTRL_BUILD_DUPLICATE_STOREY:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding && EditStorey) {
                if (storey_list[EditStorey].StoreyType != STOREY_TYPE_NORMAL) {
                    Alert *quit_alert;
                    quit_alert = new Alert;
                    quit_alert->HandleAlert("Can't Duplicate current storey (roof/firescape?) ", nullptr);
                    delete quit_alert;
                    RequestUpdate();
                    //					update_modules();
                } else {
                    std::int16_t storey, wall, index, prev = 0;
                    std::int32_t storey_height;

                    storey_height = storey_list[EditStorey].Height;
                    storey = get_new_storey();

                    if (storey) {
                        //
                        // first make a gap for the storey with a bodge call to set storey height (pretend current storey is twice as high)
                        //
                        if (set_storey_height(EditBuilding, EditStorey, storey_list[EditStorey].Height << 1))
                            storey_list[EditStorey].Height >>= 1; // then correct the height change

                        LogText(" duplicate storey %d to storey %d prev %d\n", EditStorey, storey, prev);
                        // storey_list[storey].BuildingHead=EditBuilding;
                        // storey_list[storey].StoreyFlags=1;
                        storey_list[storey] = storey_list[EditStorey];
                        if (storey_list[storey].InsideStorey) {
                            storey_list[storey].InsideStorey = duplicate_storey_list(storey_list[storey].InsideStorey, EditBuilding);
                            storey_list[storey_list[storey].InsideStorey].DY = storey_list[EditStorey].DY + storey_height;
                        }
                        if (storey_list[storey].InsideIDIndex) {
                            storey_list[storey].InsideIDIndex = copy_insides(storey_list[storey].InsideIDIndex);
                        }

                        storey_list[storey].Prev = EditStorey;
                        storey_list[storey].DY = storey_list[EditStorey].DY + storey_height;
                        //						EditY=storey_list[storey].DY;
                        index = storey_list[EditStorey].WallHead;
                        while (index) {
                            wall = get_new_wall();
                            LogText(" copy index %d to wall %d prev %d \n", index, wall, prev);
                            wall_list[wall] = wall_list[index];
                            wall_list[wall].StoreyHead = storey;
                            if (prev)
                                wall_list[prev].Next = wall;
                            else {
                                storey_list[storey].WallHead = wall;
                                LogText(" set storey %d wallhead to %d \n", storey, wall);
                            }
                            prev = wall;
                            index = wall_list[index].Next;
                        }
                        LogText(" original wallhead %d copy wallhead=%d \n", storey_list[EditStorey].WallHead, storey_list[storey].WallHead);
                        if (storey_list[EditStorey].Next) {
                            std::int32_t next;
                            next = storey_list[EditStorey].Next;
                            storey_list[storey].Next = next;
                            storey_list[next].Prev = storey;
                        } else
                            storey_list[storey].Next = 0;

                        storey_list[EditStorey].Next = storey;
                        building_list[EditBuilding].StoreyCount++;
                        //						storey_list[storey].Roof=0;
                        EditStorey = storey;

                        /*
                                                                        storey=building_list[EditBuilding].StoreyHead;
                                                                        while(storey)
                                                                        {

                                                                                switch(storey_list[storey].StoreyType)
                                                                                {
                                                                                        case	STOREY_TYPE_NORMAL:
                                                                                        case	STOREY_TYPE_LADDER:
                                                                                        case	STOREY_TYPE_FENCE:
                                                                                                if(storey_list[storey].DY>=storey_list[EditStorey].DY)
                                                                                                {

                                                                                                        storey_list[storey].DY+=storey_height;
                                                                                                }
                                                                                                break;
                                                                                }
                                                                                storey=storey_list[storey].Next;
                                                                        }
                        */

                        EditWall = 0;
                        RequestUpdate();
                    }
                }
            }
        break;
    case CTRL_ADD_FIRE_ESCAPE:

        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                std::int32_t storey;
                storey = get_new_storey();
                if (storey) {
                    storey_list[storey].BuildingHead = EditBuilding;
                    storey_list[storey].StoreyFlags = 1;
                    storey_list[storey].Prev = EditStorey;
                    storey_list[storey].StoreyType = STOREY_TYPE_FIRE_ESCAPE;
                    storey_list[storey].DY = 0; // storey_list[EditStorey].DY-BLOCK_SIZE*5;
                    //						EditY=storey_list[storey].DY;
                    if (building_list[EditBuilding].StoreyHead) {
                        storey_list[storey].Next = building_list[EditBuilding].StoreyHead;
                        storey_list[storey_list[storey].Next].Prev = storey;
                        //							storey_list[EditStorey].Next=storey;
                        building_list[EditBuilding].StoreyHead = storey;
                    } else {
                        building_list[EditBuilding].StoreyHead = storey;
                    }
                    EditStorey = storey;
                    storey_list[EditStorey].WallHead = 0;
                    //						storey_list[EditStorey].Roof=0;
                    storey_list[EditStorey].Height = 4;
                    EditWall = 0;
                    Mode = BUILD_MODE_PLACE_STOREY;
                    RequestUpdate();
                }
            }
        break;
    case CTRL_ADD_POKEY:
        if (Mode == BUILD_MODE_WAIT)
            if (EditStorey && storey_list[EditStorey].StoreyType == STOREY_TYPE_NORMAL) {
                storey_list[EditStorey].StoreyFlags |= FLAG_STOREY_LEDGE;
            }

        break;
    case CTRL_ADD_LADDER:

        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                std::int32_t storey;
                storey = get_new_storey();
                if (storey && EditStorey && EditBuilding) {
                    storey_list[storey].BuildingHead = EditBuilding;
                    storey_list[storey].StoreyFlags = 1;
                    storey_list[storey].Prev = EditStorey;
                    storey_list[storey].StoreyType = STOREY_TYPE_LADDER;
                    if (EditStorey)
                        storey_list[storey].DY = storey_list[EditStorey].DY;
                    else
                        storey_list[storey].DY = 0;
                    //						EditY=storey_list[storey].DY;
                    if (building_list[EditBuilding].StoreyHead) {
                        if (storey_list[EditStorey].Next) {
                            std::int32_t next;
                            next = storey_list[EditStorey].Next;
                            storey_list[storey].Next = next;
                            storey_list[next].Prev = storey;
                        }
                        storey_list[EditStorey].Next = storey;
                        building_list[EditBuilding].StoreyCount++;

                        //							storey_list[storey].Next=building_list[EditBuilding].StoreyHead;
                        //							storey_list[storey_list[storey].Next].Prev=storey;
                        //							storey_list[EditStorey].Next=storey;
                        //							building_list[EditBuilding].StoreyHead=storey;
                    } else {
                        building_list[EditBuilding].StoreyHead = storey;
                        building_list[EditBuilding].StoreyCount = 0;
                    }
                    EditStorey = storey;
                    storey_list[EditStorey].WallHead = 0;
                    //						storey_list[EditStorey].Roof=0;
                    storey_list[EditStorey].Height = 4;
                    EditWall = 0;
                    Mode = BUILD_MODE_PLACE_STOREY;
                    RequestUpdate();
                }
            }
        break;

    case CTRL_ADD_STAIRCASE:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                std::int32_t storey;
                storey = get_new_storey();
                if (storey) {
                    storey_list[storey].BuildingHead = EditBuilding;
                    storey_list[storey].StoreyFlags = 1;
                    storey_list[storey].Prev = EditStorey;
                    storey_list[storey].StoreyType = STOREY_TYPE_STAIRCASE;
                    storey_list[storey].DY = 0; // storey_list[EditStorey].DY-BLOCK_SIZE*5;
                    //						EditY=storey_list[storey].DY;
                    if (building_list[EditBuilding].StoreyHead) {
                        storey_list[storey].Next = building_list[EditBuilding].StoreyHead;
                        storey_list[storey_list[storey].Next].Prev = storey;
                        //							storey_list[EditStorey].Next=storey;
                        building_list[EditBuilding].StoreyHead = storey;
                    } else {
                        building_list[EditBuilding].StoreyHead = storey;
                    }
                    EditStorey = storey;
                    storey_list[EditStorey].WallHead = 0;
                    //						storey_list[EditStorey].Roof=0;
                    storey_list[EditStorey].Height = 4;
                    storey_list[EditStorey].Info1 = 0;
                    EditWall = 0;
                    Mode = BUILD_MODE_PLACE_STOREY;
                    RequestUpdate();
                }
            }
        break;
    case CTRL_ADD_CABLE:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                std::uint16_t storey;
                storey = get_new_storey();
                if (storey) {
                    storey_list[storey].BuildingHead = EditBuilding;
                    storey_list[storey].StoreyFlags = 1;
                    //						storey_list[storey].Prev=EditStorey;
                    storey_list[storey].StoreyType = STOREY_TYPE_CABLE;
                    storey_list[storey].DY = storey_list[EditStorey].DY + BLOCK_SIZE * 4;
                    //						EditY=storey_list[storey].DY;
                    if (building_list[EditBuilding].StoreyHead) {
                        storey_list[storey].Next = building_list[EditBuilding].StoreyHead;
                        storey_list[storey_list[storey].Next].Prev = storey;
                        //							storey_list[EditStorey].Next=storey;
                        building_list[EditBuilding].StoreyHead = storey;
                    } else {
                        building_list[EditBuilding].StoreyHead = storey;
                    }
                    EditStorey = storey;
                    storey_list[EditStorey].WallHead = 0;
                    //						storey_list[EditStorey].Roof=0;
                    storey_list[EditStorey].Height = 4;
                    storey_list[EditStorey].Info1 = 0;
                    EditWall = 0;
                    Mode = BUILD_MODE_PLACE_STOREY;
                    RequestUpdate();
                }
            }
        break;
    case CTRL_ADD_SKYLIGHT:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding) {
                if (storey_list[EditStorey].StoreyType != STOREY_TYPE_NORMAL) {
                    Alert *quit_alert;

                    quit_alert = new Alert;
                    quit_alert->HandleAlert("Can't add SKYLIGHT storey to current floor (roof/firescape?) ", nullptr);
                    delete quit_alert;
                    //`						RequestUpdate();
                } else {
                    std::uint16_t storey;
                    storey = get_new_storey();
                    if (storey) {
                        storey_list[storey].BuildingHead = EditBuilding;
                        storey_list[storey].StoreyFlags = 1;
                        storey_list[storey].Prev = EditStorey;
                        storey_list[storey].StoreyType = STOREY_TYPE_SKYLIGHT;
                        storey_list[storey].DY = storey_list[EditStorey].DY + BLOCK_SIZE * 4; // BLOCK_SIZE*5;
                        //							EditY=storey_list[storey].DY;
                        if (building_list[EditBuilding].StoreyHead) {
                            if (storey_list[EditStorey].Next) {
                                std::int32_t next;
                                next = storey_list[EditStorey].Next;
                                storey_list[storey].Next = next;
                                storey_list[next].Prev = storey;
                            }
                            storey_list[EditStorey].Next = storey;
                            building_list[EditBuilding].StoreyCount++;
                        } else {
                            building_list[EditBuilding].StoreyHead = storey;
                            building_list[EditBuilding].StoreyCount = 1;
                        }
                        EditStorey = storey;
                        storey_list[EditStorey].WallHead = 0;
                        //							storey_list[EditStorey].Roof=0;
                        storey_list[EditStorey].Height = BLOCK_SIZE * 4;
                        EditWall = 0;
                        Mode = BUILD_MODE_PLACE_STOREY;
                    }
                }
                RequestUpdate();
            }
        break;

    case CTRL_ADD_FLAT_ROOF_QUAD:
        if (Mode == BUILD_MODE_WAIT)
            if (EditBuilding && building_list[EditBuilding].StoreyHead) {
                std::uint16_t storey;
                storey = get_new_storey();
                if (storey) {
                    storey_list[storey].BuildingHead = EditBuilding;
                    storey_list[storey].StoreyFlags = 1;
                    storey_list[storey].Prev = EditStorey;
                    storey_list[storey].StoreyType = STOREY_TYPE_ROOF_QUAD;
                    storey_list[storey].DY = storey_list[EditStorey].DY + 4 * BLOCK_SIZE + 2;
                    //						EditY=storey_list[storey].DY;

                    //						storey_list[EditStorey].Roof=storey;
                    //						building_list[EditBuilding].StoreyCount++;

                    EditStorey = storey;
                    storey_list[EditStorey].WallHead = 0;
                    storey_list[EditStorey].Height = 0;
                    EditWall = 0;
                    Mode = BUILD_MODE_PLACE_STOREY;
                    RequestUpdate();
                }
            }
        break;
    case CTRL_DELETE_BUILDING:
        if (Mode == BUILD_MODE_WAIT) {
            if (EditBuilding)
                delete_building(EditBuilding);
            EditBuilding = 0;
            EditStorey = 0;
            EditWall = 0;
            EditY = 0;
            RequestUpdate();
        }
        break;

    case CTRL_ANOTHER_INSIDE_SEED:

        if (WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
            ASSERT(WITHIN(seed_inside_upto, 0, MAX_SEED_BACKUPS - 1));

            seed_inside[seed_inside_upto] = building_list[EditBuilding].InsideSeed;

            seed_inside_upto += 1;
            seed_inside_upto &= MAX_SEED_BACKUPS - 1;

            building_list[EditBuilding].InsideSeed = rand();

            RequestUpdate();
        }

        break;

    case CTRL_PREV_INSIDE_SEED:

        if (WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
            seed_inside_upto -= 1;
            seed_inside_upto &= MAX_SEED_BACKUPS - 1;

            ASSERT(WITHIN(seed_inside_upto, 0, MAX_SEED_BACKUPS - 1));

            building_list[EditBuilding].InsideSeed = seed_inside[seed_inside_upto];

            RequestUpdate();
        }

        break;

    case CTRL_ANOTHER_STAIRCASE_SEED:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            ASSERT(WITHIN(seed_stairs_upto, 0, MAX_SEED_BACKUPS - 1));

            seed_stairs[seed_stairs_upto] = building_list[EditBuilding].StairSeed;

            seed_stairs_upto += 1;
            seed_stairs_upto &= MAX_SEED_BACKUPS - 1;

            building_list[EditBuilding].StairSeed = rand();

            RequestUpdate();
        }

        break;

    case CTRL_PREV_STAIRCASE_SEED:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            seed_stairs_upto -= 1;
            seed_stairs_upto &= MAX_SEED_BACKUPS - 1;

            ASSERT(WITHIN(seed_stairs_upto, 0, MAX_SEED_BACKUPS - 1));

            building_list[EditBuilding].StairSeed = seed_stairs[seed_stairs_upto];

            RequestUpdate();
        }

        break;

    case CTRL_NEXT_STOREY:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1) &&
            WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
            std::int32_t i_storey;

            i_storey = storey_list[EditStorey].Next;

            if (WITHIN(i_storey, 1, MAX_STOREYS - 1)) {
                EditStorey = i_storey;
                //					EditY      = storey_list[i_storey].DY;
                RequestUpdate();
            }
        }

        break;

    case CTRL_PREV_STOREY:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1) &&
            WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
            std::int32_t i_storey;

            i_storey = storey_list[EditStorey].Prev;

            if (WITHIN(i_storey, 1, MAX_STOREYS - 1)) {
                EditStorey = i_storey;
                //					EditY      = storey_list[i_storey].DY;
                RequestUpdate();
            }
        }

        break;

    case CTRL_BUILDING_TYPE_HOUSE:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_HOUSE;
            RequestUpdate();
        }

        break;

    case CTRL_BUILDING_TYPE_WAREHOUSE:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_WAREHOUSE;
            RequestUpdate();
        }

        break;

    case CTRL_BUILDING_TYPE_OFFICE:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_OFFICE;
            RequestUpdate();
        }

        break;

    case CTRL_BUILDING_TYPE_APARTEMENT:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_APARTEMENT;
            RequestUpdate();
        }

        break;

    case CTRL_BUILDING_TYPE_CRATE_IN:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_CRATE_IN;
            RequestUpdate();
        }

        break;

    case CTRL_BUILDING_TYPE_CRATE_OUT:

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            building_list[EditBuilding].BuildingType = BUILDING_TYPE_CRATE_OUT;
            RequestUpdate();
        }

        break;

    case CTRL_SET_STOREY_TYPE_NORMAL:

        //
        // Make the current storey a fence.
        //

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            if (WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
                storey_list[EditStorey].StoreyType = STOREY_TYPE_NORMAL;
                storey_list[EditStorey].ExtraFlags &= ~FLAG_STOREY_EXTRA_ONBUILDING;
            }
        }

        break;

    case CTRL_SET_STOREY_TYPE_FENCE:

        //
        // Make the current storey a fence.
        //

        if (WITHIN(EditBuilding, 1, MAX_BUILDINGS - 1)) {
            if (WITHIN(EditStorey, 1, MAX_STOREYS - 1)) {
                storey_list[EditStorey].StoreyType = STOREY_TYPE_FENCE_FLAT;
                storey_list[EditStorey].ExtraFlags |= FLAG_STOREY_EXTRA_ONBUILDING;
            }
        }

        break;

        /*
                        case	CTRL_DEFINE_FLOOR_POINTS:
                                if(FloorHead)
                                {
                                        std::int32_t	index;
                                        index=storey_list[FloorHead].WallHead;
                                        while(index)
                                        {
                                                EditWall=index;
                                                index=wall_list[index].Next;
                                        }
                                        EditStorey=FloorHead;

                                        Mode=BUILD_MODE_CONT_STOREY;
                                }
                                else
                                {
                                                std::int32_t	storey,wall;
                                                storey=get_new_storey();
                                                if(storey)
                                                {
                                                        FloorHead=storey;
                                                        storey_list[storey].StoreyFlags=1;
                                                        storey_list[storey].Prev=0;
                                                        storey_list[storey].StoreyType=STOREY_TYPE_FLOOR_POINTS;
                                                        storey_list[storey].DY=0;
                                                        EditY=0;

                                                        EditStorey=storey;
                                                        storey_list[EditStorey].WallHead=0;
                                                        storey_list[EditStorey].Roof=0;
                                                        EditWall=0;
                                                        Mode=BUILD_MODE_PLACE_STOREY;
                                                        RequestUpdate();
                                                }
                                }
                                break;
        */
    }
}
