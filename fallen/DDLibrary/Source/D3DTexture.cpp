// D3DTexture.cpp
// Guy Simmons, 29th November 1997.

#include "DDLib.h"
#include "tga.h"

#ifdef TARGET_DC
#include "target.h"
#endif

#ifndef VERIFY
#ifdef NDEBUG
#define VERIFY(x) x
#else
#define VERIFY(x)  \
    {              \
        ASSERT(x); \
    }
#endif
#endif

#ifdef TEX_EMBED
static D3DTexture *EmbedSource = nullptr;
static LPDIRECTDRAWSURFACE4 EmbedSurface = nullptr;
static LPDIRECT3DTEXTURE2 EmbedTexture = nullptr;
static std::uint8_t EmbedOffset = 0;
#endif

extern void POLY_reset_render_states();

static DWORD dwSizeOfFastLoadBuffer = 0;
void *pvFastLoadBuffer = nullptr;

inline void *GetMeAFastLoadBufferAtLeastThisBigPlease(DWORD dwSize) {
    if (dwSizeOfFastLoadBuffer < dwSize) {
        if (pvFastLoadBuffer) {
            VirtualFree(pvFastLoadBuffer, 0, MEM_RELEASE);
        }
        // Grow slightly more than needed to prevent hammering.
        dwSizeOfFastLoadBuffer = (dwSize * 5 / 4 + 1024);
        // Ensure it's 4k-aligned.
        dwSizeOfFastLoadBuffer = ((dwSizeOfFastLoadBuffer + 4095) & ~4095);

        TRACE("Growing FastLoad buffer to 0x%x bytes\n", dwSizeOfFastLoadBuffer);

        pvFastLoadBuffer = VirtualAlloc(nullptr, dwSizeOfFastLoadBuffer, MEM_COMMIT, PAGE_READWRITE);
        ASSERT(pvFastLoadBuffer != nullptr);
    }
    return (pvFastLoadBuffer);
}

void NotGoingToLoadTexturesForAWhileNowSoYouCanCleanUpABit() {
    if (!pvFastLoadBuffer)
        return;

    TRACE("Freeing FastLoad buffer\n");
    VirtualFree(pvFastLoadBuffer, 0, MEM_RELEASE);
    pvFastLoadBuffer = nullptr;
    dwSizeOfFastLoadBuffer = 0;
}

inline void *FastLoadFileSomewhere(MFFileHandle handle, DWORD dwSize) {
    void *pvData = GetMeAFastLoadBufferAtLeastThisBigPlease(dwSize);
    ASSERT(pvData != nullptr);

    DWORD dwAlignedFileSize = dwSize & (~4095);
    // DMA read
    DWORD dwRead = 0;
    if (dwAlignedFileSize > 0) {
        dwRead = FileRead(handle, pvData, dwAlignedFileSize);
    }
    // Finish off with PIO or whatever.
    if (dwSize - dwAlignedFileSize > 0) {
        dwRead += FileRead(handle, (void *) ((char *) pvData + dwAlignedFileSize), dwSize - dwAlignedFileSize);
    }

    ASSERT(dwRead == dwSize);

    return (pvData);
}

static bool m_bTexturePagesInitialised = false;

void FreeAllD3DPages() {
    // And redo all the render states and sorting.
    POLY_reset_render_states();
}

void D3DTexture::BeginLoading() {
    std::int32_t first_time = true;

#ifdef TEX_EMBED
    EmbedSource = nullptr;
    EmbedSurface = nullptr;
    EmbedTexture = nullptr;
    EmbedOffset = 0;
#endif

    // And redo all the render states and sorting.
    POLY_reset_render_states();
}

void D3DPage::EnsureLoaded() {
    if (this->pTex != nullptr) {
        // Cool - already done.
        return;
    }

    // OK, we need to load this up.
    this->pTex = MFnew<D3DTexture>();
    ASSERT(this->pTex != nullptr);

    HRESULT hres = this->pTex->LoadTextureTGA(this->pcFilename, -1, true);
    if (FAILED(hres)) {
        this->pTex = nullptr;
    }
}

