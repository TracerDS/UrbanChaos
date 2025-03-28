// Display.cpp
// Guy Simmons, 13th November 1997.

#include "DDLib.h"
#include "..\headers\demo.h"
#include "..\headers\interfac.h"
#include "BinkClient.h"
#include "..\headers\env.h"
#include "..\headers\xlat_str.h"

#include "poly.h"
#include "vertexbuffer.h"
#include "polypoint.h"
#include "renderstate.h"
#include "polypage.h"
#include "gdisplay.h"
#include "panel.h"
#include "..\headers\game.h"

//
// From mfx_miles.h...
//

std::int32_t RealDisplayWidth;
std::int32_t RealDisplayHeight;
std::int32_t DisplayBPP;
Display the_display;
volatile std::int32_t hDDLibStyle = 0,
                      hDDLibStyleEx = 0;
volatile HWND hDDLibWindow = nullptr;
volatile HMENU hDDLibMenu = nullptr;

int VideoRes = -1; // 0 = 320x240, 1 = 512x384, 2= 640x480, 3 = 800x600, 4 = 1024x768, -1 = unknown

enumDisplayType eDisplayType;

//---------------------------------------------------------------
std::uint8_t *image_mem = nullptr, *image = nullptr;

#ifdef DEBUG
HRESULT WINAPI EnumSurfacesCallbackFunc(
    LPDIRECTDRAWSURFACE4 lpDDSurface,
    LPDDSURFACEDESC2 lpDDSurfaceDesc,
    LPVOID lpContext) {
    TRACE("Surf: width %i height %i bpp %i", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount);
    if (lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_COMPRESSED) {
        TRACE(" compressed");
    } else {
        TRACE(" uncompressed");
    }
    TRACE("\n");
    return DDENUMRET_OK;
}

#endif

// ========================================================
//
// MOVIE STUFF!
//
// ========================================================

#include "mmstream.h" // Multimedia stream interfaces
#include "amstream.h" // DirectShow multimedia stream interfaces
#include "ddstream.h" // DirectDraw multimedia stream interfaces

// extern std::uint32_t get_hardware_input(std::uint16_t type);

void RenderStreamToSurface(IDirectDrawSurface *pSurface, IMultiMediaStream *pMMStream, IDirectDrawSurface *back_surface) {
    IMediaStream *pPrimaryVidStream;
    IDirectDrawMediaStream *pDDStream;
    IDirectDrawStreamSample *pSample;
    RECT rect;
    RECT midrect;
    DDSURFACEDESC ddsd;

    pMMStream->GetMediaStream(MSPID_PrimaryVideo, &pPrimaryVidStream);
    ASSERT(pPrimaryVidStream != nullptr);
    pPrimaryVidStream->QueryInterface(IID_IDirectDrawMediaStream, (void **) &pDDStream);
    ASSERT(pDDStream != nullptr);

    InitStruct(ddsd);

    pDDStream->GetFormat(&ddsd, nullptr, nullptr, nullptr);

    midrect.top = 420 - (ddsd.dwWidth >> 1);
    midrect.left = 240 - (ddsd.dwHeight >> 1);
    midrect.bottom = midrect.top + ddsd.dwHeight;
    midrect.right = midrect.left + ddsd.dwWidth;

    rect.top = 0;
    rect.left = 0;
    rect.bottom = ddsd.dwHeight;
    rect.right = ddsd.dwWidth;

    pDDStream->CreateSample(back_surface, &rect, 0, &pSample);
    ASSERT(pSample != nullptr);
    pMMStream->SetState(STREAMSTATE_RUN);

    while (pSample->Update(0, nullptr, nullptr, 0) == S_OK) {
        if (FAILED(pSurface->Blt(
                nullptr,
                back_surface,
                &rect,
                DDBLT_WAIT,
                nullptr))) {
            pSurface->Blt(
                &midrect,
                back_surface,
                &rect,
                DDBLT_WAIT,
                nullptr);
        }

        MSG msg;

        if (PeekMessage(
                &msg,
                hDDLibWindow,
                WM_KEYDOWN,
                WM_KEYDOWN,
                PM_REMOVE)) {
            //
            // User has pressed a key.
            //

            break;
        }

        std::uint32_t input = get_hardware_input(INPUT_TYPE_JOY); // 1 << 1 ==> INPUT_TYPE_JOY

        if (input & (INPUT_MASK_JUMP | INPUT_MASK_START | INPUT_MASK_SELECT | INPUT_MASK_KICK | INPUT_MASK_PUNCH | INPUT_MASK_ACTION)) {
            break;
        }
    }

    pMMStream->SetState(STREAMSTATE_STOP);

    int i;
    i = pSample->Release();
    ASSERT(i == 0);
    i = pDDStream->Release();
    ASSERT(i == 0);
    i = pPrimaryVidStream->Release();
    ASSERT(i == 0);
}

#include "ddraw.h"    // DirectDraw interfaces
#include "mmstream.h" // Multimedia stream interfaces
#include "amstream.h" // DirectShow multimedia stream interfaces
#include "ddstream.h" // DirectDraw multimedia stream interfaces

void RenderFileToMMStream(const char *szFileName, IMultiMediaStream **ppMMStream, IDirectDraw *pDD) {
    IAMMultiMediaStream *pAMStream = nullptr;

    HRESULT hres;

    CoCreateInstance(
        CLSID_AMMultiMediaStream,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IAMMultiMediaStream,
        (void **) &pAMStream);

    WCHAR wPath[MAX_PATH]; // Wide (32-bit) string name

    MultiByteToWideChar(
        CP_ACP,
        0,
        szFileName,
        -1,
        wPath,
        sizeof(wPath) / sizeof(wPath[0]));

    hres = pAMStream->Initialize(STREAMTYPE_READ, 0, nullptr);
    if (FAILED(hres)) {
        switch (hres) {
        case E_ABORT:
            hres++;
            break;
        case E_INVALIDARG:
            hres++;
            break;
        case E_NOINTERFACE:
            hres++;
            break;
        case E_OUTOFMEMORY:
            hres++;
            break;
        case E_POINTER:
            hres++;
            break;
        }
    }
    hres = pAMStream->AddMediaStream(pDD, &MSPID_PrimaryVideo, 0, nullptr);
    hres = pAMStream->AddMediaStream(nullptr, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, nullptr);
    hres = pAMStream->OpenFile(wPath, 0);

    *ppMMStream = pAMStream;
}

extern char DATA_DIR[];

void InitBackImage(char *name) {
    MFFileHandle image_file;
    std::int32_t height;
    char fname[200];

    sprintf(fname, "%sdata\\%s", DATA_DIR, name);

    if (image_mem == 0) {
        image_mem = (std::uint8_t *) MemAlloc(640 * 480 * 3);
    }

    if (!image_mem)
        return;

    image_file = FileOpen(fname);
    if (image_file != FILE_OPEN_ERROR) {
        FileSeek(image_file, SEEK_MODE_BEGINNING, 18);
        image = image_mem + (640 * 479 * 3);
        for (height = 480; height; height--, image -= (640 * 3)) {
            FileRead(image_file, image, 640 * 3);
        }
        FileClose(image_file);
    }
    the_display.create_background_surface(image_mem);
}

void UseBackSurface(LPDIRECTDRAWSURFACE4 use) {
    the_display.use_this_background_surface(use);
}

LPDIRECTDRAWSURFACE4 m_lpLastBackground = nullptr;

void ResetBackImage() {
    the_display.destroy_background_surface();
    if (image_mem) {
        MemFree(image_mem);
        image_mem = 0;
    }
}

