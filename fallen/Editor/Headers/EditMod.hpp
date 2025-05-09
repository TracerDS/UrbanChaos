// EditMod.hpp
// Guy Simmons, 26th October 1996.

#ifndef _EDITMOD_HPP_
#define _EDITMOD_HPP_

#include "Window.hpp"

class EditorModule : public Window {
   private:
    bool EscapeFlag;
    EditorModule *LastModule,
        *NextModule;

   protected:
    std::uint32_t *ExternalUpdate;

    inline void RequestUpdate() {
        if (ExternalUpdate) *ExternalUpdate = 1;
    }

   public:
    EditorModule();
    ~EditorModule(){};
    virtual void SetupModule();

    void MoveModule(MFPoint *clicked_point);
    void SizeModule(MFPoint *clicked_point);
    virtual void HandleContentClick(std::uint8_t flags, MFPoint *clicked_point);
    virtual void HandleControlClick(std::uint8_t flags, MFPoint *clicked_point);
    virtual void HandleModule();

    inline void SetLastModuleLink(EditorModule *last_module) { LastModule = last_module; }
    inline void SetNextModuleLink(EditorModule *next_module) { NextModule = next_module; }
    inline EditorModule *GetLastModuleLink() { return LastModule; }
    inline EditorModule *GetNextModuleLink() { return NextModule; }
    inline void SetExternalUpdatePtr(std::uint32_t *ptr) { ExternalUpdate = ptr; }

    inline void SetLocalEscape() { EscapeFlag = true; }
    inline void ClearLocalEscape() { EscapeFlag = false; }
    inline bool LocalEscape() { return EscapeFlag; }
};

#endif