void D3DPage::Unload() {
    if (this->pTex != nullptr) {
        ASSERT(pTex != nullptr);
        pTex->Destroy();
        MFdelete(pTex);
        pTex = nullptr;
    }
}

#ifdef TEX_EMBED
void D3DTexture::GetTexOffsetAndScale(float *pfUScale, float *pfUOffset, float *pfVScale, float *pfVOffset) {
    switch (bPageType) {
    case D3DPAGE_NONE:
        *pfUScale = 1.0f;
        *pfVScale = 1.0f;
        *pfUOffset = 0.0f;
        *pfVOffset = 0.0f;
        break;
    case D3DPAGE_64_3X3:
    case D3DPAGE_32_3X3:
        // Arranged with 32-pixel gaps between textures, and
        // the textures are right up against the edge.
        // So along the edge, you have 64 texels, 32 pagging, 64 texels, 32 padding, 64 texels.
        // So the offsets are 0.0, 0.375, 0.75
        *pfUScale = 0.25f;
        *pfVScale = 0.25f;
        *pfUOffset = 0.375f * (float) (bPagePos % 3);
        *pfVOffset = 0.375f * (float) (bPagePos / 3);
        break;
    case D3DPAGE_64_4X4:
    case D3DPAGE_32_4X4:
        // Edge-to-edge packing.
        *pfUScale = 0.25f;
        *pfVScale = 0.25f;
        *pfUOffset = 0.25f * (float) (bPagePos & 0x3);
        *pfVOffset = 0.25f * (float) (bPagePos >> 2);
        break;
    default:
        ASSERT(false);
        break;
    }
}
#endif // #ifdef TEX_EMBED

HRESULT D3DTexture::ChangeTextureTGA(char *tga_file) {
    if (Type != D3DTEXTURE_TYPE_UNUSED) {
        //
        // There must be one already loaded.
        //
        Destroy();
        strcpy(texture_name, tga_file);
        Reload();

        return DD_OK;
    }
    return DDERR_GENERIC;
}

HRESULT D3DTexture::LoadTextureTGA(char *tga_file, std::uint32_t id, bool bCanShrink) {
    HRESULT result;

    if (Type != D3DTEXTURE_TYPE_UNUSED) {
        //
        // Already loaded.
        //

        return DD_OK;
    }

    lp_Texture = nullptr;
    lp_Surface = nullptr;

    this->bCanShrink = bCanShrink;

    // Check parameters.
    if (!tga_file) {
        // Invalid parameters.
        return DDERR_GENERIC;
    }

    strcpy(texture_name, tga_file);
    ID = id;

    Type = D3DTEXTURE_TYPE_TGA;

    result = Reload();

    if (FAILED(result)) {
        DebugText("LoadTextureTGA: unable to load texture\n");
        return (result);
    }

    //
    // Finally let the display driver know about this texture page.
    //

    the_display.AddLoadedTexture(this);

    return DD_OK;
}

