#include <MFHeader.h>
#include "I86.h"
#include "string.h"
#include "stdio.h"
#include "conio.h"
#include "dos.h"

typedef struct
{
    std::uint32_t MyEDI, MyESI, MyEBP, MyReserved, MyEBX, MyEDX, MyECX, MyEAX;
    std::uint16_t MyFlags, MyES, MyDS, MyFS, MyGS, MyIP, MyCS, MySP, MySS;
} TbRMREGS2;

std::uint8_t *WorkScreen,
    WorkScreenDepth;
std::int32_t WorkScreenHeight,
    WorkScreenWidth,
    WorkScreenPixelWidth;
std::uint8_t CurrentPalette[256 * 3];

extern std::int32_t SetupMouse();
extern std::int32_t ResetMouse();
extern void SetDrawFunctions(std::uint32_t depth);

//------------------------------------------------------------

std::int16_t VesaBytesPerLine = 0;
std::int16_t VesaHRes = 0;
std::int16_t VesaVRes = 0;
std::uint8_t *VesaData = 0;
std::int16_t VesaPage = 0;
std::int16_t VesaGran = 0;

std::int16_t vesa_modes[] =
    {
        0x13, 0x100, 0x101, 0x103, 0x105, 0x107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//-----------------------------------------------------------------------

extern std::int16_t video_int(std::int16_t, std::int16_t, std::int16_t, std::int16_t);
#pragma aux video_int =        \
    "push	bp ",          \
            "int	10h ", \
            "pop	bp ",  \
            parm caller[ax][bx][cx][dx] value[ax];

std::int32_t VesaGetGran(std::int16_t mode) {
    union REGS inregs;
    union REGS outregs;
    struct SREGS ssregs;
    TbRMREGS2 rmregs;

    memset(&rmregs, 0, sizeof(TbRMREGS2));
    rmregs.MyES = (std::uint16_t) (((std::uint32_t) VesaData) >> 4);
    rmregs.MyDS = (std::uint16_t) (((std::uint32_t) VesaData) >> 4);
    rmregs.MyEDI = 0;
    rmregs.MyEAX = 0x4f * 256 + 1;
    rmregs.MyECX = mode;

    memset(&inregs, 0, sizeof(union REGS));
    memset(&outregs, 0, sizeof(union REGS));
    memset(&ssregs, 0, sizeof(struct SREGS));
    segread(&ssregs);
    inregs.x.eax = 0x300;
    inregs.x.ebx = 0x010;
    inregs.x.edi = (std::uint32_t) &rmregs;
    int386x(0x31, &inregs, &outregs, &ssregs);

    VesaGran = (std::int16_t) (VesaData[5] * 256 + VesaData[4]);
    VesaBytesPerLine = (std::int16_t) (VesaData[17] * 256 + VesaData[16]);
    VesaHRes = (std::int16_t) (VesaData[19] * 256 + VesaData[18]);
    VesaVRes = (std::int16_t) (VesaData[21] * 256 + VesaData[20]);

    return (1);
}

std::int32_t VesaSetMode(std::int16_t bx) {
    union REGS inregs;
    union REGS outregs;
    memset(&inregs, 0, sizeof(union REGS));
    memset(&outregs, 0, sizeof(union REGS));
    inregs.h.ah = 0x4f;
    inregs.h.al = 2;
    inregs.w.bx = bx;
    int386(0x10, &inregs, &outregs);
    VesaGetGran(bx);

    return (1);
}

std::int32_t VesaSetPage(std::int16_t dx) {
    if (VesaPage != dx) {
        video_int(0x4f05, 0, 0, (std::int16_t) ((dx * 64) / VesaGran));
        video_int(0x4f05, 1, 0, (std::int16_t) ((dx * 64) / VesaGran));

        VesaPage = dx;
    }
    return (1);
}

std::int32_t VesaGetInfo() {
    union REGS inregs;
    union REGS outregs;
    struct SREGS ssregs;
    TbRMREGS2 rmregs;

    memset(&rmregs, 0, sizeof(TbRMREGS2));
    rmregs.MyES = (std::uint16_t) (((std::uint32_t) VesaData) >> 4);
    rmregs.MyDS = (std::uint16_t) (((std::uint32_t) VesaData) >> 4);
    rmregs.MyEDI = 0;
    rmregs.MyEAX = 0x4f * 256;
    rmregs.MyEBX = 0x101;

    memset(&inregs, 0, sizeof(union REGS));
    memset(&outregs, 0, sizeof(union REGS));
    memset(&ssregs, 0, sizeof(struct SREGS));
    segread(&ssregs);
    inregs.x.eax = 0x300;
    inregs.x.ebx = 0x010;
    inregs.x.edi = (std::uint32_t) &rmregs;
    int386x(0x31, &inregs, &outregs, &ssregs);

    return ((strncmp((char *) VesaData, "VESA", 4) == 0) ? 1 : 0);
}

std::uint8_t VesaIsModeAvailable(std::uint16_t mode) {
    std::uint16_t *wp;
    std::int32_t t;

    if (VesaGetInfo() == 1) {
        wp = (std::uint16_t *) &VesaData[14];
        t = *wp;
        wp++;
        t += *wp << 4;
        wp = (std::uint16_t *) t;
        for (; *wp != 0xffff; wp++) {
            if (*wp == mode) {
                return (1);
            }
        }
    }
    return (0);
}

#define DPMI_INT (0x31)

void *allocDOS(unsigned long int nbytes, short int *pseg, short int *psel) {
    union REGS regs;
    struct SREGS sregs;
    unsigned npara = (nbytes + 15) / 16;
    void *pprot;

    pprot = nullptr;
    *pseg = 0; // assume will fail
    *psel = 0;

    // DPMI call 100h allocates DOS memory
    segread(&sregs);
    regs.x.eax = 0x0100; // DPMI: Allocate DOS Memory
    regs.x.ebx = npara;  // number of paragraphs to alloc

    int386(DPMI_INT, &regs, &regs);

    if (regs.w.cflag == 0) {
        *pseg = regs.x.eax; // the real-mode segment
        *psel = regs.x.edx; // equivalent protected-mode selector
        // pprot is the protected mode address of the same allocated block.
        // The Rational extender maps the 1 MB physical DOS memory into
        // the bottom of our virtual address space.
        pprot = (void *) ((unsigned) *pseg << 4);
    }
    return (pprot);
}

void freeDOS(short int sel) {
    union REGS regs;
    regs.x.eax = 0x0101; // DPMI free DOS memory
    regs.x.edx = sel;

    int386(DPMI_INT, &regs, &regs);
}

struct ScreenModes {
    std::uint16_t Mode;
    std::uint16_t Width;
    std::uint16_t Height;
};

struct ScreenModes screen_modes[] =
    {
        {0x13, 320, 200},
        {0x13, 320, 200},
        {0x100, 640, 400},
        {0x101, 640, 480},
        {0x103, 800, 600},
        {0x105, 1024, 768},
        {0x107, 1280, 1024},
        {0, 0, 0}};

#define SCREEN_MODE_320_200_8 1
#define SCREEN_MODE_640_400_8 2
#define SCREEN_MODE_640_480_8 3
#define SCREEN_MODE_800_600_8 4
#define SCREEN_MODE_1024_768_8 5
#define SCREEN_MODE_1280_1024_8 6

std::uint16_t old_display_mode = 0;

std::uint16_t find_mode(std::int32_t width, std::int32_t height) {
    std::int32_t c0 = 1;

    while (screen_modes[c0].Mode) {
        if (screen_modes[c0].Width == width && screen_modes[c0].Height == height)
            return (screen_modes[c0].Mode);
        c0++;
    }
    printf(" res %d * %d not found \n", width, height);
    return (0);
}

void MEM_COPY_LONG(std::uint8_t *, std::uint8_t *, std::uint32_t);
#pragma aux MEM_COPY_LONG = \
    "rep 	movsd" parm[edi][esi][ecx] modify[edi esi ecx];

static vesa_flag = 0;
static screen_setup = 0;

static std::int16_t sect1, sect2;
static std::uint8_t alloc_for_vesa = 0;
static std::uint8_t *screen_mem = 0;
std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth) {
    if (SetDisplay(width, height, depth) == 0)
        return (0);
    SetDrawFunctions(8);
    SetupMouse();
    return (1);
}