void ShowBackImage(bool b3DInFrame) {
    the_display.blit_background_surface(b3DInFrame);
}

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags) {
    HRESULT result;

    result = the_manager.Init();
    if (FAILED(result)) {
        return -1;
    }

    extern HINSTANCE hGlobalThisInst;

    VideoRes = ENV_get_value_number("video_res", -1, "Render");
    VideoRes = max(min(VideoRes, 4), 0);

    depth = 32;
    switch (VideoRes) {
    case 0:
        width = 320;
        height = 240;
        break;
    case 1:
        width = 512;
        height = 384;
        break;
    case 2:
        width = 640;
        height = 480;
        break;
    case 3:
        width = 800;
        height = 600;
        break;
    case 4:
        width = 1024;
        height = 768;
        break;
    default:
        width = 1280;
        height = 720;
        break;
        // case 5:		width = 1920; height = 1080; break;		// TODO: Investigate modern screen resolutions
    }

    if (flags & FLAGS_USE_3D)
        the_display.Use3DOn();

    if (flags & FLAGS_USE_WORKSCREEN)
        the_display.UseWorkOn();

    the_display.FullScreenOff();

    result = SetDisplay(width, height, depth);

    return result;
}

std::int32_t CloseDisplay() {
    the_display.Fini();
    the_manager.Fini();

    return 1;
}

std::int32_t SetDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth) {
    HRESULT result;

    RealDisplayWidth = width;
    RealDisplayHeight = height;
    DisplayBPP = depth;
    result = the_display.ChangeMode(width, height, depth, 0);
    if (FAILED(result))
        return -1;

    // tell the polygon engine
    PolyPage::SetScaling(float(width) / float(DisplayWidth), float(height) / float(DisplayHeight));

    return 0;
}

std::int32_t ClearDisplay(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    DDBLTFX dd_bltfx;

    dd_bltfx.dwSize = sizeof(dd_bltfx);
    dd_bltfx.dwFillColor = 0;

    the_display.lp_DD_FrontSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &dd_bltfx);

    return 0;
}

struct RGB_565 {
    std::uint16_t R : 5,
        G : 6,
        B : 5;
};

struct RGB_555 {
    std::uint16_t R : 5,
        G : 5,
        B : 5;
};

void LoadBackImage(std::uint8_t *image_data) {
    ASSERT(0);

    std::uint16_t pixel,
        *surface_mem;
    std::int32_t try_count,
        height,
        pitch,
        width;
    DDSURFACEDESC2 dd_sd;
    HRESULT result;
    RGB_565 rgb;

    {
        InitStruct(dd_sd);
        try_count = 0;
    do_the_lock:
        result = the_display.lp_DD_BackSurface->Lock(nullptr, &dd_sd, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, nullptr);
        switch (result) {
        case DD_OK:
            pitch = dd_sd.lPitch >> 1;
            surface_mem = (std::uint16_t *) dd_sd.lpSurface;
            for (height = 0; (unsigned) height < dd_sd.dwHeight; height++, surface_mem += pitch) {
                for (width = 0; (unsigned) width < dd_sd.dwWidth; width++) {
                    pixel = the_display.GetFormattedPixel(*(image_data + 2), *(image_data + 1), *(image_data + 0));
                    *(surface_mem + width) = pixel;
                    image_data += 3;
                }
            }
            try_count = 0;
        do_the_unlock:
            result = the_display.lp_DD_BackSurface->Unlock(nullptr);
            switch (result) {
            case DDERR_SURFACELOST:
                try_count++;
                if (try_count < 10) {
                    the_display.Restore();
                    goto do_the_unlock;
                }
                break;
            }
            break;
        case DDERR_SURFACELOST:
            try_count++;
            if (try_count < 10) {
                the_display.Restore();
                goto do_the_lock;
            }
            break;
        default:
            try_count++;
            if (try_count < 10)
                goto do_the_lock;
        }
    }
}

Display::Display() {
    DisplayFlags = 0;
    CurrDevice = nullptr;
    CurrDriver = nullptr;
    CurrMode = nullptr;

    CreateZBufferOn();

    lp_DD_FrontSurface = nullptr;
    lp_DD_BackSurface = nullptr;
    lp_DD_WorkSurface = nullptr;
    lp_DD_ZBuffer = nullptr;
    lp_D3D_Black = nullptr;
    lp_D3D_White = nullptr;
    lp_D3D_User = nullptr;
    lp_DD_GammaControl = nullptr;

    BackColour = BK_COL_BLACK;
    TextureList = nullptr;
}

Display::~Display() {
    Fini();
}

HRESULT Display::Init() {
    HRESULT result;
    static bool run_fmv = false;

    if (!IsInitialised()) {
        if ((!hDDLibWindow) || (!IsWindow(hDDLibWindow))) {
            result = DDERR_GENERIC;
            // Output error.
            return result;
        }

        // Create DD/D3D Interface objects.
        result = InitInterfaces();
        if (FAILED(result))
            goto cleanup;

        // Attach the window to the DD interface.
        result = InitWindow();
        if (FAILED(result))
            goto cleanup;

        // Set the Mode.
        result = InitFullscreenMode();
        if (FAILED(result))
            goto cleanup;

        // Create Front Surface.
        result = InitFront();
        if (FAILED(result))
            goto cleanup;

        // Create Back surface & D3D device.
        result = InitBack();
        if (FAILED(result))
            goto cleanup;

        if (background_image_mem) {
            create_background_surface(background_image_mem);
        } else {
        }

        InitOn();

        // run the FMV
        if (!run_fmv) {
            RunFMV();
            run_fmv = true;
        }

        return DD_OK;

    cleanup:
        Fini();
        return DDERR_GENERIC;
    }
    return DD_OK;
}

//---------------------------------------------------------------

HRESULT Display::Fini() {
    // Cleanup
    toGDI();

    if (lp_DD_Background) {
        lp_DD_Background->Release();
        lp_DD_Background = nullptr;
    }
    FiniBack();
    FiniFront();
    FiniFullscreenMode();
    FiniWindow();
    FiniInterfaces();

    InitOff();
    return DD_OK;
}

HRESULT Display::GenerateDefaults() {
    D3DDeviceInfo *new_device;
    DDDriverInfo *new_driver;
    DDModeInfo *new_mode;
    HRESULT result;

    new_driver = ValidateDriver(nullptr);
    if (!new_driver) {
        // Error, invalid DD Guid
        result = DDERR_GENERIC;
        ;
        DebugText("GenerateDefaults: unable to get default driver\n");
        // Output error.
        return result;
    }

    if (IsFullScreen()) {
        // Get D3D device and compatible mode
        if (
            !GetFullscreenMode(
                new_driver,
                nullptr,
                DEFAULT_WIDTH,
                DEFAULT_HEIGHT,
                DEFAULT_DEPTH,
                0,
                &new_mode,
                &new_device)) {
            result = DDERR_GENERIC;
            DebugText("GenerateDefaults: unable to get default screen mode\n");
            return result;
        }
    } else {
        // Get Desktop mode and compatible D3D device
        if (
            !GetDesktopMode(
                new_driver,
                nullptr,
                &new_mode,
                &new_device)) {
            result = DDERR_GENERIC;
            DebugText("GenrateDefaults: unable to get default screen mode\n");
            return result;
        }
    }

    // Return results
    CurrDriver = new_driver;
    CurrMode = new_mode;
    CurrDevice = new_device;

    // Success.
    return DD_OK;
}

