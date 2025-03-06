// Display.h
// Guy Simmons, 13th November 1997.

#ifndef DISPLAY_H
#define DISPLAY_H

#include "libgpu.h"
#include "libgte.h"

//---------------------------------------------------------------

#define SHELL_ACTIVE (LibShellActive())
#define SHELL_CHANGED (LibShellChanged())

//---------------------------------------------------------------

#define DEFAULT_WIDTH (320)
#define DEFAULT_HEIGHT (240)

//---------------------------------------------------------------

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags);
std::int32_t CloseDisplay();
std::int32_t SetDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth);
void ShellPaused();
void ShellPauseOn();
void ShellPauseOff();
void DumpBackToTGA(char *tga_name);
void DumpBackToRaw();

//---------------------------------------------------------------

#define DISPLAY_INIT (1 << 0)
#define DISPLAY_PAUSE (1 << 1)
#define DISPLAY_PAUSE_ACK (1 << 2)
#define DISPLAY_LOCKED (1 << 3)

#define BK_COL_NONE 0
#define BK_COL_BLACK 1
#define BK_COL_WHITE 2
#define BK_COL_USER 3

#define OTLEN 10 // 4096 buckets
#define OTSIZE (1 << OTLEN)
#define PANEL_OTZ (OTSIZE - 1)
#define FLARE_OTZ (OTSIZE - 2)

extern std::uint16_t psx_tpages[22];
extern std::uint16_t psx_tpages_clut[16];

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 250

// I've reduced bucket memory to a mear 64K instead of 80K
// I've checked and found this to be a reasonable size given the
// usage on Botanic is only 54836bytes.

// #define BUCKET_MEM	81920
// #define BUCKET_MEM	(72*1024)

typedef struct {
    DRAWENV Draw;                                                 /* drawing environment */
    DISPENV Disp;                                                 /* display environment */
    std::uint32_t *ot;                                            /* ordering table */
    std::uint8_t *PrimMem; /* Bucket Memory Try using one lot. */ //[BUCKET_MEM];
} DB;

typedef struct
{
    std::uint8_t *CurrentPrim;
    DB DisplayBuffers[2]; /* packet double buffer */
    DB *CurrentDisplayBuffer;

    std::uint32_t BackColour,
        PaletteSize;
    RECT DisplayRect; // Current surface rectangle.

    std::int32_t screen_width;
    std::int32_t screen_height;
    std::int32_t screen_pitch;
    std::uint32_t Max_Used; /* Maximum bucket memory used */

} Display;

//---------------------------------------------------------------

extern std::int32_t DisplayWidth,
    DisplayHeight,
    DisplayBPP;
extern Display the_display;

//---------------------------------------------------------------

#endif