std::int32_t CloseDisplay() {
    ResetMouse();
    if (alloc_for_vesa)
        freeDOS(sect2);
    if (old_display_mode)
        VesaSetMode((std::int16_t) old_display_mode);

    if (screen_mem)
        MemFree((void *) screen_mem);

    return (1);
}

std::int32_t SetDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth) {
    union REGS inregs;
    union REGS outregs;
    std::int16_t mode;

    SetDrawFunctions(8);
    if (!alloc_for_vesa) {
        alloc_for_vesa = 1;
        VesaData = (std::uint8_t *) allocDOS(512, &sect1, &sect2);
    }
    if (!screen_mem)
        screen_mem = (std::uint8_t *) MemAlloc(1280 * 1024);

    if (!screen_mem)
        return (0);

    if ((mode = find_mode(width, height)) == 0)
        return (0);

    if (old_display_mode == 0) {
        inregs.x.eax = 0x0f00;
        int386(0x10, &inregs, &outregs);
        old_display_mode = outregs.h.al;
    }
    if (mode == 0x13) {
        inregs.x.eax = mode;
        int386(0x10, &inregs, &outregs);
        vesa_flag = 0;
        screen_setup = 1;
    } else {
        if (VesaSetMode((std::int16_t) mode))
            screen_setup = 1;
        vesa_flag = 1;
    }
    WorkScreenPixelWidth = width;
    WorkScreenWidth = width;
    WorkScreenHeight = height;
    return (1);
}