HRESULT Display::InitInterfaces() {
    GUID *the_guid;
    HRESULT result;

    // Do we have a current DD Driver
    if (!CurrDriver) {
        // So, Grab the Primary DD driver.
        CurrDriver = ValidateDriver(nullptr);
        if (!CurrDriver) {
            // Error, No current Driver
            result = DDERR_GENERIC;
            // Output error.
            return result;
        }
    }

    // Get DD Guid.
    the_guid = CurrDriver->GetGuid();

    // Create DD interface
    result = DirectDrawCreate(the_guid, &lp_DD, nullptr);
    if (FAILED(result)) {
        // Error
        // Output error.
        goto cleanup;
    }

    // Get DD4 interface
    result = lp_DD->QueryInterface((REFIID) IID_IDirectDraw4, (void **) &lp_DD4);
    if (FAILED(result)) {
        // Error
        // Output error.

        // Inform User that they Need DX 6.0 installed
        MessageBox(hDDLibWindow, TEXT("Need DirectX 6.0 or greater to run"), TEXT("Error"), MB_OK);
        goto cleanup;
    }

    // Get D3D interface
    result = lp_DD4->QueryInterface((REFIID) IID_IDirect3D3, (void **) &lp_D3D);
    if (FAILED(result)) {
        // Error
        // Output error.
        goto cleanup;
    }

    // Mark this stage as done
    TurnValidInterfaceOn();

    // Success
    return DD_OK;

cleanup:
    // Failure
    FiniInterfaces();

    return result;
}

//---------------------------------------------------------------

HRESULT Display::FiniInterfaces() {
    // Mark this stage as invalid
    TurnValidInterfaceOff();

    // Release Direct3D Interface
    if (lp_D3D) {
        lp_D3D->Release();
        lp_D3D = nullptr;
    }

    // Release DirectDraw4 Interface
    if (lp_DD4) {
        lp_DD4->Release();
        lp_DD4 = nullptr;
    }

    // Release DirectDraw Interface
    if (lp_DD) {
        lp_DD->Release();
        lp_DD = nullptr;
    }

    // Success
    return DD_OK;
}

HRESULT Display::InitWindow() {
    HRESULT result;
    std::int32_t flags;

    // Check Initialization
    if ((!hDDLibWindow) || (!IsWindow(hDDLibWindow))) {
        // Error, we need a valid window to continue
        result = DDERR_GENERIC;
        // Output error.
        return result;
    }

    // Get Cooperative Flags
    if (IsFullScreen()) {
        flags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_FPUSETUP;
    } else {
        flags = DDSCL_NORMAL | DDSCL_FPUSETUP;
    }

    // Set Cooperative Level
    result = lp_DD4->SetCooperativeLevel(hDDLibWindow, flags);
    if (FAILED(result)) {
        // Error
        // Output error.
        return result;
    }

    // init VB
    VB_Init();
    TheVPool->Create(lp_D3D, !CurrDevice->IsHardware());

    // Success
    return DD_OK;
}

HRESULT Display::FiniWindow() {
    HRESULT result;

    VB_Term();

    if (lp_DD4) {
        result = lp_DD4->SetCooperativeLevel(hDDLibWindow, DDSCL_NORMAL | DDSCL_FPUSETUP);
        if (FAILED(result)) {
            // Error
            // Output error.
            return result;
        }
    }

    // Success
    return DD_OK;
}

#define FMV1a "eidos"
#define FMV1b "logo24"
#define FMV2 "pcintro_withsound"
#define FMV3 "new_pccutscene%d_300"

LPDIRECTDRAWSURFACE4 mirror;

static bool quick_flipper() {
    the_display.lp_DD_FrontSurface->Blt(&the_display.DisplayRect, mirror, nullptr, DDBLT_WAIT, nullptr);

    return true;
}

void PlayQuickMovie(std::int32_t type, std::int32_t language_ignored, bool bIgnored) {
    DDSURFACEDESC2 back;
    DDSURFACEDESC2 mine;

    //
    // Must create a 640x480 surface with the same pixel format as the
    // primary surface.
    //

    InitStruct(back);

    the_display.lp_DD_BackSurface->GetSurfaceDesc(&back);

    //
    // Create the mirror surface in system memory.
    //

    InitStruct(mine);

    mine.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    mine.dwWidth = 640;
    mine.dwHeight = 480;
    mine.ddpfPixelFormat = back.ddpfPixelFormat;
    mine.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    HRESULT result = the_display.lp_DD4->CreateSurface(&mine, &mirror, nullptr);

    IDirectDrawSurface *lpdds;

    if (SUCCEEDED(mirror->QueryInterface(IID_IDirectDrawSurface, (void **) &lpdds))) {
        if (!type) {
            BinkPlay("bink\\" FMV1a ".bik", lpdds, quick_flipper);
            BinkPlay("bink\\" FMV1b ".bik", lpdds, quick_flipper);
            BinkPlay("bink\\" FMV2 ".bik", lpdds, quick_flipper);
        } else {
            char filename[MAX_PATH];
            sprintf(filename, "bink\\" FMV3 ".bik", type);
            TRACE("Playing %s\n", filename);
            BinkPlay(filename, lpdds, quick_flipper);
        }
    }

    mirror->Release();
}

void Display::RunFMV() {
    if (!hDDLibWindow) return;

    // should we run it?
    if (!ENV_get_value_number("play_movie", 1, "Movie")) return;

    PlayQuickMovie(0, 0, true);
}

void Display::RunCutscene(int which, int language, bool bAllowButtonsToExit) {
    PlayQuickMovie(which, language, bAllowButtonsToExit);
}

//---------------------------------------------------------------

HRESULT Display::InitFullscreenMode() {
    std::int32_t flags = 0,
                 style,
                 w, h, bpp, refresh;
    HRESULT result;

    // Check Initialization
    if ((!CurrMode) || (!lp_DD4)) {
        // Error, we need a valid mode and DirectDraw 2 interface to proceed
        result = DDERR_GENERIC;
        DebugText("InitFullScreenMode: invalid initialization\n");
        return result;
    }

    // Do window mode setup.
    if (!IsFullScreen()) {
        // Set window style.
        style = GetWindowStyle(hDDLibWindow);
        style &= ~WS_POPUP;
        style |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
        SetWindowLong(hDDLibWindow, GWL_STYLE, style);

        // Save Surface Rectangle.
        DisplayRect.left = 0;
        DisplayRect.top = 0;
        DisplayRect.right = RealDisplayWidth;
        DisplayRect.bottom = RealDisplayHeight;

        AdjustWindowRectEx(
            &DisplayRect,
            GetWindowStyle(hDDLibWindow),
            GetMenu(hDDLibWindow) != nullptr,
            GetWindowExStyle(hDDLibWindow));
        SetWindowPos(
            hDDLibWindow,
            nullptr,
            0, 0,
            DisplayRect.right - DisplayRect.left,
            DisplayRect.bottom - DisplayRect.top,
            SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);

        GetClientRect(hDDLibWindow, &the_display.DisplayRect);
        ClientToScreen(hDDLibWindow, (LPPOINT) &the_display.DisplayRect);
        ClientToScreen(hDDLibWindow, (LPPOINT) &the_display.DisplayRect + 1);

        // Success
        TurnValidFullscreenOn();
        return DD_OK;
    }

    // Get the shell menu & window style.
    hDDLibMenu = GetMenu(hDDLibWindow);
    hDDLibStyle = GetWindowLong(hDDLibWindow, GWL_STYLE);
    hDDLibStyleEx = GetWindowLong(hDDLibWindow, GWL_EXSTYLE);

    // Calculate Mode info
    CurrMode->GetMode(&w, &h, &bpp, &refresh);

    // Special check for mode 320 x 200 x 8
    if ((w == 320) && (h == 200) && (bpp == 8)) {
        // Make sure we use Mode 13 instead of Mode X
        flags = DDSDM_STANDARDVGAMODE;
    }

    // Set Requested Fullscreen mode
    result = lp_DD4->SetDisplayMode(w, h, bpp, refresh, flags);
    if (SUCCEEDED(result)) {
        // Save Surface Rectangle
        DisplayRect.left = 0;
        DisplayRect.top = 0;
        DisplayRect.right = w;
        DisplayRect.bottom = h;

        // Success
        TurnValidFullscreenOn();
        return result;
    }

    DebugText("SetDisplayMode failed (%d x %d x %d), trying (640 x 480 x %d)\n", w, h, bpp, bpp);

    // Don't give up!
    // Try 640 x 480 x bpp mode instead
    if ((w != DEFAULT_WIDTH || h != DEFAULT_HEIGHT)) {
        w = DEFAULT_WIDTH;
        h = DEFAULT_HEIGHT;

        CurrMode = ValidateMode(CurrDriver, w, h, bpp, 0, CurrDevice);
        if (CurrMode) {
            result = lp_DD4->SetDisplayMode(w, h, bpp, 0, 0);
            if (SUCCEEDED(result)) {
                // Save Surface Rectangle
                DisplayRect.left = 0;
                DisplayRect.top = 0;
                DisplayRect.right = w;
                DisplayRect.bottom = h;

                // Success
                TurnValidFullscreenOn();
                return result;
            }
        }
    }

    // Keep trying
    // Try 640 x 480 x 16 mode instead
    if (bpp != DEFAULT_DEPTH) {
        DebugText("SetDisplayMode failed (640 x 480 x %d), trying (640 x 480 x 16)\n", bpp);
        bpp = DEFAULT_DEPTH;

        CurrMode = ValidateMode(CurrDriver, w, h, bpp, 0, CurrDevice);
        if (CurrMode) {
            result = lp_DD4->SetDisplayMode(w, h, bpp, 0, 0);
            if (SUCCEEDED(result)) {
                // Save Surface Rectangle
                DisplayRect.left = 0;
                DisplayRect.top = 0;
                DisplayRect.right = w;
                DisplayRect.bottom = h;

                // Success
                TurnValidFullscreenOn();
                return result;
            }
        }
    }
    // Failure
    // Output error.
    return result;
}

