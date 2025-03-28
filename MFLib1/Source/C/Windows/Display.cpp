// Display.cpp	-	Windows.
// Guy Simmons, 1st February 1997.

#include <MFHeader.h>

#define MF_DD2

bool EmulateLoRes = false,
     Got3DFX = false;
std::uint8_t DisplayActive = 0,
             DisplayState = 0,
             *WorkScreen,
             WorkScreenDepth;
std::int32_t DisplayMode = DISPLAY_MODE_NONE,
             WorkScreenHeight,
             WorkScreenWidth,
             WorkScreenPixelWidth;
GUID DevicePrimary,
    Device3DFX;
DDSURFACEDESC DD_DisplayDesc;
LPDIRECTDRAW lp_DD = nullptr;    // Main DirectDraw object
LPDIRECTDRAW2 lp_DD_2 = nullptr; // Main DirectDraw2 object
LPDIRECTDRAWCLIPPER lp_DD_Clipper = nullptr;
LPDIRECTDRAWSURFACE lp_DD_BackSurface = nullptr,
                    lp_DD_FrontSurface = nullptr,
                    lp_DD_WorkSurface = nullptr;

volatile std::uint8_t MFShellActive = 0;

std::int32_t CreateSurfaces();
std::int32_t DestroySurfaces();
void RestoreSurfaces();
HRESULT CALLBACK DisplayModesCallback(LPDDSURFACEDESC p_dd_sd, LPVOID ignore);

extern HANDLE hShellThread;
extern HWND hShellWindow;
extern RECT ShellRect;
extern void SetDrawFunctions(std::uint32_t depth);

bool WINAPI EnumDeviceCallback(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext);

//---------------------------------------------------------------

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags) {
    std::int32_t result = DisplayCreationError;
    HRESULT dd_result;

    WorkScreenHeight = height;
    WorkScreenWidth = width;
    WorkScreenPixelWidth = width;

    DisplayActive = 0;

    if (hShellThread) {
        // Create Direct Draw Object.
#ifdef _DEBUG
        flags = flags;
        dd_result = DirectDrawCreate((GUID *) DDCREATE_EMULATIONONLY, &lp_DD, nullptr);
#else
        dd_result = DirectDrawEnumerate(EnumDeviceCallback, nullptr);

        if (Got3DFX && flags & FLAGS_USE_3DFX)
            dd_result = DirectDrawCreate(&Device3DFX, &lp_DD, nullptr);
        else
            dd_result = DirectDrawCreate(nullptr, &lp_DD, nullptr);
#endif
        if (dd_result == DD_OK) {
#ifdef MF_DD2
            dd_result = lp_DD->QueryInterface(IID_IDirectDraw2, (LPVOID *) &lp_DD_2);
#endif

            if (dd_result == DD_OK) {
                DisplayState |= DS_DIRECT_DRAW_ACTIVE;

#ifdef _DEBUG
                // Normal mode for Debug.
#ifdef MF_DD2
                dd_result = lp_DD_2->SetCooperativeLevel(hShellWindow, DDSCL_NORMAL);
#else
                dd_result = lp_DD->SetCooperativeLevel(hShellWindow, DDSCL_NORMAL);
#endif

#else
                // Exclusive mode for Release.
#ifdef MF_DD2
                dd_result = lp_DD_2->SetCooperativeLevel(hShellWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN); //|DDSCL_ALLOWMODEX);
#else
                dd_result = lp_DD->SetCooperativeLevel(hShellWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN); //|DDSCL_ALLOWMODEX);
#endif

                // Find all the available display modes.
#ifdef MF_DD2
                lp_DD_2->EnumDisplayModes(0, nullptr, 0, DisplayModesCallback);
#else
                lp_DD->EnumDisplayModes(0, nullptr, 0, DisplayModesCallback);
#endif

#endif
                InitPalettes();

                if (SetDisplay(width, height, depth) == NoError) {
                    SetupD3D2();
                    return NoError;
                }
            }
        }
    }

    return result;
}

//---------------------------------------------------------------