void ShowWorkScreen(std::uint32_t flags) {
    std::int32_t copy_size;
    std::int32_t page = 0;
    std::int32_t pages;
    std::int32_t size;
    std::uint8_t *ptr;

    ptr = WorkScreen;
    if (!screen_setup)
        return;
    //	LbMousePlace();

    if (!vesa_flag) {
        VesaSetPage((std::int16_t) 0);
        MEM_COPY_LONG((std::uint8_t *) 0xa0000, WorkScreen, (320 * 200) >> 2);
    } else {
        page = 0;
        size = WorkScreenWidth * WorkScreenHeight;
        pages = size >> 16;

        for (page = 0; page < pages; page++) {
            if (size < 65536)
                copy_size = size;
            else
                copy_size = 65536;
            VesaSetPage((std::int16_t) page);
            MEM_COPY_LONG((std::uint8_t *) 0xa0000, ptr, copy_size >> 2);
            ptr += 65536;
        }
    }
    //	LbMouseRemove();
}

void SetPalette(std::uint8_t *pal) {
    std::uint16_t c0;

    outp(0x3c6, 0xff); // tell card some palette things are a comin
    for (c0 = 0; c0 < 256; ++c0) {
        outp(0x3c8, c0);
        outp(0x3c9, (*pal++));
        outp(0x3c9, (*pal++));
        outp(0x3c9, (*pal++));
    }
}

void ClearWorkScreen(std::uint8_t colour) {
    memset(WorkScreen, 0, WorkScreenWidth * WorkScreenHeight);
}

void FadeDisplay(std::uint8_t mode) {
}

void *LockWorkScreen() {
    return ((void *) WorkScreen);
}

void UnlockWorkScreen() {
}

void ClearDisplay() {
}

void ShowWorkWindow(std::uint32_t flags) {
    std::int32_t line;
    std::uint8_t *dest, *source;
    std::uint8_t current_page, page, left_page, right_page;
    std::uint32_t offset;

    offset = WorkWindowRect.Top * WorkScreenWidth + WorkWindowRect.Left;
    dest = (std::uint8_t *) 0xa0000;
    source = WorkWindow;
    current_page = offset >> 16;
    offset &= 0xffff;

    VesaSetPage((std::uint16_t) current_page);

    for (line = WorkWindowRect.Top; line < WorkWindowRect.Bottom; line++) {
        if (offset >= 0x10000) { // address out of range, so page change
            offset -= 0x10000;
            VesaSetPage((std::uint16_t) ++current_page);
        }

        if ((offset + WorkWindowWidth) >= 0x10000) {
            // line spans 2 pages
            std::int32_t copy_size;

            copy_size = 0x10000 - offset;
            memcpy(dest + offset, source, copy_size);
            VesaSetPage((std::uint16_t) ++current_page);
            offset -= 0x10000;

            memcpy((std::uint8_t *) 0xa0000, source + copy_size, WorkWindowWidth - copy_size);
        } else
            memcpy(dest + offset, source, WorkWindowWidth);

        offset += WorkScreenWidth;
        source += WorkScreenWidth;
    }
}

std::int32_t FindColour(std::uint8_t *the_palette, std::int32_t r, std::int32_t g, std::int32_t b) {
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

            found = (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
            break;
        case 4:
            found = ((r << 16) | (g << 8) | (b));
            break;
    }
    return (found);
}
