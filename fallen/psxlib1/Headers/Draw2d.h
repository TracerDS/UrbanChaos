// Draw2D.h
// Guy Simmons, 7th October 1996.

#ifndef _DRAW2D_H_
#define _DRAW2D_H_

#ifndef _MF_TYPES_H_
#include <MFTypes.h>
#endif

#ifndef _DISPLAY_H_
#include <Display.h>
#endif

typedef struct
{
    std::int32_t X,
        Y;
} MFPoint;

typedef struct
{
    std::int32_t Left,
        Top,
        Right,
        Bottom,
        Width,
        Height;
} MFRect;

inline bool XYInRect(std::int32_t x, std::int32_t y, MFRect *the_rect) {
    if (x >= the_rect->Left && y >= the_rect->Top && x <= the_rect->Right && y <= the_rect->Bottom)
        return true;
    else
        return false;
}
inline bool PointInRect(MFPoint *the_point, MFRect *the_rect) {
    if (the_point->X >= the_rect->Left && the_point->Y >= the_rect->Top && the_point->X <= the_rect->Right && the_point->Y <= the_rect->Bottom)
        return true;
    else
        return false;
}

//---------------------------------------------------------------
// Draw2D.c

extern std::uint8_t *WorkWindow;
extern std::int32_t WorkWindowHeight,
    WorkWindowWidth;
extern MFRect WorkWindowRect;

void SetWorkWindowBounds(std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
MFPoint *GlobalToLocal(MFPoint *the_point);
void GlobalXYToLocal(std::int32_t *x, std::int32_t *y);
inline void SetWorkWindow() {
    WorkWindow = (WorkScreen + WorkWindowRect.Left * WorkScreenDepth + (WorkWindowRect.Top * WorkScreenWidth));
}

//---------------------------------------------------------------
// DrawBox.c

extern void (*DrawBox)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);
extern void (*DrawBoxC)(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour);

//---------------------------------------------------------------
// DrawLine.c

extern void (*DrawLine)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void (*DrawLineC)(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour);
extern void (*DrawHLine)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void (*DrawHLineC)(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour);
extern void (*DrawVLine)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);
extern void (*DrawVLineC)(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour);

//---------------------------------------------------------------
// DrawPoint.c

extern void (*DrawPoint)(MFPoint *the_point, std::uint32_t colour);
extern void (*DrawPointC)(MFPoint *the_point, std::uint32_t colour);

//---------------------------------------------------------------
// DrawPixel.c

extern void (*DrawPixel)(std::int32_t x, std::int32_t y, std::uint32_t colour);
extern void (*DrawPixelC)(std::int32_t x, std::int32_t y, std::uint32_t colour);

//---------------------------------------------------------------
// DrawRect.c

void DrawRect(MFRect *the_rect, std::uint32_t colour);
void DrawRectC(MFRect *the_rect, std::uint32_t colour);

//---------------------------------------------------------------
// QuickText.c

extern std::uint8_t *CharTable[];

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

//---------------------------------------------------------------

#endif