std::int32_t CloseDisplay() {
    HRESULT dd_result;

    /*
            if(DisplayState&DS_THREAD_ACTIVE)
            {
                    SendMessage(hShellWindow,WM_CLOSE,0,0);
                    WaitForSingleObject(hShellThread,4000);
                    CloseHandle(hShellThread);
            }
    */

    DestroySurfaces();
    DestroyPalettes();

#ifdef MF_DD2
    if (lp_DD_2)
        dd_result = lp_DD_2->SetCooperativeLevel(hShellWindow, DDSCL_NORMAL);
#else
    if (lp_DD)
        dd_result = lp_DD->SetCooperativeLevel(hShellWindow, DDSCL_NORMAL);
#endif
    /*
            ResetD3D2();

            if(lp_DD_2)
            {
                    dd_result	=	lp_DD_2->Release();
                    if(dd_result!=DD_OK)
                    {
                            LogText("error: %ld - Unable to release DirectDraw2\n",dd_result&0xff);
                    }
                    else
                            lp_DD_2		=	nullptr;
            }
    */
    if (lp_DD) {
        dd_result = lp_DD->Release();
        if (dd_result != DD_OK) {
            LogText("error: %ld - Unable to release DirectDraw\n", dd_result & 0xff);
        } else
            lp_DD = nullptr;
    }
    return NoError;
}

//---------------------------------------------------------------

