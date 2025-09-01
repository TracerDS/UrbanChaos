// Draw2D.c
// Guy Simmons, 7th October 1996.

#include "Editor.hpp"

//---------------------------------------------------------------
// Function pointers for draw routines.

void (*DrawBox)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
void (*DrawBoxC)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);

void (*DrawLine)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
void (*DrawLineC)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
void (*DrawHLine)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
void (*DrawHLineC)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
void (*DrawVLine)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
void (*DrawVLineC)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);

void (*DrawPoint)(MFPoint *the_point, std::uint32_t colour);
void (*DrawPointC)(MFPoint *the_point, std::uint32_t colour);

void (*DrawPixel)(std::int32_t x, std::int32_t y, std::uint32_t colour);
void (*DrawPixelC)(std::int32_t x, std::int32_t y, std::uint32_t colour);

void (*QuickText)(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
void (*QuickTextC)(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
void (*QuickChar)(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);
void (*QuickCharC)(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);

void (*DrawBSprite)(std::int32_t x, std::int32_t y, BSprite *the_sprite);
void (*DrawBSpriteC)(std::int32_t x, std::int32_t y, BSprite *the_sprite);

void (*DrawMonoBSprite)(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
void (*DrawMonoBSpriteC)(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);

//---------------------------------------------------------------

extern void DrawBox8(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void DrawBoxC8(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void DrawBox16(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void DrawBoxC16(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void DrawBox32(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void DrawBoxC32(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);

extern void DrawLine8(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void DrawLineC8(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void DrawLine16(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void DrawLineC16(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void DrawLine32(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void DrawLineC32(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);

extern void DrawHLine8(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void DrawHLineC8(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void DrawHLine16(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void DrawHLineC16(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void DrawHLine32(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void DrawHLineC32(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);

extern void DrawVLine8(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void DrawVLineC8(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void DrawVLine16(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void DrawVLineC16(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void DrawVLine32(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void DrawVLineC32(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);

extern void DrawPoint8(MFPoint *the_point, std::uint32_t colour);
extern void DrawPointC8(MFPoint *the_point, std::uint32_t colour);
extern void DrawPoint16(MFPoint *the_point, std::uint32_t colour);
extern void DrawPointC16(MFPoint *the_point, std::uint32_t colour);
extern void DrawPoint32(MFPoint *the_point, std::uint32_t colour);
extern void DrawPointC32(MFPoint *the_point, std::uint32_t colour);

extern void DrawPixel8(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void DrawPixelC8(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void DrawPixel16(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void DrawPixelC16(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void DrawPixel32(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void DrawPixelC32(std::int32_t x, std::int32_t y, std::uint32_t colour);

extern void QuickText8(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickTextC8(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickChar8(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);
extern void QuickCharC8_16_32(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);
extern void QuickText16(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickTextC16(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickChar16(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);
extern void QuickText32(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickTextC32(std::int32_t x, std::int32_t y, char *the_string, std::uint32_t colour);
extern void QuickChar32(std::int32_t x, std::int32_t y, char the_char, std::uint32_t colour);

extern void DrawBSprite8(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void DrawBSprite16(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void DrawBSprite32(std::int32_t x, std::int32_t y, BSprite *the_sprite);

extern void DrawBSpriteC8(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void DrawBSpriteC16(std::int32_t x, std::int32_t y, BSprite *the_sprite);
extern void DrawBSpriteC32(std::int32_t x, std::int32_t y, BSprite *the_sprite);

extern void DrawMonoBSprite8(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
extern void DrawMonoBSprite16(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
extern void DrawMonoBSprite32(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);

extern void DrawMonoBSpriteC8(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
extern void DrawMonoBSpriteC16(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);
extern void DrawMonoBSpriteC32(std::int32_t x, std::int32_t y, BSprite *the_sprite, std::uint32_t colour);

//---------------------------------------------------------------
// Set Function pointers for this display depth

void SetDrawFunctions(std::uint32_t depth) {
    switch (depth) {
    case 8:
        DrawBox = DrawBox8;
        DrawBoxC = DrawBoxC8;

        DrawLine = DrawLine8;
        DrawLineC = DrawLineC8;
        DrawHLine = DrawHLine8;
        DrawHLineC = DrawHLineC8;
        DrawVLine = DrawVLine8;
        DrawVLineC = DrawVLineC8;

        DrawPoint = DrawPoint8;
        DrawPointC = DrawPointC8;

        DrawPixel = DrawPixel8;
        DrawPixelC = DrawPixelC8;

        QuickText = QuickText8;
        QuickTextC = QuickTextC8;
        QuickChar = QuickChar8;
        QuickCharC = QuickCharC8_16_32;

        DrawBSprite = DrawBSprite8;
        DrawBSpriteC = DrawBSpriteC8;

        DrawMonoBSprite = DrawMonoBSprite8;
        DrawMonoBSpriteC = DrawMonoBSpriteC8;

        WorkScreenDepth = 1;
        break;
    case 15:
    case 16:
        DrawBox = DrawBox16;
        DrawBoxC = DrawBoxC16;

        DrawLine = DrawLine16;
        DrawLineC = DrawLineC8;
        DrawHLine = DrawHLine16;
        DrawHLineC = DrawHLineC16;
        DrawVLine = DrawVLine16;
        DrawVLineC = DrawVLineC16;

        DrawPoint = DrawPoint16;
        DrawPointC = DrawPointC16;

        DrawPixel = DrawPixel16;
        DrawPixelC = DrawPixelC16;

        QuickText = QuickText16;
        QuickTextC = QuickTextC16;
        QuickChar = QuickChar16;
        QuickCharC = QuickCharC8_16_32;

        DrawBSprite = DrawBSprite16;
        DrawBSpriteC = DrawBSpriteC16;

        DrawMonoBSprite = DrawMonoBSprite16;
        DrawMonoBSpriteC = DrawMonoBSpriteC16;

        WorkScreenDepth = 2;
        break;
    case 24:
    case 32:
        DrawBox = DrawBox32;
        DrawBoxC = DrawBoxC32;

        DrawLine = DrawLine32;
        DrawLineC = DrawLineC8;
        DrawHLine = DrawHLine32;
        DrawHLineC = DrawHLineC32;
        DrawVLine = DrawVLine32;
        DrawVLineC = DrawVLineC32;

        DrawPoint = DrawPoint32;
        DrawPointC = DrawPointC32;

        DrawPixel = DrawPixel32;
        DrawPixelC = DrawPixelC32;

        QuickText = QuickText32;
        QuickTextC = QuickTextC32;
        QuickChar = QuickChar32;
        QuickCharC = QuickCharC8_16_32;

        DrawBSprite = DrawBSprite32;
        DrawBSpriteC = DrawBSpriteC32;

        DrawMonoBSprite = DrawMonoBSprite32;
        DrawMonoBSpriteC = DrawMonoBSpriteC32;

        WorkScreenDepth = 4;
        break;
    }
}

//---------------------------------------------------------------