HRESULT D3DTexture::CreateUserPage(std::int32_t texture_size, bool i_want_an_alpha_channel) {
    HRESULT result;

    ASSERT(Type == D3DTEXTURE_TYPE_UNUSED);

    lp_Texture = nullptr;
    lp_Surface = nullptr;
    UserWantsAlpha = i_want_an_alpha_channel;

    //
    // A user page.
    //

    size = texture_size;

    //
    // Reload it... or rather, re-create it, or even create it in the first place!
    //

    Type = D3DTEXTURE_TYPE_USER;

    result = Reload();

    if (FAILED(result)) {
        DebugText("Could not create user page.\n");
    }

    //
    // Let the display driver know about this texture page.
    //

    the_display.AddLoadedTexture(this);

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

void OS_calculate_mask_and_shift(
    std::uint32_t bitmask,
    std::int32_t *mask,
    std::int32_t *shift) {
    std::int32_t i;
    std::int32_t b;
    std::int32_t num_bits = 0;
    std::int32_t first_bit = -1;

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

    ASSERT(first_bit != -1 && num_bits != 0);

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

HRESULT D3DTexture::Reload_TGA() {
    D3DDeviceInfo *current_device;

    DDModeInfo *mi;

    // std::int32_t bpp;

    DDSURFACEDESC2 dd_sd;

    TRACE("Tex<%s>\n", texture_name);

    TGA_Info ti;
    TGA_Pixel *tga;

    // HRESULT result;

    //
    // Allocate memory for the texture.
    //

    tga = (TGA_Pixel *) MemAlloc(256 * 256 * sizeof(TGA_Pixel));

    if (!tga) {
        TRACE("Not enough MAIN memory to load tga %s\n", texture_name);

        return DDERR_GENERIC;
    }

    //
    // Load the texture.
    //
    ti = TGA_load(
        texture_name,
        256,
        256,
        tga,
        ID,
        bCanShrink);

    if (!ti.valid) {
        //
        // Invalid tga.
        //

        TRACE("TGA %s is invalid\n", texture_name);
        // ASSERT ( false );
        MemFree(tga);
        return DDERR_GENERIC;
    }

    if (ti.width != ti.height) {
        TRACE("TGA %s is not square\n", texture_name);
        MemFree(tga);
        return DDERR_GENERIC;
    }

    if ((ti.width & (ti.width - 1)) != 0) {
        TRACE("TGA %s is not a valid size", texture_name);
        MemFree(tga);
        return DDERR_GENERIC;
    }

    size = ti.width;

    //
    // Get the current device.
    //

    current_device = the_display.GetDeviceInfo();

    if (!current_device) {
        TRACE("No device!\n");

        return DDERR_GENERIC;
    }

    TRACE("texture = %s\n", this->texture_name);

    //
    // Does this texture page contain alpha?
    //

    ContainsAlpha = ti.contains_alpha;

    //
    // Find the best texture format.
    //

    if (ContainsAlpha) {
        mi = current_device->AlphaTexFmt;
    } else {
        mi = current_device->OpaqueTexFmt;
    }

    //
    // Use the best texture format.
    //

    std::int32_t dwMaskR, dwMaskG, dwMaskB, dwMaskA;
    std::int32_t dwShiftR, dwShiftG, dwShiftB, dwShiftA;

    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRBitMask, &dwMaskR, &dwShiftR);
    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwGBitMask, &dwMaskG, &dwShiftG);
    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwBBitMask, &dwMaskB, &dwShiftB);

    if (ContainsAlpha) {
        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask, &dwMaskA, &dwShiftA);
    }
    mask_red = (std::uint8_t) dwMaskR;
    mask_green = (std::uint8_t) dwMaskG;
    mask_blue = (std::uint8_t) dwMaskB;
    mask_alpha = (std::uint8_t) dwMaskA;
    shift_red = (std::uint8_t) dwShiftR;
    shift_green = (std::uint8_t) dwShiftG;
    shift_blue = (std::uint8_t) dwShiftB;
    shift_alpha = (std::uint8_t) dwShiftA;

    //
    // Get rid of the old texture stuff.
    //

    Destroy();

    //	Guy.	Do all the font mapping stuff here.
    if (IsFont()) {
        CreateFonts(&ti, tga);

        //	Change the outline colour to black.
        std::int32_t size = (ti.width * ti.height);

        while (size--) {
            if (
                (tga + size)->red == 0xff &&
                (tga + size)->green == 0 &&
                (tga + size)->blue == 0xff) {
                (tga + size)->red = 0;
                (tga + size)->green = 0;
                (tga + size)->blue = 0;
            }
        }
    }

    // replace red-only pixels with black
    //
    // WITHOUT AFFECTING BLACK'S ALPHA-CHANNELS. ATF.
    if (IsFont2()) {
        std::int32_t size = (ti.width * ti.height);

        while (size--) {
            if ((tga[size].green == 0) && (tga[size].blue == 0) && (tga[size].red > 128)) {
                tga[size].red = 0;
                tga[size].alpha = 0;
            }
        }
    }

    int interlace;
    int xoff, yoff;

    {
        dd_sd = mi->ddSurfDesc;

        dd_sd.dwSize = sizeof(dd_sd);
        dd_sd.dwFlags =
            DDSD_CAPS |
            DDSD_HEIGHT |
            DDSD_WIDTH |
            DDSD_PIXELFORMAT;
        dd_sd.dwWidth = ti.width;
        dd_sd.dwHeight = ti.height;
        dd_sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
#ifdef TARGET_DC
        dd_sd.ddsCaps.dwCaps2 = 0;
#else
        dd_sd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
#endif
        dd_sd.dwTextureStage = 0;

        VERIFY(SUCCEEDED(the_display.lp_DD4->CreateSurface(&dd_sd, &lp_Surface, nullptr)));
        VERIFY(SUCCEEDED(lp_Surface->QueryInterface(IID_IDirect3DTexture2, (LPVOID *) &lp_Texture)));

        interlace = ti.width;
        xoff = yoff = 0;
    }

    //
    // Lock the surface.
    //

    dd_sd.dwSize = sizeof(dd_sd);
    HRESULT res = lp_Surface->Lock(nullptr, &dd_sd, 0, nullptr);
    ASSERT(SUCCEEDED(res));

    //
    // Copy the texture in
    //
    // ASSUMES 16 or 32-bits PER PIXEL!
    //

    {
        std::uint16_t *wscreenw = (std::uint16_t *) dd_sd.lpSurface;
        std::uint32_t *wscreenl = (std::uint32_t *) dd_sd.lpSurface;

        std::int32_t i;
        std::int32_t j;
        std::uint32_t pixel;

        std::int32_t red;
        std::int32_t green;
        std::int32_t blue;

        std::int32_t bright;

        for (j = 0; j < ti.height; j++)
            for (i = 0; i < ti.width; i++) {
                pixel = 0;

                red = tga[i + j * ti.width].red;
                green = tga[i + j * ti.width].green;
                blue = tga[i + j * ti.width].blue;

                /*

                //
                // Add some gamma!
                //

                red   = 256 - ((256 - red)   * (256 - red)   >> 8);
                green = 256 - ((256 - green) * (256 - green) >> 8);
                blue  = 256 - ((256 - blue)  * (256 - blue)  >> 8);

                if (red   > 255) {red   = 255;}
                if (green > 255) {green = 255;}
                if (blue  > 255) {blue  = 255;}

                */

                if (GreyScale) {
                    bright = (red + green + blue) * 85 >> 8;

                    red = bright;
                    green = bright;
                    blue = bright;
                }

                pixel |= (red >> mask_red) << shift_red;
                pixel |= (green >> mask_green) << shift_green;
                pixel |= (blue >> mask_blue) << shift_blue;

#define ISPIXEL(x, y) (tga[(x) + (y) * ti.width].red | tga[(x) + (y) * ti.width].green | tga[(x) + (y) * ti.width].blue)

                if (ContainsAlpha) {
                    pixel |= (tga[i + j * ti.width].alpha >> mask_alpha) << shift_alpha;

                    if (!pixel && !ISPIXEL(i, j)) {
                        // this is a bit bad ... we want to copy the nearest texel across
                        int i2, j2;

                        if ((i - 1 >= 0) && ISPIXEL(i - 1, j)) {
                            i2 = i - 1;
                            j2 = j;
                        } else if ((i + 1 < ti.width) && ISPIXEL(i + 1, j)) {
                            i2 = i + 1;
                            j2 = j;
                        } else if ((j - 1 >= 0) && ISPIXEL(i, j - 1)) {
                            i2 = i;
                            j2 = j - 1;
                        } else if ((j + 1 < ti.height) && ISPIXEL(i, j + 1)) {
                            i2 = i;
                            j2 = j + 1;
                        } else if ((i - 1 >= 0) && (j - 1 >= 0) && ISPIXEL(i - 1, j - 1)) {
                            i2 = i - 1;
                            j2 = j - 1;
                        } else if ((i - 1 >= 0) && (j + 1 < ti.height) && ISPIXEL(i - 1, j + 1)) {
                            i2 = i - 1;
                            j2 = j + 1;
                        } else if ((i + 1 < ti.width) && (j - 1 >= 0) && ISPIXEL(i + 1, j - 1)) {
                            i2 = i + 1;
                            j2 = j - 1;
                        } else if ((i + 1 < ti.width) && (j + 1 < ti.height) && ISPIXEL(i + 1, j + 1)) {
                            i2 = i + 1;
                            j2 = j + 1;
                        } else {
                            i2 = i;
                            j2 = j;
                        }

                        red = tga[i2 + j2 * ti.width].red;
                        green = tga[i2 + j2 * ti.width].green;
                        blue = tga[i2 + j2 * ti.width].blue;

                        pixel |= (red >> mask_red) << shift_red;
                        pixel |= (green >> mask_green) << shift_green;
                        pixel |= (blue >> mask_blue) << shift_blue;
                    }
                }

                if (dd_sd.ddpfPixelFormat.dwRGBBitCount == 32) {
                    wscreenl[i + xoff + (j + yoff) * interlace] = pixel;
                } else {
                    wscreenw[i + xoff + (j + yoff) * interlace] = (WORD) pixel;
                }
            }
    }

    //
    // Unlock the surface.
    //
    VERIFY(SUCCEEDED(lp_Surface->Unlock(nullptr)));

    MemFree(tga);

    return DD_OK;
}