std::int32_t SetDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth) {
    std::int32_t mode = DISPLAY_MODE_NONE;
    HRESULT dd_result;

    if (DisplayState & DS_SCREEN_LOCKED) {
        ERROR_MSG(0, "Screen Locked");
    }
    if (DisplayState & DS_DIRECT_DRAW_ACTIVE) {
        mode = DisplayModeAvailable(width, height, depth);
        if (mode) {
            // Clear away the old display stuff.
            DestroySurfaces();
            DestroyPalettes();

            // Initially force
            WorkScreenHeight = height;
            WorkScreenWidth = width;
            WorkScreenPixelWidth = width;
            WorkScreenDepth = depth;

            // Set display size.
#ifdef _RELEASE
            if (EmulateLoRes) {
#ifdef MF_DD2
                dd_result = lp_DD_2->SetDisplayMode(640, 480, 8, 0, 0);
#else
                dd_result = lp_DD->SetDisplayMode(640, 480, 8);
#endif
            } else {
#ifdef MF_DD2
                dd_result = lp_DD_2->SetDisplayMode(width, height, depth, 0, 0);
#else
                dd_result = lp_DD->SetDisplayMode(width, height, depth);
#endif
            }
#else
            DWORD dw_style;
            RECT temp_rect;

            dw_style = GetWindowStyle(hShellWindow);
            dw_style &= ~WS_POPUP;
            dw_style |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
            SetWindowLong(hShellWindow, GWL_STYLE, dw_style);

            SetRect(&temp_rect, 0, 0, width, height);
            AdjustWindowRectEx(
                &temp_rect,
                GetWindowStyle(hShellWindow),
                GetMenu(hShellWindow) != nullptr,
                GetWindowExStyle(hShellWindow));
            SetWindowPos(
                hShellWindow,
                nullptr,
                0, 0,
                temp_rect.right - temp_rect.left,
                temp_rect.bottom - temp_rect.top,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            dd_result = DD_OK;
#endif

            if (dd_result == DD_OK) {
                // Create the new display stuff.
                DisplayState |= DS_DISPLAY_MODE_SET;
                DisplayMode = mode;
#ifdef _DEBUG
                dd_result = lp_DD_2->GetDisplayMode(&DD_DisplayDesc);
#else
                DD_DisplayDesc = DisplayModes[DisplayMode].DD_ModeDesc;
#endif
                CreateSurfaces();
                CreatePalettes();

                SetWorkWindowBounds(0, 0, width, height);
                SetDrawFunctions(depth);

                return NoError;
            } else {
                LogText("Unable to set display mode. Error: %ld\n", dd_result & 0xffff);
                //				ERROR_MSG((dd_result==DD_OK),"Unable to set display mode.");
            }
        } else {
            LogText("\nDisplay Mode not available\n");
            //			ERROR_MSG((dd_result==DD_OK),"Display mode unavailable.");
        }
    }
    return -1;
}

//---------------------------------------------------------------

std::int32_t CreateSurfaces() {
#ifdef _RELEASE
    DDSCAPS dd_scaps;
#endif
    DDSURFACEDESC dd_sd;
    HRESULT dd_result;

    ZeroMemory(&dd_sd, sizeof(dd_sd));
    dd_sd.dwSize = sizeof(dd_sd);
#ifdef _DEBUG
    if (lp_DD_FrontSurface == nullptr) {
        // Create DEBUG front surface.
        dd_sd.dwFlags = DDSD_CAPS;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
#ifdef MF_DD2
        dd_result = lp_DD_2->CreateSurface(&dd_sd, &lp_DD_FrontSurface, nullptr);
#else
        dd_result = lp_DD->CreateSurface(&dd_sd, &lp_DD_FrontSurface, nullptr);
#endif
        ERROR_MSG((dd_result == DD_OK), "Can't create lp_DD_FrontSurface");
    }

    if (lp_DD_BackSurface == nullptr) {
        // Create DEBUG back surface.
        dd_sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        dd_sd.dwHeight = WorkScreenHeight;
        dd_sd.dwWidth = WorkScreenWidth;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE; //|DDSCAPS_SYSTEMMEMORY;
#ifdef MF_DD2
        dd_result = lp_DD_2->CreateSurface(&dd_sd, &lp_DD_BackSurface, nullptr);
#else
        dd_result = lp_DD->CreateSurface(&dd_sd, &lp_DD_BackSurface, nullptr);
#endif
        ERROR_MSG((dd_result == DD_OK), "Can't create lp_DD_BackSurface");
    }

    if (lp_DD_Clipper == nullptr) {
        // Create DEBUG clipper.
#ifdef DD2
        dd_result = lp_DD_2->CreateClipper(0, &lp_DD_Clipper, nullptr);
#else
        dd_result = lp_DD->CreateClipper(0, &lp_DD_Clipper, nullptr);
#endif
        ERROR_MSG((dd_result == DD_OK), "Can't create lp_DD_Clipper");

        // Set clipper window handle.
        dd_result = lp_DD_Clipper->SetHWnd(0, hShellWindow);
        ERROR_MSG((dd_result == DD_OK), "Can't set lp_DD_Clipper window handle");

        // Attach clipper to front surface.
        lp_DD_FrontSurface->SetClipper(lp_DD_Clipper);
        ERROR_MSG((dd_result == DD_OK), "Can't attach lp_DD_Clipper to lp_DD_FrontSurface");
    }
#else
    if (lp_DD_FrontSurface == nullptr) {
        // Create RELEASE front & back Surfaces.
        dd_sd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
        dd_sd.dwBackBufferCount = 1;
#ifdef MF_DD2
        dd_result = lp_DD_2->CreateSurface(&dd_sd, &lp_DD_FrontSurface, nullptr);
#else
        dd_result = lp_DD->CreateSurface(&dd_sd, &lp_DD_FrontSurface, nullptr);
#endif
        ERROR_MSG((dd_result == DD_OK), "Can't create lp_DD_FrontSurface");

        // Extract RELEASE Back surface.
        dd_scaps.dwCaps = DDSCAPS_BACKBUFFER;
        dd_result = lp_DD_FrontSurface->GetAttachedSurface(&dd_scaps, &lp_DD_BackSurface);
        ERROR_MSG((dd_result == DD_OK), "Can't extract lp_DD_BackSurface");
    }
#endif
    if (lp_DD_WorkSurface == nullptr) {
        // Create work surface.
        dd_sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        dd_sd.dwHeight = WorkScreenHeight;
        dd_sd.dwWidth = WorkScreenWidth;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
#ifdef MF_DD2
        dd_result = lp_DD_2->CreateSurface(&dd_sd, &lp_DD_WorkSurface, nullptr);
#else
        dd_result = lp_DD->CreateSurface(&dd_sd, &lp_DD_WorkSurface, nullptr);
#endif
        ERROR_MSG((dd_result == DD_OK), "Can't create lp_DD_WorkSurface");
    }

    DisplayState |= DS_CREATED_SURFACES;

    return NoError;
}

//---------------------------------------------------------------

std::int32_t DestroySurfaces() {
    HRESULT dd_result;

#ifdef _DEBUG
    if (lp_DD_Clipper) {
        lp_DD_Clipper->Release();
        lp_DD_Clipper = nullptr;
    }
    if (lp_DD_BackSurface) {
        dd_result = lp_DD_BackSurface->Release();
        //		if(dd_result!=DD_OK)
        //			LogText("error: %ld - Unable to release lp_DD_BackSurface\n",dd_result&0xffff);
    }
#endif
    if (lp_DD_FrontSurface) {
        dd_result = lp_DD_FrontSurface->Release();
        if (dd_result != DD_OK)
            LogText("error: %ld - Unable to release lp_DD_FrontSurface\n", dd_result & 0xffff);
        lp_DD_FrontSurface = nullptr;
        lp_DD_BackSurface = nullptr;
    }
    if (lp_DD_WorkSurface) {
        dd_result = lp_DD_WorkSurface->Release();
        if (dd_result != DD_OK)
            LogText("error: %ld - Unable to release lp_DD_WorkSurface\n", dd_result & 0xffff);
        lp_DD_WorkSurface = nullptr;
    }
    DisplayState &= ~(DS_CREATED_SURFACES);

    return NoError;
}

//---------------------------------------------------------------

void ClearDisplay() {
    DDBLTFX dd_bltfx;
    HRESULT dd_result;

    dd_bltfx.dwSize = sizeof(dd_bltfx);
    dd_bltfx.dwFillColor = 0;
    dd_result = lp_DD_FrontSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &dd_bltfx);
    switch (dd_result) {
    case DD_OK:
        break;
    case DDERR_SURFACELOST:
        RestoreSurfaces();
        lp_DD_FrontSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &dd_bltfx);
        break;
    }
}

