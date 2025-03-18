// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef _PRIMPICKTAB_HPP_
#define _PRIMPICKTAB_HPP_

#include "ModeTab.hpp"
#include "Stealth.h"
#include "EditMod.hpp"
#include "undo.hpp"

/*
#define	CTRL_PRIM_LOAD_BACKGROUND	1
#define	CTRL_PRIM_APPEND_NEW		2
#define	CTRL_PRIM_LOAD_MAP			3
#define	CTRL_PRIM_SAVE_MAP			4
#define	CTRL_PRIM_LORES_TEST		5
#define	CTRL_PRIM_X_AXIS_FREE		6
#define	CTRL_PRIM_Y_AXIS_FREE		7
#define	CTRL_PRIM_Z_AXIS_FREE		8
#define	CTRL_PRIM_GRID_ON			9
*/

#define X_AXIS (1 << 0)
#define Y_AXIS (1 << 1)
#define Z_AXIS (1 << 2)

#define PRIM_MODE_SINGLE 0
#define PRIM_MODE_MULTI 1
#define PRIM_MODE_BACK 2
#define PRIM_MODE_ANIM_KEY 3
#define PRIM_MODE_ANIM_MORPH 4

extern void record_prim_status();

extern void apply_user_rotates(struct PrimPoint *point);

class PrimPickTab : public ModeTab {
   private:
    std::int32_t ListPos;
    std::int32_t CurrentPrim;
    std::int32_t DragThingView1;
    std::int32_t DragThingView2;
    std::int32_t DragThingView3;
    std::int32_t Axis;
    std::int32_t GridFlag;
    std::int32_t GridMax;
    std::int32_t GridCorner;
    std::uint8_t AxisMode,
        PrimTabMode;
    EdRect PrimRect,
        View1,
        View2,
        View3;
    EditorModule *Parent;
    std::uint8_t RedrawTabContent;
    void DrawABuildingInRect(std::uint32_t prim, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    void DrawAPrimInRect(std::uint32_t prim, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    void DrawAMultiPrimInRect(std::uint32_t prim, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t PrimScale, BackScale;

   public:
    PrimPickTab(EditorModule *parent);
    ~PrimPickTab();
    void UpdatePrimInfo();
    void DrawPrims();
    void UpdatePrimPickWindow();
    void DrawTabContent();
    void HandleTab(MFPoint *current_point);
    std::uint16_t HandleTabClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleControl(std::uint16_t control_id);
    void DrawModuleContent(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t HandleModuleContentClick(MFPoint *clicked_point, std::uint8_t flags, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t HiLightObjects(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    inline std::int32_t GetCurrentPrim() { return CurrentPrim; }
    inline void SetCurrentPrim(std::int32_t prim) { CurrentPrim = prim; }
    std::int32_t SetWorldMouse(std::uint32_t flag);
    std::int32_t DragAPrim(std::uint8_t flags, MFPoint *clicked_point, std::int32_t button);
    std::int32_t DragEngine(std::uint8_t flags, MFPoint *clicked_point);
    std::int32_t KeyboardInterface();
    Undo MyUndo;
    std::uint8_t RedrawModuleContent;
    std::uint8_t View2Mode;

    inline std::uint8_t GetPrimTabMode() { return PrimTabMode; }
    inline void SetPrimTabMode(std::uint8_t mode) { PrimTabMode = mode; }
};

extern void add_a_background_thing(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);

#endif