HRESULT Display::FiniFullscreenMode() {
    TurnValidFullscreenOff();

    // Restore original desktop mode
    if (lp_DD4)
        lp_DD4->RestoreDisplayMode();

    // Success
    return DD_OK;
}

//
// Given the bitmask for a colour in a pixel format, it calculates the mask and
// shift so that you can construct a pixel in pixel format given its RGB values.
// The formula is...
//
//	PIXEL(r,g,b) = ((r >> mask) << shift) | ((g >> mask) << shift) | ((b >> mask) << shift);
//
// THIS ASSUMES that r,g,b are 8-bit values.
//

void calculate_mask_and_shift(
    std::uint32_t bitmask,
    std::int32_t *mask,
    std::int32_t *shift) {
    std::int32_t i;
    std::int32_t b;
    std::int32_t num_bits = 0;
    std::int32_t first_bit = -1;

    LogText(" bitmask %x \n", bitmask);

    for (i = 0, b = 1; i < 32; i++, b <<= 1) {
        if (bitmask & b) {
            num_bits += 1;

            if (first_bit == -1) {
                //
                // We have found the first bit.
                //

                first_bit = i;
            }
        }
    }

    if (first_bit == -1 ||
        num_bits == 0) {
        //
        // This is bad!
        //

        LogText(" poo mask shift  first bit %d num_bits %d \n", first_bit, num_bits);
        TRACE("No valid masks and shifts.\n");
        LogText("No valid masks and shifts.\n");

        *mask = 0;
        *shift = 0;

        return;
    }

    *mask = 8 - num_bits;
    *shift = first_bit;

    if (*mask < 0) {
        //
        // More than 8 bits per colour component? May
        // as well support it!
        //

        *shift -= *mask;
        *mask = 0;
    }
}

HRESULT Display::InitFront() {
    DDSURFACEDESC2 dd_sd;
    HRESULT result;

    // Check Initialization
    if ((!CurrMode) || (!lp_DD4)) {
        // Error, Need a valid mode and DD interface to proceed
        result = DDERR_GENERIC;
        // Output error.
        return result;
    }

    // Note:  There is no need to fill in width, height, bpp, etc.
    //        This was taken care of in the SetDisplayMode call.

    // Setup Surfaces caps for a front buffer and back buffer
    InitStruct(dd_sd);

    if (IsFullScreen() && CurrDevice->IsHardware()) {
        //
        // Fullscreen harware.
        //

        dd_sd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;
        dd_sd.dwBackBufferCount = 1;
    } else {
        //
        // In a window or software mode.
        //

        dd_sd.dwFlags = DDSD_CAPS;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }

    // Create Primary surface
    result = lp_DD4->CreateSurface(&dd_sd, &lp_DD_FrontSurface, nullptr);
    if (FAILED(result)) {
        // Error
        DebugText("InitFront: unable to create front surface result %d\n", result);
        return result;
    }

    // create gamma control
    result = lp_DD_FrontSurface->QueryInterface(IID_IDirectDrawGammaControl, (void **) &lp_DD_GammaControl);
    if (FAILED(result)) {
        lp_DD_GammaControl = nullptr;
        TRACE("No gamma\n");
    } else {
        TRACE("Gamma control OK\n");

        int black, white;

        GetGamma(&black, &white);
        SetGamma(black, white);
    }

    // Mark as Valid
    TurnValidFrontOn();

    //
    // We need to work out the pixel format of the front buffer.
    //

    InitStruct(dd_sd);

    lp_DD_FrontSurface->GetSurfaceDesc(&dd_sd);

    //
    // It must be an RGB mode!
    //

    ASSERT(dd_sd.ddpfPixelFormat.dwFlags & DDPF_RGB);

    //
    // Work out the masks and shifts for each colour component.
    //

    calculate_mask_and_shift(dd_sd.ddpfPixelFormat.dwRBitMask, &mask_red, &shift_red);
    calculate_mask_and_shift(dd_sd.ddpfPixelFormat.dwGBitMask, &mask_green, &shift_green);
    calculate_mask_and_shift(dd_sd.ddpfPixelFormat.dwBBitMask, &mask_blue, &shift_blue);

    LogText(" mask red %x shift red %d \n", mask_red, shift_red);
    LogText(" mask green %x shift green %d \n", mask_green, shift_green);
    LogText(" mask blue %x shift blue %d \n", mask_blue, shift_blue);

    // Success
    return DD_OK;
}

HRESULT Display::FiniFront() {
    // Mark as Invalid
    TurnValidFrontOff();

    // release gamma control
    if (lp_DD_GammaControl) {
        lp_DD_GammaControl->Release();
        lp_DD_GammaControl = nullptr;
    }

    // Release Front Surface Object
    if (lp_DD_FrontSurface) {
        lp_DD_FrontSurface->Release();
        lp_DD_FrontSurface = nullptr;
    }

    // Success
    return DD_OK;
}

