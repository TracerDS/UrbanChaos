// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef _TEXTAB_HPP_
#define _TEXTAB_HPP_

#include "undo.hpp"
#include "ModeTab.hpp"
#include "Stealth.h"

#define FLAGS_SHOW_TEXTURE (1 << 0)
#define FLAGS_QUADS (1 << 1)
#define FLAGS_FIXED (1 << 2)

class TextureTab : public ModeTab {
   private:
    std::uint32_t CurrentTexturePage,
        TextureFlags;

    std::int32_t TextureWidth,
        TextureHeight,
        TextureX,
        TextureY,
        TextureZoom;
    EditorModule *Parent;
    EdRect ClickRect[4],
        TextureRect;
    EdTexture CurrentTexture;

    void do_undo_me_bloody_self_then(std::int32_t index);

   public:
    TextureTab(EditorModule *parent);
    void DrawTabContent();
    void DrawTexture();
    void UpdateTexture();
    void UpdateTextureInfo();
    void HandleTab(MFPoint *current_point);
    std::uint16_t HandleTabClick(std::uint8_t flags, MFPoint *clicked_point);
    void HandleControl(std::uint16_t control_id);
    std::uint16_t ConvertFreeToFixedEle(struct TextureBits *t);
    void ConvertFixedToFree(struct TextureBits *t);

    inline std::uint32_t GetTexturePage() { return CurrentTexturePage; }
    inline void SetTexturePage(std::uint32_t page) { CurrentTexturePage = page; }
    inline EdTexture *GetTexture() { return &CurrentTexture; }
    inline std::uint32_t GetTextureFlags() { return TextureFlags; }
    inline void SetTextureFlags(std::uint32_t flags) { TextureFlags = flags; }
    Undo MyUndo;
    bool ApplyTexture(struct EditFace *edit_face);
};

#endif