HRESULT D3DTexture::Reload_user() {
    D3DDeviceInfo *current_device;

    std::int32_t score;
    DDModeInfo *mi;
    std::int32_t best_score;
    DDModeInfo *best_mi;

    // std::int32_t bpp;

    std::int32_t try_shift_alpha;
    std::int32_t try_shift_red;
    std::int32_t try_shift_green;
    std::int32_t try_shift_blue;

    std::int32_t try_mask_alpha;
    std::int32_t try_mask_red;
    std::int32_t try_mask_green;
    std::int32_t try_mask_blue;

    DDSURFACEDESC2 dd_sd;
    HRESULT result;

    //
    // Get the current device.
    //

    current_device = the_display.GetDeviceInfo();

    if (!current_device) {
        TRACE("No device!\n");

        return DDERR_GENERIC;
    }

    best_score = 0;
    best_mi = nullptr;

    if (UserWantsAlpha) {
        //
        // Find the texture format with the most bits of alpha.
        //

        for (mi = current_device->FormatList; mi; mi = mi->Next) {
            if (mi->ddSurfDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) {
                if (mi->ddSurfDesc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {
                    if (mi->ddSurfDesc.ddpfPixelFormat.dwRGBBitCount == 16) {
                        //
                        // Find out how many bits there are for each component.
                        //

                        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask, &try_mask_alpha, &try_shift_alpha);
                        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRBitMask, &try_mask_red, &try_shift_red);
                        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwGBitMask, &try_mask_green, &try_shift_green);
                        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwBBitMask, &try_mask_blue, &try_shift_blue);

                        score = (32 - try_mask_alpha) << 8;
                        score /= mi->ddSurfDesc.ddpfPixelFormat.dwRGBBitCount;

                        if (score > best_score) {
                            best_score = score;
                            best_mi = mi;
                        }
                    }
                }
            }
        }
    } else {
        //
        // Find a 5:6:5 or similar format.
        //

        for (mi = current_device->FormatList; mi; mi = mi->Next) {
            if (mi->ddSurfDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) {
                //
                // True colour...
                //

                if (mi->ddSurfDesc.ddpfPixelFormat.dwRGBBitCount >= 16) {
                    score = 0x100;
                    score -= mi->ddSurfDesc.ddpfPixelFormat.dwRGBBitCount;

                    if (mi->ddSurfDesc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) {
                        //
                        // Knock off score for alpha
                        //

                        score -= 1;
                    }

                    if (score > best_score) {
                        best_score = score;
                        best_mi = mi;
                    }
                }
            }
        }
    }

    if (!best_mi) {
        //
        // Couldn't find a suitable texture format.
        //

        TRACE("Could not find texture format for the user texture page\n");
        return DDERR_GENERIC;
    }

    mi = best_mi;

    std::int32_t dwMaskR, dwMaskG, dwMaskB, dwMaskA;
    std::int32_t dwShiftR, dwShiftG, dwShiftB, dwShiftA;

    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRBitMask, &dwMaskR, &dwShiftR);
    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwGBitMask, &dwMaskG, &dwShiftG);
    OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwBBitMask, &dwMaskB, &dwShiftB);

    if (UserWantsAlpha) {
        OS_calculate_mask_and_shift(mi->ddSurfDesc.ddpfPixelFormat.dwRGBAlphaBitMask, &dwMaskA, &dwShiftA);
    } else {
        dwMaskA = 0;
        dwShiftA = 0;
    }
    mask_red = (std::uint8_t) dwMaskR;
    mask_green = (std::uint8_t) dwMaskG;
    mask_blue = (std::uint8_t) dwMaskB;
    mask_alpha = (std::uint8_t) dwMaskA;
    shift_red = (std::uint8_t) dwShiftR;
    shift_green = (std::uint8_t) dwShiftG;
    shift_blue = (std::uint8_t) dwShiftB;
    shift_alpha = (std::uint8_t) dwShiftA;

    //
    // Get rid of the old texture stuff.
    //

    Destroy();

    //
    // The surface
    //

    dd_sd = mi->ddSurfDesc;

    dd_sd.dwSize = sizeof(dd_sd);
    dd_sd.dwFlags =
        DDSD_CAPS |
        DDSD_HEIGHT |
        DDSD_WIDTH |
        DDSD_PIXELFORMAT;
    dd_sd.dwWidth = size;
    dd_sd.dwHeight = size;
    dd_sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
