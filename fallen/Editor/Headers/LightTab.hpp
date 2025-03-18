// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef _LIGHTTAB_HPP_
#define _LIGHTTAB_HPP_

#include "ModeTab.hpp"
#include "Stealth.h"
#include "EditMod.hpp"
#include "undo.hpp"

#define LIGHT_TAB_MODE_WAIT 0
#define LIGHT_TAB_MODE_PLACE_LIGHT 1
#define LIGHT_TAB_MODE_EDIT_LIGHT 2
#define LIGHT_TAB_MODE_DRAG_LIGHT 3
#define LIGHT_TAB_MODE_PLACE_AMBIENT 4
#define LIGHT_TAB_MODE_REPEAT_PLACE_LIGHT 5

class LightTab : public ModeTab {
   private:
    std::int32_t Axis;
    std::int32_t GridFlag;
    std::uint8_t AxisMode;
    EdRect View1;
    EdRect View2;
    EdRect View3;
    std::uint8_t RedrawTabContent;
    std::uint8_t Shadow;
    std::uint16_t CurrentLight;

   public:
    LightTab(EditorModule *parent);
    ~LightTab();
    void DrawTabContent();
    void HandleTab(MFPoint *current_point);
    std::uint16_t HandleTabClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleControl(std::uint16_t control_id);
    void DrawModuleContent(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t HandleModuleContentClick(MFPoint *clicked_point, std::uint8_t flags, std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h);
    std::int32_t SetWorldMouse(std::uint32_t flag);
    std::int32_t KeyboardInterface();
    std::int16_t CreateLightThing(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t bright);
    void DeleteLightThing(std::int16_t thing);
    std::int32_t DragALight(std::uint8_t flags, MFPoint *clicked_point, std::uint16_t copy);
    std::int32_t DragEngine(std::uint8_t flags, MFPoint *clicked_point);
    void SetAmbientAngle();
    void RecalcAllLights();
    void SmoothGroup();
    Undo MyUndo;
    std::uint8_t RedrawModuleContent;
    std::uint16_t Mode;
    std::int32_t ClickOnLight(MFPoint *clicked_point);
    EditorModule *Parent;
};

extern void add_a_background_thing(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);
extern void apply_light_to_map(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t bright);

#endif
