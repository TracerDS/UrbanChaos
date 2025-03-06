// WorkScreen.h
// Guy Simmons, 10th December 1997

#ifndef WORK_SCREEN_H
#define WORK_SCREEN_H

// Never used on DC.
#ifndef TARGET_DC

//---------------------------------------------------------------

extern std::uint8_t *WorkScreen,
    WorkScreenDepth;
extern std::int32_t WorkScreenHeight,
    WorkScreenWidth,
    WorkScreenPixelWidth;

extern std::uint8_t *WorkWindow;
extern std::int32_t WorkWindowHeight,
    WorkWindowWidth;
extern MFRect WorkWindowRect;

//---------------------------------------------------------------

void ShowWorkScreen(std::uint32_t flags);
void *LockWorkScreen();
void UnlockWorkScreen();

void SetWorkWindowBounds(std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);

inline void SetWorkWindow() {
    WorkWindow = (WorkScreen + WorkWindowRect.Left * WorkScreenDepth + (WorkWindowRect.Top * WorkScreenWidth));
}

MFPoint *GlobalToLocal(MFPoint *the_point);
void GlobalXYToLocal(std::int32_t *x, std::int32_t *y);
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

#endif // #ifndef TARGET_DC

#endif