#ifdef TARGET_DC
    dd_sd.ddsCaps.dwCaps2 = 0;
#else
    dd_sd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
#endif

    VERIFY(SUCCEEDED(the_display.lp_DD4->CreateSurface(&dd_sd, &lp_Surface, nullptr)));

    //
    // Get d3d texture interface.
    //

    result = lp_Surface->QueryInterface(IID_IDirect3DTexture2, (LPVOID *) &lp_Texture);

    if (FAILED(result)) {
        TRACE("ReloadTextureUser: Could not get the texture interface.\n");
        Destroy();
        return DDERR_GENERIC;
    }

    //
    // Success.
    //

    return DD_OK;
}

HRESULT D3DTexture::LockUser(std::uint16_t **bitmap, std::int32_t *pitch) {
    DDSURFACEDESC2 dd_sd;

    //	ASSERT(Type == D3DTEXTURE_TYPE_USER);

    InitStruct(dd_sd);

    if (lp_Surface == nullptr || FAILED(lp_Surface->Lock(nullptr, &dd_sd, DDLOCK_WAIT, nullptr))) {
        return DDERR_GENERIC;
    } else {
        *bitmap = (std::uint16_t *) dd_sd.lpSurface;
        *pitch = dd_sd.lPitch;

        return DD_OK;
    }
}