//---------------------------------------------------------------

void FadeDisplay(std::uint8_t mode) {
    switch (mode) {
    case FADE_IN:
        break;
    case FADE_OUT:
        break;
    }
}

//---------------------------------------------------------------

void *LockWorkScreen() {
    DDSURFACEDESC dd_sd;
    HRESULT dd_result;

    if (lp_DD_WorkSurface) {
        dd_sd.dwSize = sizeof(dd_sd);
        dd_result = lp_DD_WorkSurface->Lock(nullptr, &dd_sd, DDLOCK_WAIT, nullptr);
        switch (dd_result) {
        case DD_OK:
            WorkScreenWidth = dd_sd.lPitch;
            WorkScreen = (std::uint8_t *) dd_sd.lpSurface;
            SetWorkWindow();
            DisplayState |= DS_SCREEN_LOCKED;
            return dd_sd.lpSurface;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            return 0;
        }
    }
    return 0;
}

//---------------------------------------------------------------

void UnlockWorkScreen() {
    HRESULT dd_result;

    if (lp_DD_WorkSurface) {
        dd_result = lp_DD_WorkSurface->Unlock(nullptr);
        switch (dd_result) {
        case DD_OK:
            DisplayState &= ~(DS_SCREEN_LOCKED);
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        }
    }
}

//---------------------------------------------------------------

void ShowWorkScreen(std::uint32_t flags) {
    HRESULT dd_result;

    if (lp_DD_WorkSurface == nullptr)
        return;

    // Apparently Blt is better than BltFst for System to Display blits.

    if (flags & DS_WAIT_VBI) {
#ifdef MF_DD2
        dd_result = lp_DD_2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#else
        dd_result = lp_DD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#endif
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
#ifdef MF_DD2
            lp_DD_2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#else
            lp_DD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#endif
            break;
        }
    }

    if (flags & DS_DO_FLIP) {
#ifdef _DEBUG
        dd_result = lp_DD_BackSurface->Blt(
            &ShellRect,
            lp_DD_WorkSurface,
            nullptr,
            DDBLT_WAIT,
            nullptr);
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
/*
                dd_result	=	lp_DD_FrontSurface->Blt	(
                                                                                                        &ShellRect,
                                                                                                        lp_DD_BackSurface,
                                                                                                        nullptr,
                                                                                                        DDBLT_WAIT,
                                                                                                        nullptr
                                                                                                );
                switch(dd_result)
                {
                        case	DD_OK:
                                break;
                        case	DDERR_SURFACELOST:
                                RestoreSurfaces();
                                break;
                        default:
                                LogText("Flip failed in ShowWorkScreen. Error: %ld\n",dd_result&0xffff);
                }
*/
#else
        dd_result = lp_DD_BackSurface->Blt(
            nullptr,
            lp_DD_WorkSurface,
            nullptr,
            DDBLT_WAIT,
            nullptr);
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
/*
                dd_result	=	lp_DD_FrontSurface->Flip(nullptr, DDFLIP_WAIT);
                switch(dd_result)
                {
                        case	DD_OK:
                                break;
                        case	DDERR_SURFACELOST:
                                RestoreSurfaces();
                                break;
                        default:
                                LogText("Flip failed in ShowWorkScreen. Error: %ld\n",dd_result&0xffff);
                }
*/
#endif
    } else {
#ifdef _DEBUG
        dd_result = lp_DD_FrontSurface->Blt(
            &ShellRect,
            lp_DD_WorkSurface,
            nullptr,
            DDBLT_WAIT,
            nullptr);
#else
        dd_result = lp_DD_FrontSurface->Blt(
            nullptr,
            lp_DD_WorkSurface,
            nullptr,
            DDBLT_WAIT,
            nullptr);
#endif
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
    }
}

