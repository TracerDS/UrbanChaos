//	Display.h
//	Guy Simmons, 6th October 1996.

#pragma once

#ifndef	_MF_TYPES_H_
#	include <MFTypes.h>
#endif

#include <cstdint>

//---------------------------------------------------------------
// DModes.cpp

#define	DS_THREAD_ACTIVE		(1<<0)
#define	DS_SHELL_WINDOW_OPEN	(1<<1)
#define	DS_DIRECT_DRAW_ACTIVE	(1<<2)
#define	DS_DISPLAY_MODE_SET		(1<<3)
#define	DS_CREATED_SURFACES		(1<<4)
#define	DS_SCREEN_LOCKED		(1<<5)

enum DisplayModes
{
	DISPLAY_MODE_NONE			=	0,

	DISPLAY_MODE_320_200_8,		// 320x200
	DISPLAY_MODE_320_200_16,
	DISPLAY_MODE_320_200_24,
	DISPLAY_MODE_320_200_32,
	DISPLAY_MODE_320_240_8,		// 320x240
	DISPLAY_MODE_320_240_16,
	DISPLAY_MODE_320_240_24,
	DISPLAY_MODE_320_240_32,
	DISPLAY_MODE_320_400_8,		// 320x400
	DISPLAY_MODE_320_400_16,
	DISPLAY_MODE_320_400_24,
	DISPLAY_MODE_320_400_32,
	DISPLAY_MODE_512_384_8,		// 512x384
	DISPLAY_MODE_512_384_16,
	DISPLAY_MODE_512_384_24,
	DISPLAY_MODE_512_384_32,
	DISPLAY_MODE_640_400_8,		// 640x400
	DISPLAY_MODE_640_400_16,
	DISPLAY_MODE_640_400_24,
	DISPLAY_MODE_640_400_32,
	DISPLAY_MODE_640_480_8,		// 640x480
	DISPLAY_MODE_640_480_16,
	DISPLAY_MODE_640_480_24,
	DISPLAY_MODE_640_480_32,
	DISPLAY_MODE_800_600_8,		// 800x600
	DISPLAY_MODE_800_600_16,
	DISPLAY_MODE_800_600_24,
	DISPLAY_MODE_800_600_32,
	DISPLAY_MODE_1024_768_8,	// 1024x768
	DISPLAY_MODE_1024_768_16,
	DISPLAY_MODE_1024_768_24,
	DISPLAY_MODE_1024_768_32,
	DISPLAY_MODE_1152_864_8,	// 1152x864
	DISPLAY_MODE_1152_864_16,
	DISPLAY_MODE_1152_864_24,
	DISPLAY_MODE_1152_864_32,
	DISPLAY_MODE_1280_1024_8,	// 1280x1024
	DISPLAY_MODE_1280_1024_16,
	DISPLAY_MODE_1280_1024_24,
	DISPLAY_MODE_1280_1024_32,

	DISPLAY_MODE_COUNT
};

struct DisplayModeInfo
{
	bool Availability;	// Do we have this mode?
	std::uint32_t Width, Height, Depth; // Display dimensions.
	std::int32_t DisplayMode;	// Internal host screen mode identifier.
#ifdef _MF_WINDOWS
	DDSURFACEDESC DD_ModeDesc;
#endif
};

extern DisplayModeInfo DisplayModes[];


std::int32_t DisplayModeAvailable(std::uint32_t width, std::uint32_t height, std::uint32_t depth);


//---------------------------------------------------------------
// Display.cpp

#define	FLAGS_USE_3DFX (1<<0)

#define	DS_WAIT_VBI (1<<0)
#define	DS_DO_FLIP (1<<1)

extern std::uint8_t DisplayActive, WorkScreenDepth, *WorkScreen;
extern std::int32_t WorkScreenHeight, WorkScreenPixelWidth, WorkScreenWidth;

#ifdef	_MF_WINDOWS
extern DDSURFACEDESC		DD_DisplayDesc;
extern LPDIRECTDRAW			lp_DD;           // Main DirectDraw object
extern LPDIRECTDRAWSURFACE	lp_DD_FrontSurface,
							lp_DD_BackSurface;
extern volatile std::uint8_t		MFShellActive;
#endif

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags);
std::int32_t CloseDisplay();
std::int32_t SetDisplay(std::uint32_t width,std::uint32_t height,std::uint32_t depth);
void ClearDisplay();
void FadeDisplay(std::uint8_t mode);
void* LockWorkScreen();
void UnlockWorkScreen();
void ShowWorkScreen(std::uint32_t flags);
void ShowWorkWindow(std::uint32_t flags);
void ClearWorkScreen(std::uint8_t colour);