void D3DTexture::UnlockUser() {
    //	ASSERT(Type == D3DTEXTURE_TYPE_USER);

    VERIFY(SUCCEEDED(lp_Surface->Unlock(nullptr)));
}

HRESULT D3DTexture::Reload() {
    Font *current_font,
        *next_font;
    HRESULT ans;

    //
    // erk ... we have to call the POLY engine from here
    // this hook needs calling when the textures are reloaded
    // en masse, but tracking down each point in the game where
    // this happens is tricky ...
    // so there's a cheeky little call here ...
    //
    POLY_reset_render_states();

    if (IsFont()) {
        current_font = FontList;
        while (current_font) {
            next_font = current_font->NextFont;
            MFdelete(current_font);
            current_font = next_font;
        }
        FontList = nullptr;
    }

    switch (Type) {
    case D3DTEXTURE_TYPE_TGA:
        ans = Reload_TGA();
        break;

    case D3DTEXTURE_TYPE_USER:
        ans = Reload_user();
        break;

    default:
        ASSERT(0);
        break;
    }

    return ans;
}

HRESULT D3DTexture::Destroy() {
    int a = 0, b = 0, c = 0, d = 0;

    Font *current_font,
        *next_font;

    //	Get rid of fonts.
    if (IsFont()) {
        current_font = FontList;
        while (current_font) {
            next_font = current_font->NextFont;
            MFdelete(current_font);
            current_font = next_font;
        }
        FontList = nullptr;
    }

    // Release texture.
    if (lp_Texture) {
        DebugText("Releasing texture\n");
        a = lp_Texture->Release();
        DebugText("Done\n");
        lp_Texture = nullptr;
    }

    // Release surface.
    if (lp_Surface) {
        DebugText("Releasing surface\n");
        c = lp_Surface->Release();
        DebugText("Done\n");
        lp_Surface = nullptr;
    }

    //	TextureHandle = nullptr;

    return DD_OK;
}

