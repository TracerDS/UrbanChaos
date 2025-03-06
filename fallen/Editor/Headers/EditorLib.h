// EditorLib.h
// Guy Simmons, 10th December 1997.

#ifndef EDITOR_LIB_H
#define EDITOR_LIB_H

#include <MFStdLib.h>
#include "..\ddlibrary\headers\DDLib.h"

void Time(struct MFTime *the_time);

//---------------------------------------------------------------
// Draw2D

extern std::uint8_t *CharTable[];
extern std::uint8_t *WorkWindow;
extern std::int32_t WorkWindowHeight,
    WorkWindowWidth;
extern MFRect WorkWindowRect;

void SetWorkWindowBounds(std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
MFPoint *GlobalToLocal(MFPoint *the_point);
void GlobalXYToLocal(std::int32_t *x, std::int32_t *y);

extern void (*DrawBox)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void (*DrawBoxC)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);

extern void DrawCircle(std::int32_t x, std::int32_t y, std::int32_t radius, std::uint32_t colour);
extern void DrawCircleC(std::int32_t x, std::int32_t y, std::int32_t radius, std::uint32_t colour);

extern void (*DrawLine)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void (*DrawLineC)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void (*DrawHLine)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void (*DrawHLineC)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void (*DrawVLine)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void (*DrawVLineC)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);

extern void (*DrawPoint)(MFPoint *the_point, std::uint32_t colour);
extern void (*DrawPointC)(MFPoint *the_point, std::uint32_t colour);

extern void (*DrawPixel)(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void (*DrawPixelC)(std::int32_t x, std::int32_t y, std::uint32_t colour);

extern void DrawRect(MFRect *the_rect, std::uint32_t colour);
extern void DrawRectC(MFRect *the_rect, std::uint32_t colour);

extern void (*QuickText)(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void (*QuickTextC)(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void (*QuickChar)(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);
extern void (*QuickCharC)(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);

std::int32_t QTStringWidth(char *the_string);
inline std::int32_t QTStringHeight() {
    return 8;
}
inline std::int32_t QTCharWidth(char the_char) {
    return (CharTable[the_char])[0];
}
inline std::int32_t QTCharHeight(char the_char) {
    return (CharTable[the_char])[1];
}

void SetDrawFunctions(std::uint32_t depth);
void ShowWorkWindow(std::uint32_t flags);

//---------------------------------------------------------------
// Palette

#define FADE_IN 1 << 0
#define FADE_OUT 1 << 1

extern std::uint8_t CurrentPalette[256 * 3];

void InitPalettes();
std::int32_t CreatePalettes();
void DestroyPalettes();
void RestorePalettes();
void SetPalette(std::uint8_t *the_palette);
std::int32_t FindColour(std::uint8_t *pal, std::int32_t r, std::int32_t g, std::int32_t b);

//---------------------------------------------------------------
// Sprites

#define END_LINE 0x00
#define COPY_PIXELS 0x01
#define SKIP_PIXELS 0x02
#define DUPLICATE_PIXELS 0x03
#define FINISHED 0x04

typedef struct
{
    std::uint8_t *SpriteData;
    std::uint16_t SpriteHeight;
    std::uint16_t SpriteWidth;
} BSprite;

extern void (*DrawBSprite)(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void (*DrawBSpriteC)(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void (*DrawMonoBSprite)(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
extern void (*DrawMonoBSpriteC)(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);

void DrawBSpritePal16(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint8_t *pal);
void DrawBSpritePal32(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint8_t *pal);
void DrawBSpritePalC16(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint8_t *pal);
void DrawBSpritePalC32(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint8_t *pal);
void SetupBSprites(BSprite *sprite_ref, std::uint8_t *sprite_data);

//---------------------------------------------------------------

#endif