HRESULT Display::InitBack() {
    std::int32_t mem_type,
        w, h;
    DDSCAPS2 dd_scaps;
    DDSURFACEDESC2 dd_sd;
    HRESULT result;
    LPD3DDEVICEDESC device_desc;

    // Check Initialization
    if (
        (!hDDLibWindow) || (!IsWindow(hDDLibWindow)) ||
        (!CurrDevice) || (!CurrMode) ||
        (!lp_DD4) || (!lp_D3D) || (!lp_DD_FrontSurface)) {
        // Error, Not initialized properly before calling this method
        result = DDERR_GENERIC;
        DebugText("InitBack: invalid initialisation\n");
        return result;
    }

    // Calculate the width & height.  This is useful for windowed mode & the z buffer.
    w = abs(DisplayRect.right - DisplayRect.left);
    h = abs(DisplayRect.bottom - DisplayRect.top);

    if (IsFullScreen() && CurrDevice->IsHardware()) {
        // Get the back surface from the front surface.
        memset(&dd_scaps, 0, sizeof(dd_scaps));
        dd_scaps.dwCaps = DDSCAPS_BACKBUFFER;
        result = lp_DD_FrontSurface->GetAttachedSurface(&dd_scaps, &lp_DD_BackSurface);
        if (FAILED(result)) {
            DebugText("InitBack: no attached surface\n");
            return result;
        }
    } else {
        // Create the back surface.
        InitStruct(dd_sd);
        dd_sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        dd_sd.dwWidth = w;
        dd_sd.dwHeight = h;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

        if (!CurrDevice->IsHardware()) {
            dd_sd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        }

        result = lp_DD4->CreateSurface(&dd_sd, &lp_DD_BackSurface, nullptr);
        if (FAILED(result)) {
            DebugText("InitBack: unable to create back surface\n");
            return result;
        }
    }

    if (IsUse3D()) {
        // Create and attach Z-buffer (optional)

        // Get D3D Device description
        if (CurrDevice->IsHardware()) {
            // Hardware device - Z buffer on video ram.
            device_desc = &(CurrDevice->d3dHalDesc);
            mem_type = DDSCAPS_VIDEOMEMORY;
        } else {
            // Software device - Z buffer in system ram.
            device_desc = &(CurrDevice->d3dHelDesc);
            mem_type = DDSCAPS_SYSTEMMEMORY;
        }

        // Enumerate all Z formats associated with this D3D device
        result = CurrDevice->LoadZFormats(lp_D3D);
        if (FAILED(result)) {
            // Error, no texture formats
            // Hope we can run OK without textures
            DebugText("InitBack: unable to load Z formats\n");
        }

        // Can we create a Z buffer?
        if (IsCreateZBuffer() && device_desc && device_desc->dwDeviceZBufferBitDepth) {
            // Create the z-buffer.
            InitStruct(dd_sd);
            dd_sd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
            dd_sd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | mem_type;
            dd_sd.dwWidth = w;
            dd_sd.dwHeight = h;
            memcpy(&dd_sd.ddpfPixelFormat, CurrDevice->GetZFormat(), sizeof(DDPIXELFORMAT));
            result = lp_DD4->CreateSurface(&dd_sd, &lp_DD_ZBuffer, nullptr);
            if (FAILED(result)) {
                DebugText("InitBack: unable to create ZBuffer\n");
                dd_error(result);

                // Note: we may be able to continue without a z buffer
                // So don't exit
            } else {
                // Attach Z buffer to back surface.
                result = lp_DD_BackSurface->AddAttachedSurface(lp_DD_ZBuffer);
                if (FAILED(result)) {
                    DebugText("InitBack: unable to attach ZBuffer 1\n");

                    if (lp_DD_ZBuffer) {
                        lp_DD_ZBuffer->Release();
                        lp_DD_ZBuffer = nullptr;
                    }

                    // Note: we may be able to continue without a z buffer
                    // So don't exit
                }
            }
        }

        //	Create the D3D device interface
        result = lp_D3D->CreateDevice(CurrDevice->guid, lp_DD_BackSurface, &lp_D3D_Device, nullptr);
        if (FAILED(result)) {
            DebugText("InitBack: unable to create D3D device\n");
            d3d_error(result);
            return result;
        }

        // Enumerate all Texture formats associated with this D3D device
        result = CurrDevice->LoadFormats(lp_D3D_Device);
        if (FAILED(result)) {
            // Error, no texture formats
            // Hope we can run OK without textures
            DebugText("InitBack: unable to load texture formats\n");
        }

        //	Create the viewport
        result = InitViewport();
        if (FAILED(result)) {
            DebugText("InitBack: unable to init viewport\n");
            return result;
        }

        // check the device caps
        CurrDevice->CheckCaps(lp_D3D_Device);
    }

    if (IsUseWork()) {
        // Create a work screen for user access.

        // Get D3D Device description.  We want a system ram surface regardless of the device type.
        if (CurrDevice->IsHardware()) {
            // Hardware device - Z buffer on video ram.
            device_desc = &(CurrDevice->d3dHalDesc);
        } else {
            // Software device - Z buffer in system ram.
            device_desc = &(CurrDevice->d3dHelDesc);
        }

        // Can we create the surface?
        if (device_desc) {
            // Create the z-buffer.
            InitStruct(dd_sd);
            dd_sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
            dd_sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
            dd_sd.dwWidth = w;
            dd_sd.dwHeight = h;
            result = lp_DD4->CreateSurface(&dd_sd, &lp_DD_WorkSurface, nullptr);
            if (FAILED(result)) {
                DebugText("InitBack: unable to create work surface\n");
                dd_error(result);
                return result;
            }
            WorkScreenPixelWidth = w;
            WorkScreenWidth = w;
            WorkScreenHeight = h;
        }
    }

    // Mark as valid
    TurnValidBackOn();

    // Success
    return DD_OK;
}

HRESULT Display::FiniBack() {
    // Mark as invalid
    TurnValidBackOff();

    // Clean up the work screen stuff.
    if (IsUseWork()) {
        // Release the work surface.
        if (lp_DD_WorkSurface) {
            lp_DD_WorkSurface->Release();
            lp_DD_WorkSurface = nullptr;
        }
    }

    // Clean up the D3D stuff.
    if (IsUse3D()) {
        // Cleanup viewport
        FiniViewport();

        // Release D3D Device
        if (lp_D3D_Device) {
            lp_D3D_Device->Release();
            lp_D3D_Device = nullptr;
        }

        // Release Z Buffer
        if (lp_DD_ZBuffer) {
            // Detach Z-Buffer from back buffer
            if (lp_DD_BackSurface)
                lp_DD_BackSurface->DeleteAttachedSurface(0L, lp_DD_ZBuffer);

            // Release Z-Buffer
            lp_DD_ZBuffer->Release();
            lp_DD_ZBuffer = nullptr;
        }
    }

    // Release back surface.
    if (lp_DD_BackSurface) {
        lp_DD_BackSurface->Release();
        lp_DD_BackSurface = nullptr;
    }

    // Success
    return DD_OK;
}

HRESULT Display::InitViewport() {
    D3DMATERIAL material;
    HRESULT result;

    // Check Initialization
    if ((!lp_D3D) || (!lp_D3D_Device)) {
        // Error, Not properly initialized before calling this method
        result = DDERR_GENERIC;
        // Output error.
        return result;
    }

    // Create Viewport
    result = lp_D3D->CreateViewport(&lp_D3D_Viewport, nullptr);
    if (FAILED(result)) {
        // Output error.
        return result;
    }

    // Attach viewport to D3D device
    result = lp_D3D_Device->AddViewport(lp_D3D_Viewport);
    if (FAILED(result)) {
        lp_D3D_Viewport->Release();
        lp_D3D_Viewport = nullptr;

        // Output error.
        return result;
    }

    // Black material.
    result = lp_D3D->CreateMaterial(&lp_D3D_Black, nullptr);
    if (FAILED(result)) {
        DebugText("InitViewport: Error creating black material\n");
        return result;
    }

    InitStruct(material);
    material.diffuse.r = D3DVAL(0.0);
    material.diffuse.g = D3DVAL(0.0);
    material.diffuse.b = D3DVAL(0.0);
    material.diffuse.a = D3DVAL(1.0);
    material.ambient.r = D3DVAL(0.0);
    material.ambient.g = D3DVAL(0.0);
    material.ambient.b = D3DVAL(0.0);
    material.dwRampSize = 0;

    result = lp_D3D_Black->SetMaterial(&material);
    if (FAILED(result)) {
        DebugText("InitViewport: Error setting black material\n");
        lp_D3D_Black->Release();
        lp_D3D_Black = nullptr;
        return result;
    }
    result = lp_D3D_Black->GetHandle(lp_D3D_Device, &black_handle);
    if (FAILED(result)) {
        DebugText("InitViewport: Error getting black handle\n");
        lp_D3D_Black->Release();
        lp_D3D_Black = nullptr;
        return result;
    }

    // White material.
    result = lp_D3D->CreateMaterial(&lp_D3D_White, nullptr);
    if (FAILED(result)) {
        DebugText("InitViewport: Error creating white material\n");
        return result;
    }
    material.diffuse.r = D3DVAL(1.0);
    material.diffuse.g = D3DVAL(1.0);
    material.diffuse.b = D3DVAL(1.0);
    material.diffuse.a = D3DVAL(1.0);
    material.ambient.r = D3DVAL(1.0);
    material.ambient.g = D3DVAL(1.0);
    material.ambient.b = D3DVAL(1.0);
    material.dwRampSize = 0;

    result = lp_D3D_White->SetMaterial(&material);
    if (FAILED(result)) {
        DebugText("InitViewport: Error setting white material\n");
        lp_D3D_White->Release();
        lp_D3D_White = nullptr;
        return result;
    }
    result = lp_D3D_White->GetHandle(lp_D3D_Device, &white_handle);
    if (FAILED(result)) {
        DebugText("InitViewport: Error getting white handle\n");
        lp_D3D_White->Release();
        lp_D3D_White = nullptr;
        return result;
    }

    //
    // User material.
    //

    SetUserColour(255, 150, 255);

    // Set up Initial Viewport parameters
    result = UpdateViewport();
    if (FAILED(result)) {
        lp_D3D_Device->DeleteViewport(lp_D3D_Viewport);
        lp_D3D_Viewport->Release();
        lp_D3D_Viewport = nullptr;

        return result;
    }

    // Mark as valid
    TurnValidViewportOn();

    /// Success
    return DD_OK;
}

