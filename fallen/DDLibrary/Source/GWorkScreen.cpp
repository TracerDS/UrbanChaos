// WorkScreen.cpp
// Guy Simmons, 10th December 1997

// The work screen is never used on the DC
#ifndef TARGET_DC

#include "DDLib.h"

std::uint8_t CurrentPalette[256 * 3],
    WorkScreenDepth,
    *WorkScreen,
    *WorkWindow;
std::int32_t WorkScreenHeight,
    WorkScreenWidth,
    WorkScreenPixelWidth,
    WorkWindowHeight,
    WorkWindowWidth;

MFRect WorkWindowRect;

//---------------------------------------------------------------

void ShowWorkScreen(std::uint32_t flags) {
    flags = flags;

    the_display.ShowWorkScreen();
}

//---------------------------------------------------------------

void* LockWorkScreen() {
    DDSURFACEDESC2 dd_sd;
    HRESULT result;

    if (the_display.lp_DD_WorkSurface) {
        InitStruct(dd_sd);
        result = the_display.lp_DD_WorkSurface->Lock(
            nullptr,
            &dd_sd,
            DDLOCK_WAIT,
            nullptr);
        switch (result) {
        case DD_OK:
            WorkScreenPixelWidth = dd_sd.dwWidth;
            WorkScreenWidth = dd_sd.lPitch;
            WorkScreenHeight = dd_sd.dwHeight;
            WorkScreen = (std::uint8_t*) dd_sd.lpSurface;
            SetWorkWindow();
            return dd_sd.lpSurface;
        case DDERR_SURFACELOST:
            the_display.Restore();
            return LockWorkScreen();
        }
    }
    return 0;
}

//---------------------------------------------------------------

void UnlockWorkScreen() {
    HRESULT result;

    if (the_display.lp_DD_WorkSurface) {
        result = the_display.lp_DD_WorkSurface->Unlock(nullptr);
        if (result == DDERR_SURFACELOST) {
            the_display.Restore();
            UnlockWorkScreen();
        }
    }
}

//---------------------------------------------------------------

void ClearWorkScreen(std::uint8_t colour) {
    DDBLTFX dd_bltfx;
    HRESULT result;

    if (the_display.lp_DD_WorkSurface) {
        InitStruct(dd_bltfx);
        dd_bltfx.dwFillColor = colour;
        result = the_display.lp_DD_WorkSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &dd_bltfx);
        switch (result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            the_display.Restore();
            ClearWorkScreen(colour);
            break;
        }
    }
}

//---------------------------------------------------------------

void ShowWorkWindow(std::uint32_t flags) {
    std::int32_t x_scale,
        y_scale;
    HRESULT result;
    RECT ww_dest_rect,
        ww_source_rect;

    if (the_display.lp_DD_WorkSurface == nullptr)
        return;

    ww_source_rect.left = WorkWindowRect.Left;
    ww_source_rect.top = WorkWindowRect.Top;
    ww_source_rect.right = WorkWindowRect.Right;
    ww_source_rect.bottom = WorkWindowRect.Bottom;

    if (the_display.IsFullScreen()) {
        //		result	=	the_display.ShowWorkScreen();
        result = the_display.lp_DD_FrontSurface->Blt(
            &ww_source_rect,
            the_display.lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
    } else {
        x_scale = ((the_display.DisplayRect.right - the_display.DisplayRect.left) << 16) / WorkScreenPixelWidth;
        y_scale = ((the_display.DisplayRect.bottom - the_display.DisplayRect.top) << 16) / WorkScreenHeight;
        ww_dest_rect.left = the_display.DisplayRect.left + ww_source_rect.left;
        ww_dest_rect.top = the_display.DisplayRect.top + ww_source_rect.top;
        ww_dest_rect.right = ww_dest_rect.left + ((WorkWindowWidth * x_scale) >> 16);
        ww_dest_rect.bottom = ww_dest_rect.top + ((WorkWindowHeight * y_scale) >> 16);

        result = the_display.lp_DD_FrontSurface->Blt(
            &ww_dest_rect,
            the_display.lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
    }
    if (result == DDERR_SURFACELOST) {
        if (SUCCEEDED(the_display.Restore()))
            ShowWorkWindow(flags);
    }
}

//---------------------------------------------------------------

void SetWorkWindowBounds(std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height) {
    if ((left + width) >= WorkScreenPixelWidth) {
        width -= (left + width) - WorkScreenPixelWidth;
        if (width < 1) {
            left = 0;
            width = 1;
        }
    }
    if ((top + height) >= WorkScreenHeight) {
        height -= (top + height) - WorkScreenHeight;
        if (height < 1) {
            top = 0;
            height = 1;
        }
    }
    WorkWindowRect.Left = left;
    WorkWindowRect.Top = top;
    WorkWindowRect.Right = (left + width) - 1;
    WorkWindowRect.Bottom = (top + height) - 1;
    WorkWindowRect.Width = width;
    WorkWindowRect.Height = height;

    WorkWindowHeight = height;
    WorkWindowWidth = width;

    SetWorkWindow();
}

//---------------------------------------------------------------

MFPoint* GlobalToLocal(MFPoint* the_point) {
    the_point->X -= WorkWindowRect.Left;
    the_point->Y -= WorkWindowRect.Top;

    return the_point;
}

//---------------------------------------------------------------

void GlobalXYToLocal(std::int32_t* x, std::int32_t* y) {
    *x -= WorkWindowRect.Left;
    *y -= WorkWindowRect.Top;
}

//---------------------------------------------------------------

void SetPalette(std::uint8_t* the_palette) {
    std::uint32_t c0;

    for (c0 = 0; c0 < 256; c0++) {
        CurrentPalette[c0 * 3 + 0] = the_palette[(c0 * 3) + 0];
        CurrentPalette[c0 * 3 + 1] = the_palette[(c0 * 3) + 1];
        CurrentPalette[c0 * 3 + 2] = the_palette[(c0 * 3) + 2];
    }
}

//---------------------------------------------------------------

std::int32_t FindColour(std::uint8_t* the_palette, std::int32_t r, std::int32_t g, std::int32_t b) {
    std::int32_t found = -1;

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;

    switch (WorkScreenDepth) {
    case 1: {
        std::int32_t dist = 0x7fffffff,
                     c0,
                     dist2,
                     tr,
                     tg,
                     tb;

        for (c0 = 0; c0 < 256; c0++) {
            tr = *the_palette++;
            tg = *the_palette++;
            tb = *the_palette++;

            tr -= r;
            tg -= g;
            tb -= b;

            dist2 = abs(tr * tr) + abs(tg * tg) + abs(tb * tb);
            if (dist2 < dist) {
                found = c0;
                dist = dist2;
                if (dist < 8)
                    return (c0);
            }
        }
        break;
    }
    case 2:
        found = the_display.GetFormattedPixel(r, g, b); //			 found=(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
        break;
    case 4:
        found = ((r << 16) | (g << 8) | (b));
        break;
    }
    return (found);
}

//---------------------------------------------------------------

#endif // #ifndef TARGET_DC