//---------------------------------------------------------------

void ShowWorkWindow(std::uint32_t flags) {
    HRESULT dd_result;
    RECT ww_source_rect;
#ifdef _DEBUG
    std::int32_t x_scale,
        y_scale;
    RECT ww_dest_rect;
#endif

    if (lp_DD_WorkSurface == nullptr)
        return;

    ww_source_rect.left = WorkWindowRect.Left;
    ww_source_rect.top = WorkWindowRect.Top;
    ww_source_rect.right = WorkWindowRect.Right;
    ww_source_rect.bottom = WorkWindowRect.Bottom;

    // Apparently Blt is better than BltFst for System to Display blits.

    if (flags & DS_WAIT_VBI) {
#ifdef MF_DD2
        dd_result = lp_DD_2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#else
        dd_result = lp_DD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#endif
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_WASSTILLDRAWING:
            while (lp_DD_WorkSurface->GetBltStatus(DDGBS_ISBLTDONE) == DDERR_WASSTILLDRAWING);
            while (lp_DD_BackSurface->GetBltStatus(DDGBS_ISBLTDONE) == DDERR_WASSTILLDRAWING);
            while (lp_DD_FrontSurface->GetBltStatus(DDGBS_ISBLTDONE) == DDERR_WASSTILLDRAWING);
#ifdef MF_DD2
            lp_DD_2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#else
            lp_DD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, nullptr);
#endif
            break;
        }
    }

    if (flags & DS_DO_FLIP) {
#ifdef _DEBUG
        x_scale = ((ShellRect.right - ShellRect.left) << 16) / WorkScreenPixelWidth;
        y_scale = ((ShellRect.bottom - ShellRect.top) << 16) / WorkScreenHeight;
        ww_dest_rect.left = ShellRect.left + ww_source_rect.left;
        ww_dest_rect.top = ShellRect.top + ww_source_rect.top;
        ww_dest_rect.right = ww_dest_rect.left + ((WorkWindowWidth * x_scale) >> 16);
        ww_dest_rect.bottom = ww_dest_rect.top + ((WorkWindowHeight * y_scale) >> 16);

        dd_result = lp_DD_BackSurface->Blt(
            &ww_source_rect,
            lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
/*
                dd_result	=	lp_DD_FrontSurface->Blt	(
                                                                                                        &ww_dest_rect,
                                                                                                        lp_DD_BackSurface,
                                                                                                        &ShellRect,
                                                                                                        DDBLT_WAIT,
                                                                                                        nullptr
                                                                                                );
                switch(dd_result)
                {
                        case	DD_OK:
                                break;
                        case	DDERR_SURFACELOST:
                                RestoreSurfaces();
                                break;
                        default:
                                LogText("Flip failed in ShowWorkScreen. Error: %ld\n",dd_result&0xffff);
                }
*/
#else
        dd_result = lp_DD_BackSurface->Blt(
            &ww_source_rect,
            lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
/*
                dd_result	=	lp_DD_FrontSurface->Flip(nullptr, DDFLIP_WAIT);
                switch(dd_result)
                {
                        case	DD_OK:
                                break;
                        case	DDERR_SURFACELOST:
                                RestoreSurfaces();
                                break;
                        default:
                                LogText("Flip failed in ShowWorkScreen. Error: %ld\n",dd_result&0xffff);
                }
*/
#endif
    } else {
#ifdef _DEBUG
        x_scale = ((ShellRect.right - ShellRect.left) << 16) / WorkScreenPixelWidth;
        y_scale = ((ShellRect.bottom - ShellRect.top) << 16) / WorkScreenHeight;
        ww_dest_rect.left = ShellRect.left + ww_source_rect.left;
        ww_dest_rect.top = ShellRect.top + ww_source_rect.top;
        ww_dest_rect.right = ww_dest_rect.left + ((WorkWindowWidth * x_scale) >> 16);
        ww_dest_rect.bottom = ww_dest_rect.top + ((WorkWindowHeight * y_scale) >> 16);

        dd_result = lp_DD_FrontSurface->Blt(
            &ww_dest_rect,
            lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
#else
        dd_result = lp_DD_FrontSurface->Blt(
            &ww_source_rect,
            lp_DD_WorkSurface,
            &ww_source_rect,
            DDBLT_WAIT,
            nullptr);
#endif
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            break;
        default:
            LogText("Blt failed in ShowWorkScreen. Error: %ld\n", dd_result & 0xffff);
        }
    }
}

//---------------------------------------------------------------

void ClearWorkScreen(std::uint8_t colour) {
    DDBLTFX dd_bltfx;
    HRESULT dd_result;

    if (lp_DD_WorkSurface) {
        dd_bltfx.dwSize = sizeof(dd_bltfx);
        dd_bltfx.dwFillColor = colour;
        dd_result = lp_DD_WorkSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &dd_bltfx);
        switch (dd_result) {
        case DD_OK:
            break;
        case DDERR_SURFACELOST:
            RestoreSurfaces();
            lp_DD_WorkSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &dd_bltfx);
            break;
        }
    }
}