void Display::SetUserColour(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    D3DMATERIAL material;
    HRESULT result;

    float r = (1.0F / 255.0F) * float(red);
    float g = (1.0F / 255.0F) * float(green);
    float b = (1.0F / 255.0F) * float(blue);

    if (lp_D3D_User) {
        lp_D3D_User->Release();
        lp_D3D_User = nullptr;
        user_handle = 0;
    }

    result = lp_D3D->CreateMaterial(&lp_D3D_User, nullptr);

    ASSERT(!FAILED(result));

    InitStruct(material);
    material.diffuse.r = D3DVAL(r);
    material.diffuse.g = D3DVAL(g);
    material.diffuse.b = D3DVAL(b);
    material.diffuse.a = D3DVAL(1.0);
    material.ambient.r = D3DVAL(r);
    material.ambient.g = D3DVAL(g);
    material.ambient.b = D3DVAL(b);
    material.dwRampSize = 0;

    result = lp_D3D_User->SetMaterial(&material);

    ASSERT(!FAILED(result));

    result = lp_D3D_User->GetHandle(lp_D3D_Device, &user_handle);

    ASSERT(!FAILED(result));
}

HRESULT Display::FiniViewport() {
    // Mark as invalid
    TurnValidViewportOff();

    // Get rid of any loaded texture maps.
    FreeLoadedTextures();

    // Release materials.
    if (lp_D3D_Black) {
        lp_D3D_Black->Release();
        lp_D3D_Black = nullptr;
        black_handle = 0;
    }

    if (lp_D3D_White) {
        lp_D3D_White->Release();
        lp_D3D_White = nullptr;
        white_handle = 0;
    }

    if (lp_D3D_User) {
        lp_D3D_User->Release();
        lp_D3D_User = nullptr;
        user_handle = 0;
    }

    // Release D3D viewport
    if (lp_D3D_Viewport) {
        lp_D3D_Device->DeleteViewport(lp_D3D_Viewport);
        lp_D3D_Viewport->Release();
        lp_D3D_Viewport = nullptr;
    }

    // Success
    return DD_OK;
}

HRESULT Display::UpdateViewport() {
    std::int32_t s_w, s_h;
    HRESULT result;
    D3DVIEWPORT2 d3d_viewport;

    // Check Parameters
    if ((!lp_D3D_Device) || (!lp_D3D_Viewport)) {
        // Not properly initialized before calling this method
        result = DDERR_GENERIC;
        // Output error.
        return result;
    }

    // Get Surface Width and Height
    s_w = abs(DisplayRect.right - DisplayRect.left);
    s_h = abs(DisplayRect.bottom - DisplayRect.top);

    // Update Viewport
    InitStruct(d3d_viewport);
    d3d_viewport.dwX = 0;
    d3d_viewport.dwY = 0;
    d3d_viewport.dwWidth = s_w;
    d3d_viewport.dwHeight = s_h;
    d3d_viewport.dvClipX = 0.0f;
    d3d_viewport.dvClipY = 0.0f;
    d3d_viewport.dvClipWidth = (float) s_w;
    d3d_viewport.dvClipHeight = (float) s_h;
    d3d_viewport.dvMinZ = 1.0f;
    d3d_viewport.dvMaxZ = 0.0f;

    // Update Viewport
    result = lp_D3D_Viewport->SetViewport2(&d3d_viewport);
    if (FAILED(result)) {
        // Output error.
        return result;
    }

    // Update D3D device to use this viewport
    result = lp_D3D_Device->SetCurrentViewport(lp_D3D_Viewport);
    if (FAILED(result)) {
        // Output error.
        return result;
    }

    // Reload any pre-loaded textures.
    ReloadTextures();

    // Set the view port rect.
    ViewportRect.x1 = 0;
    ViewportRect.y1 = 0;
    ViewportRect.x2 = s_w;
    ViewportRect.y2 = s_h;

    // Set the background colour.
    switch (the_display.BackColour) {
    case BK_COL_NONE:
        break;
    case BK_COL_BLACK: return the_display.SetBlackBackground();
    case BK_COL_WHITE: return the_display.SetWhiteBackground();
    case BK_COL_USER: return the_display.SetUserBackground();
    }

    // Success
    return DD_OK;
}

HRESULT Display::ChangeMode(
    std::int32_t w,
    std::int32_t h,
    std::int32_t bpp,
    std::int32_t refresh) {
    HRESULT result;
    DDDriverInfo *old_driver;
    DDModeInfo *new_mode,
        *old_mode;
    D3DDeviceInfo *new_device,
        *next_best_device,
        *old_device;

    // Check Initialization
    if ((!hDDLibWindow) || (!IsWindow(hDDLibWindow))) {
        result = DDERR_GENERIC;
        DebugText("ChangeMode: main window not initialised\n");
        return result;
    }

    if (!IsInitialised()) {
        result = GenerateDefaults();
        if (FAILED(result)) {
            result = DDERR_GENERIC;
            // Output error.
            return result;
        }

        result = Init();
        if (FAILED(result)) {
            result = DDERR_GENERIC;
            // Output error.
            return result;
        }
    }

    old_driver = CurrDriver;
    old_mode = CurrMode;
    old_device = CurrDevice;

    //
    // Step 1. Get New Mode
    //
    // Find new mode corresponding to w, h, bpp
    new_mode = old_driver->FindMode(w, h, bpp, 0, nullptr);
    if (!new_mode) {
        result = DDERR_GENERIC;
        DebugText("ChangeMode: mode not available with this driver\n");
        return result;
    }

    //
    // Step 2.   Check if Device needs to be changed as well
    //
    if (new_mode->ModeSupported(old_device)) {
        new_device = nullptr;
    } else {
        new_device = old_driver->FindDeviceSupportsMode(&old_device->guid, new_mode, &next_best_device);
        if (!new_device) {
            if (!next_best_device) {
                // No D3D device is compatible with this new mode
                result = DDERR_GENERIC;
                DebugText("ChangeMode: No device is compatible with this mode\n");
                return result;
            }
            new_device = next_best_device;
        }
    }

    //
    // Step 3.	Destroy current Mode
    //
    FiniBack();
    FiniFront();
    //  FiniFullscreenMode ();		// Don't do this => unnecessary mode switch

    //
    // Step 4.  Create new mode
    //
    CurrMode = new_mode;
    if (new_device)
        CurrDevice = new_device;

    // Change Mode
    result = InitFullscreenMode();
    if (FAILED(result))
        return result;

    // Create Primary Surface
    result = InitFront();
    if (FAILED(result)) {
        DebugText("ChangeMode: Error in InitFront\n");

        // Try to restore old mode
        CurrMode = old_mode;
        CurrDevice = old_device;

        InitFullscreenMode();
        InitFront();
        InitBack();

        return result;
    }

    // Create Render surface
    result = InitBack();
    if (FAILED(result)) {
        DebugText("ChangeMode: Error in InitBack\n");

        FiniFront();

        // Try to restore old mode
        CurrMode = old_mode;
        CurrDevice = old_device;

        InitFullscreenMode();
        InitFront();
        InitBack();

        return result;
    }

    //
    // Reload the background image.
    //

    if (background_image_mem) {
        create_background_surface(background_image_mem);
    }

    // Notify a display change.
    DisplayChangedOn();

    // Success
    return DD_OK;
}

