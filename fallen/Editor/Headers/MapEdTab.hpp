// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef _MAPEDTAB_HPP_
#define _MAPEDTAB_HPP_

#include "ModeTab.hpp"
#include "Stealth.h"
#include "EditMod.hpp"
#include "undo.hpp"

#define FLOOR_CUT_BRUSH 1
#define FLOOR_PASTE_BRUSH 2
#define FLOOR_HOLD_BRUSH 3
#define FLOOR_CUT_BRUSH_DEF 4

class MapEdTab : public ModeTab {
   private:
    std::int32_t Axis;
    std::int32_t GridFlag;
    std::uint8_t AxisMode;
    EdRect View1;
    EdRect View2;
    EdRect View3;
    std::uint8_t RedrawTabContent;
    std::uint16_t CurrentCol;
    std::int32_t X1, Y1, Z1, X2, Y2, Z2;

   public:
    MapEdTab(EditorModule *parent);
    ~MapEdTab();
    void DrawTabContent();
    void HandleTab(MFPoint *current_point);
    std::uint16_t HandleTabClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleControl(std::uint16_t control_id);
    void DrawModuleContent(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t HandleModuleContentClick(MFPoint *clicked_point, std::uint8_t flags, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t SetWorldMouse(std::uint32_t flag);
    std::int32_t KeyboardInterface();
    std::int32_t DragEngine(std::uint8_t flags, MFPoint *clicked_point);
    std::int32_t CalcMapCoord(std::int32_t *mapx, std::int32_t *mapy, std::int32_t *mapz, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, MFPoint *clicked_point);
    std::int32_t DragPaint(std::uint8_t flags);
    std::int32_t DragMark(std::uint8_t flags);
    void CutFloorBrush(MFPoint *current_point, std::int32_t button);
    std::int32_t MouseInContent();
    void DragAltitude(std::int32_t mx, std::int32_t mz);
    std::int32_t FlattenArea();
    std::int32_t SmoothArea();
    std::int32_t SlopeArea();
    void ChangeMapAltitude(std::int32_t mx, std::int32_t mz, std::int32_t step, std::uint8_t offset_flag);
    //		Undo				MyUndo;
    std::uint8_t RedrawModuleContent;
    void Clear();
    std::uint16_t Mode;
    std::uint16_t SubMode;
    EditorModule *Parent;
    BuildTab *BuildMode;
    MapBlock CutMapBlock;
    std::int32_t RoofTop;
    std::int32_t Texture;
};

#endif