//---------------------------------------------------------------

void RestoreSurfaces() {
    if (lp_DD_FrontSurface) {
        if (lp_DD_FrontSurface->IsLost() == DDERR_SURFACELOST) {
            lp_DD_FrontSurface->Restore();
            RestorePalettes();
        }
    }
#ifdef _DEBUG
    if (lp_DD_BackSurface) {
        if (lp_DD_BackSurface->IsLost() == DDERR_SURFACELOST) {
            lp_DD_BackSurface->Restore();
        }
    }
#endif
    if (lp_DD_WorkSurface) {
        if (lp_DD_WorkSurface->IsLost() == DDERR_SURFACELOST) {
            lp_DD_WorkSurface->Restore();
        }
    }
}

//---------------------------------------------------------------

HRESULT CALLBACK DisplayModesCallback(LPDDSURFACEDESC p_dd_sd, LPVOID ignore) {
    std::uint32_t c0;

    ignore = ignore; // Stop the compiler whinging.

    for (c0 = 1; c0 < DISPLAY_MODE_COUNT; c0++) {
        if (
            DisplayModes[c0].Width == p_dd_sd->dwWidth &&
            DisplayModes[c0].Height == p_dd_sd->dwHeight &&
            DisplayModes[c0].Depth == p_dd_sd->ddpfPixelFormat.dwRGBBitCount) {
            DisplayModes[c0].DD_ModeDesc = *p_dd_sd;
            DisplayModes[c0].Availability = true;
            break;
        }
    }
    return DDENUMRET_OK;
}

//---------------------------------------------------------------

bool WINAPI EnumDeviceCallback(
    GUID FAR *lpGUID,
    LPSTR lpDriverDescription,
    LPSTR lpDriverName,
    LPVOID lpContext) {
    char *str_ptr = lpDriverDescription;

    lpDriverName = lpDriverName;
    lpContext = lpContext;

    while (*str_ptr != 0) {
        if (*str_ptr == '3') {
            if ((*(str_ptr + 1) == 'D' || *(str_ptr + 1) == 'd') && (*(str_ptr + 2) == 'F' || *(str_ptr + 2) == 'f') && (*(str_ptr + 3) == 'X' || *(str_ptr + 3) == 'x')) {
                Device3DFX = *lpGUID;
                Got3DFX = true;
                return DDENUMRET_CANCEL;
            }
        }
        str_ptr++;
    }
    return DDENUMRET_OK;
}

//---------------------------------------------------------------