HRESULT Display::Restore() {
    HRESULT result;

    // Check Initialization
    if (!IsValid()) {
        result = DDERR_GENERIC;
        DebugText("Restore: invalid initialisation\n");
        return result;
    }

    // Restore Primary Surface
    if (lp_DD_FrontSurface) {
        result = lp_DD_FrontSurface->IsLost();
        if (FAILED(result)) {
            result = lp_DD_FrontSurface->Restore();
            if (FAILED(result))
                return result;
        }
    }

    // Restore Z Buffer
    if (lp_DD_ZBuffer) {
        result = lp_DD_ZBuffer->IsLost();
        if (FAILED(result)) {
            result = lp_DD_ZBuffer->Restore();
            if (FAILED(result))
                return result;
        }
    }

    // Restore Rendering surface
    if (lp_DD_BackSurface) {
        result = lp_DD_BackSurface->IsLost();
        if (FAILED(result)) {
            result = lp_DD_BackSurface->Restore();
            if (FAILED(result))
                return result;
        }
    }

    // Success
    return DD_OK;
}

//---------------------------------------------------------------

HRESULT Display::AddLoadedTexture(D3DTexture *the_texture) {
#ifdef DEBUG
    // Check that this isn't a circular list and that this texture isn't already loaded.
    D3DTexture *t = TextureList;
    int iCountdown = 10000;
    while (t != nullptr) {
        ASSERT(t != the_texture);
        t = t->NextTexture;
        iCountdown--;
        ASSERT(iCountdown > 0);
    }

#endif

    the_texture->NextTexture = TextureList;
    TextureList = the_texture;

    return DD_OK;
}

void Display::RemoveAllLoadedTextures() {
    TextureList = nullptr;
}

HRESULT Display::FreeLoadedTextures() {
    D3DTexture *current_texture;

    int iCountdown = 10000;

    current_texture = TextureList;
    while (current_texture) {
        D3DTexture *next_texture = current_texture->NextTexture;
        current_texture->Destroy();
        current_texture = next_texture;
        iCountdown--;
        if (iCountdown == 0) {
            // Oh dear - not good.
            ASSERT(false);
            break;
        }
    }

    return DD_OK;
}

static char clumpfile[MAX_PATH] = "";
static size_t clumpsize = 0;

void SetLastClumpfile(char *file, size_t size) {
    strcpy(clumpfile, file);
    clumpsize = size;
}

HRESULT Display::ReloadTextures() {
    D3DTexture *current_texture;

    if (clumpfile[0]) {
        OpenTGAClump(clumpfile, clumpsize, true);
    }

    current_texture = TextureList;
    while (current_texture) {
        D3DTexture *next_texture = current_texture->NextTexture;
        current_texture->Reload();
        current_texture = next_texture;
    }

    if (clumpfile[0]) {
        CloseTGAClump();
    }

    return DD_OK;
}

HRESULT Display::toGDI() {
    HRESULT result;

    // Flip to GDI Surface
    if (lp_DD4) {
        result = lp_DD4->FlipToGDISurface();
        if (FAILED(result)) {
            // Output error.
            return result;
        }
    }

    // Force window to redraw itself (on GDI surface).
    if ((hDDLibWindow) && (IsWindow(hDDLibWindow))) {
        DrawMenuBar(hDDLibWindow);
        RedrawWindow(hDDLibWindow, nullptr, nullptr, RDW_FRAME);
    }

    // Success
    return DD_OK;
}

HRESULT Display::fromGDI() {
    // Success
    return DD_OK;
}

HRESULT Display::ShowWorkScreen() {
    return lp_DD_FrontSurface->Blt(&DisplayRect, lp_DD_WorkSurface, nullptr, DDBLT_WAIT, nullptr);
}

void *Display::screen_lock() {
    if (DisplayFlags & DISPLAY_LOCKED) {
        //
        // Don't do anything if you try to lock the screen twice in a row.
        //

        TRACE("Locking the screen when it is already locked!\n");
    } else {
        DDSURFACEDESC2 ddsdesc;
        HRESULT ret;

        InitStruct(ddsdesc);

        ret = lp_DD_BackSurface->Lock(nullptr, &ddsdesc, DDLOCK_WAIT | DDLOCK_NOSYSLOCK, nullptr);

        if (SUCCEEDED(ret)) {
            screen_width = ddsdesc.dwWidth;
            screen_height = ddsdesc.dwHeight;
            screen_pitch = ddsdesc.lPitch;
            screen_bbp = ddsdesc.ddpfPixelFormat.dwRGBBitCount;
            screen = (std::uint8_t *) ddsdesc.lpSurface;

            DisplayFlags |= DISPLAY_LOCKED;
        } else {
            d3d_error(ret);

            screen = nullptr;
        }
    }

    return screen;
}

void Display::screen_unlock() {
    if (DisplayFlags & DISPLAY_LOCKED) {
        lp_DD_BackSurface->Unlock(nullptr);
    }

    screen = nullptr;
    DisplayFlags &= ~DISPLAY_LOCKED;
}

void Display::PlotPixel(std::int32_t x, std::int32_t y, std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
    if (DisplayFlags & DISPLAY_LOCKED) {
        if (WITHIN(x, 0, screen_width - 1) &&
            WITHIN(y, 0, screen_height - 1)) {
            if (CurrMode->GetBPP() == 16) {
                std::uint16_t *dest;

                std::uint16_t pixel = GetFormattedPixel(red, green, blue);
                std::int32_t index = x + x + y * screen_pitch;

                dest = (std::uint16_t *) (&(screen[index]));
                dest[0] = pixel;
            } else {
                std::uint32_t *dest;
                std::uint32_t pixel = GetFormattedPixel(red, green, blue);
                std::int32_t index = x * 4 + y * screen_pitch;

                dest = (std::uint32_t *) (screen + index);
                dest[0] = pixel;
            }
        }
    } else {
        //
        // Do nothing if the screen is not locked.
        //

        TRACE("PlotPixel while screen is not locked.\n");
    }
}

void Display::PlotFormattedPixel(std::int32_t x, std::int32_t y, std::uint32_t colour) {
    if (DisplayFlags & DISPLAY_LOCKED) {
        if (WITHIN(x, 0, screen_width - 1) &&
            WITHIN(y, 0, screen_height - 1)) {
            if (CurrMode->GetBPP() == 16) {
                std::uint16_t *dest;
                std::int32_t index = x + x + y * screen_pitch;

                dest = (std::uint16_t *) (&(screen[index]));
                dest[0] = colour;
            } else {
                std::uint32_t *dest;
                std::int32_t index = x * 4 + y * screen_pitch;
                dest = (std::uint32_t *) (screen + index);
                dest[0] = colour;
            }
        }
    } else {
        //
        // Do nothing if the screen is not locked.
        //

        TRACE("PlotFormattedPixel while screen is not locked.\n");
    }
}