#define MATCH_TGA_PIXELS(p1, p2) ((p1)->red == (p2)->red && (p1)->green == (p2)->green && (p1)->blue == (p2)->blue)

bool scan_for_baseline(TGA_Pixel **line_ptr, TGA_Pixel *underline, TGA_Info *info, std::int32_t *y_ptr) {
    while (*y_ptr < info->height) {
        if (MATCH_TGA_PIXELS(*line_ptr, underline)) {
            //	Got the baseline so drop to the next line.
            *y_ptr += 1;
            *line_ptr += info->width;
            return true;
        }

        *y_ptr += 1;
        *line_ptr += info->width;
    }
    return false;
}

HRESULT D3DTexture::CreateFonts(TGA_Info *tga_info, TGA_Pixel *tga_data) {
    std::int32_t current_char,
        char_x, char_y,
        char_height, char_width,
        tallest_char;
    Font *the_font;
    TGA_Pixel underline,
        *current_line,
        *current_pixel;

    //	Scan down the image looking for the underline.
    underline.red = 0xff;
    underline.green = 0x00;
    underline.blue = 0xff;
    current_line = tga_data;
    char_y = 0;
    if (scan_for_baseline(&current_line, &underline, tga_info, &char_y)) {
    map_font:
        //	Found a font baseline so map it.
        the_font = MFnew<Font>();
        if (FontList) {
            the_font->NextFont = FontList;
            FontList = the_font;
        } else {
            the_font->NextFont = nullptr;
            FontList = the_font;
        }

        current_char = 0;
        char_x = 0;
        tallest_char = 1;

        while (current_char < 93) {
            //	Scan across to find the width of char.
            char_width = 0;
            current_pixel = current_line + char_x;
            while (!MATCH_TGA_PIXELS(current_pixel, &underline)) {
                current_pixel++;
                char_width++;

                //	Reached the end of the line.
                if (char_x + char_width >= tga_info->width) {
                    //	Find the next baseline.
                    char_y += tallest_char + 1;
                    if (char_y >= tga_info->height)
                        return DDERR_GENERIC;
                    current_line = tga_data + (char_y * tga_info->width);

                    if (!scan_for_baseline(&current_line, &underline, tga_info, &char_y))
                        return DDERR_GENERIC;

                    char_x = 0;
                    tallest_char = 1;
                    char_width = 0;
                    current_pixel = current_line;
                }
            }

            //	Now scan down to find the height of the char
            char_height = 0;
            current_pixel = current_line + char_x;
            while (!MATCH_TGA_PIXELS(current_pixel, &underline)) {
                current_pixel += tga_info->width;
                char_height++;

                //	Reached the bottom of the page.
                if (char_height >= tga_info->height)
                    return DDERR_GENERIC;
            }

            the_font->CharSet[current_char].X = char_x;
            the_font->CharSet[current_char].Y = char_y;
            the_font->CharSet[current_char].Width = char_width;
            the_font->CharSet[current_char].Height = char_height;

            char_x += char_width + 1;
            if (tallest_char < char_height)
                tallest_char = char_height;

            current_char++;
        }

        //	Find out if there's another font in this file.
        char_y += tallest_char + 1;
        if (char_y >= tga_info->height)
            return DDERR_GENERIC;
        current_line = tga_data + (char_y * tga_info->width);

        if (scan_for_baseline(&current_line, &underline, tga_info, &char_y))
            goto map_font;
    }

    return DD_OK;
}

Font *D3DTexture::GetFont(std::int32_t id) {
    Font *current_font;

    current_font = FontList;
    while (id && current_font) {
        current_font = current_font->NextFont;
        id--;
    }
    return current_font;
}

void D3DTexture::set_greyscale(bool is_greyscale) {
    if (is_greyscale != GreyScale) {
        GreyScale = is_greyscale;

        if (Type != D3DTEXTURE_TYPE_UNUSED) {
            Reload();
        }
    }
}
