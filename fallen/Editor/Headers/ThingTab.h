// ThingTab.h
// Guy Simmons, 15th January 1998.

#ifndef THINGTAB_H
#define THINGTAB_H

#define THING_MODE_NONE 0
#define THING_MODE_SELECT_THING 1
#define THING_MODE_SELECT_SWITCH 2

//---------------------------------------------------------------

class ThingTab : public ModeTab {
   private:
    bool Update;
    std::uint8_t CurrentClass,
        CurrentGenus;
    std::uint16_t CurrentThing,
        TabMode;
    std::int32_t TabData,
        ThingFlags,
        *DataPtr;
    ControlSet CurrentSet;
    EdRect CurrentSetRect;

   public:
    ThingTab();
    ~ThingTab();

    void DrawTabContent();
    void UpdateTab(std::uint8_t update_level);
    std::uint16_t HandleTabClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleTab(MFPoint *current_point);
    void HandleControl(std::uint16_t control_id);
    void HandleClassControl(std::uint16_t control_id);
    void HandleBuildingControl(std::uint16_t control_id);
    EditComList *SelectCommandList();

    void DrawClassSet();
    void UpdateTabInfo();
    void UpdateClassInfo();
    void UpdateCheckBoxes();

    inline std::uint8_t GetCurrentClass() { return CurrentClass; }
    inline std::uint8_t GetCurrentGenus() { return CurrentGenus; }
    inline std::uint8_t GetThingFlags() { return ThingFlags; }
    inline void SetCurrentClass(std::uint8_t clss) {
        CurrentClass = clss;
        UpdateTabInfo();
    }
    inline void SetCurrentGenus(std::uint8_t genus) {
        CurrentGenus = genus;
        UpdateClassInfo();
    }

    inline std::uint16_t GetTabMode() { return TabMode; }
    inline void SetTabMode(std::uint16_t mode) { TabMode = mode; }
    inline void SetTabData(std::int32_t data) { TabData = data; }
    inline void SetCurrentThing(std::uint16_t thing) {
        CurrentThing = thing;
        UpdateClassInfo();
    }
};

//---------------------------------------------------------------

#endif