void Display::GetPixel(std::int32_t x, std::int32_t y, std::uint8_t *red, std::uint8_t *green, std::uint8_t *blue) {
    std::int32_t index;

    std::uint32_t colour;

    *red = 0;
    *green = 0;
    *blue = 0;

    if (DisplayFlags & DISPLAY_LOCKED) {
        if (WITHIN(x, 0, screen_width - 1) &&
            WITHIN(y, 0, screen_height - 1)) {
            if (CurrMode->GetBPP() == 16) {
                std::uint16_t *dest;
                std::int32_t index = x + x + y * screen_pitch;

                dest = (std::uint16_t *) (&(screen[index]));
                colour = dest[0];
            } else {
                std::uint32_t *dest;
                std::int32_t index = 4 * x + y * screen_pitch;
                dest = (std::uint32_t *) (screen + index);
                colour = dest[0];
            }

            *red = ((colour >> shift_red) << mask_red) & 0xff;
            *green = ((colour >> shift_green) << mask_green) & 0xff;
            *blue = ((colour >> shift_blue) << mask_blue) & 0xff;
        }
    }
}

void Display::blit_back_buffer() {
    POINT clientpos;
    RECT dest;

    HRESULT res;

    if (the_display.IsFullScreen()) {
        res = lp_DD_FrontSurface->Blt(nullptr, lp_DD_BackSurface, nullptr, DDBLT_WAIT, 0);
    } else {
        clientpos.x = 0;
        clientpos.y = 0;

        ClientToScreen(
            hDDLibWindow,
            &clientpos);

        GetClientRect(
            hDDLibWindow,
            &dest);

        dest.top += clientpos.y;
        dest.left += clientpos.x;
        dest.right += clientpos.x;
        dest.bottom += clientpos.y;

        res = lp_DD_FrontSurface->Blt(&dest, lp_DD_BackSurface, nullptr, DDBLT_WAIT, 0);
    }

    if (FAILED(res)) {
        dd_error(res);
    }
}

void CopyBackground32(std::uint8_t *image_data, IDirectDrawSurface4 *surface) {
    DDSURFACEDESC2 mine;
    HRESULT res;

    InitStruct(mine);
    res = surface->Lock(nullptr, &mine, DDLOCK_WAIT, nullptr);
    if (FAILED(res)) return;

    std::int32_t pitch = mine.lPitch >> 2;
    std::uint32_t *mem = (std::uint32_t *) mine.lpSurface;
    std::int32_t width;
    std::int32_t height;

    // stretch the image

    std::int32_t sdx = 65536 * 640 / mine.dwWidth;
    std::int32_t sdy = 65536 * 480 / mine.dwHeight;

    std::int32_t lsy = -1;
    std::int32_t sy = 0;
    std::uint32_t *lmem = nullptr;

    for (height = 0; (unsigned) height < mine.dwHeight; height++) {
        std::uint8_t *src = image_data + 640 * 3 * (sy >> 16);

        if ((sy >> 16) == lsy) {
            // repeat line
            memcpy(mem, lmem, mine.dwWidth * 4);
        } else {
            std::int32_t sx = 0;

            for (width = 0; (unsigned) width < mine.dwWidth; width++) {
                std::uint8_t *pp = src + 3 * (sx >> 16);

                mem[width] = the_display.GetFormattedPixel(pp[2], pp[1], pp[0]);

                sx += sdx;
            }
        }

        lmem = mem;
        lsy = sy >> 16;

        mem += pitch;
        sy += sdy;
    }

    surface->Unlock(nullptr);
}

void CopyBackground(std::uint8_t *image_data, IDirectDrawSurface4 *surface) {
    CopyBackground32(image_data, surface);
}

void PANEL_ResetDepthBodge();

HRESULT Display::Flip(LPDIRECTDRAWSURFACE4 alt, std::int32_t flags) {
    extern void PreFlipTT();
    PreFlipTT();

    // Make sure panels and text work.
    PANEL_ResetDepthBodge();

    // Draw the screensaver (if any).
    PANEL_screensaver_draw();

    if (IsFullScreen() && CurrDevice->IsHardware()) {
        return lp_DD_FrontSurface->Flip(alt, flags);
    } else {
        return lp_DD_FrontSurface->Blt(&DisplayRect, lp_DD_BackSurface, nullptr, DDBLT_WAIT, nullptr);
    }
}

void Display::use_this_background_surface(LPDIRECTDRAWSURFACE4 this_one) {
    lp_DD_Background_use_instead = this_one;
}

void Display::create_background_surface(std::uint8_t *image_data) {
    DDSURFACEDESC2 back;
    DDSURFACEDESC2 mine;

    //
    // Remember this if we have to reload.
    //

    background_image_mem = image_data;

    //
    // Incase we already have one!
    //

    if (lp_DD_Background) {
        lp_DD_Background->Release();
        lp_DD_Background = nullptr;
    }

    lp_DD_Background_use_instead = nullptr;

    //
    // Create a mirror surface to the back buffer.
    //

    InitStruct(back);

    lp_DD_BackSurface->GetSurfaceDesc(&back);

    //
    // Create the mirror surface in system memory.
    //

    InitStruct(mine);

    mine.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    mine.dwWidth = back.dwWidth;
    mine.dwHeight = back.dwHeight;
    mine.ddpfPixelFormat = back.ddpfPixelFormat;
    mine.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

    HRESULT result = lp_DD4->CreateSurface(&mine, &lp_DD_Background, nullptr);

    if (FAILED(result)) {
        lp_DD_Background = nullptr;
        background_image_mem = nullptr;
        return;
    }

    //
    // Copy the image into the surface...
    //

    CopyBackground(image_data, lp_DD_Background);

    return;
}

void Display::blit_background_surface(bool b3DInFrame) {
    LPDIRECTDRAWSURFACE4 lpBG = nullptr;

    if (lp_DD_Background_use_instead != nullptr) {
        lpBG = lp_DD_Background_use_instead;
    } else if (lp_DD_Background != nullptr) {
        lpBG = lp_DD_Background;
    } else {
        return;
    }

    HRESULT result;

    {
        result = lp_DD_BackSurface->Blt(nullptr, lpBG, nullptr, DDBLT_WAIT, 0);

        if (FAILED(result)) {
            dd_error(result);
        }
    }
}

void Display::destroy_background_surface() {
    if (lp_DD_Background) {
        lp_DD_Background->Release();
        lp_DD_Background = nullptr;
    }

    background_image_mem = nullptr;
}

bool Display::IsGammaAvailable() {
    return (lp_DD_GammaControl != nullptr);
}

// note: 0,256 is normal - white is *exclusive*

void Display::SetGamma(int black, int white) {
    if (!lp_DD_GammaControl) return;

    if (black < 0) black = 0;
    if (white > 256) white = 256;
    if (black > white - 1) black = white - 1;

    ENV_set_value_number("BlackPoint", black, "Gamma");
    ENV_set_value_number("WhitePoint", white, "Gamma");

    DDGAMMARAMP ramp;
    int diff = white - black;

    black <<= 8;

    for (int ii = 0; ii < 256; ii++) {
        ramp.red[ii] = black;
        ramp.green[ii] = black;
        ramp.blue[ii] = black;

        black += diff;
    }

    lp_DD_GammaControl->SetGammaRamp(0, &ramp);
}

void Display::GetGamma(int *black, int *white) {
    *black = ENV_get_value_number("BlackPoint", 0, "Gamma");
    *white = ENV_get_value_number("WhitePoint", 256, "Gamma");
}
