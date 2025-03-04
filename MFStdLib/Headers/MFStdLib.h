
// MFStdLib.h
// Guy Simmons, 18th December 1997.

#pragma once


//---------------------------------------------------------------

// Standard 'C' includes.
#if !defined(TARGET_DC)
#	include <ctime>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cstdint>

// Library defines.
#define	_MF_WINDOWS

#ifndef	_WIN32
	#define	_WIN32
#endif

#ifndef	WIN32
	#define	WIN32
#endif

// Specific Windows includes.
#define D3D_OVERLOADS
#include	<windows.h>
#include	<windowsx.h>
#include	<d3dtypes.h>
#include	<ddraw.h>
#ifndef TARGET_DC
// For the DX8 headers, you need to define this to get old interfaces.
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0700
#endif
#endif
#include	<dinput.h>
#include	<d3d.h>

#ifdef TARGET_DC
#include "target.h"
#endif

//---------------------------------------------------------------

struct MFPoint {
	std::int32_t X, Y;
};

struct MFRect {
	std::int32_t Left;
	std::int32_t Top;
	std::int32_t Right;
	std::int32_t Bottom;
	std::int32_t Width;
	std::int32_t Height;
};

//---------------------------------------------------------------
// MF Standard includes.

#include "StdFile.h"
#include "StdKeybd.h"
#include "StdMaths.h"
#include "StdMem.h"
#include "StdMouse.h"

//---------------------------------------------------------------
// Display

#define	FLAGS_USE_3DFX			(1<<0)
#define	FLAGS_USE_3D			(1<<1)
#define	FLAGS_USE_WORKSCREEN	(1<<2)

extern std::uint8_t				WorkScreenDepth,
							*WorkScreen;
extern std::int32_t				WorkScreenHeight,
							WorkScreenPixelWidth,
							WorkScreenWidth;
extern std::int32_t				DisplayWidth,
							DisplayHeight,
							DisplayBPP;

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags);
std::int32_t SetDisplay(std::uint32_t width,std::uint32_t height,std::uint32_t depth);
std::int32_t CloseDisplay();
std::int32_t ClearDisplay(std::uint8_t r,std::uint8_t g,std::uint8_t b);
void FadeDisplay(std::uint8_t mode);
void* LockWorkScreen();
void UnlockWorkScreen();
void ShowWorkScreen(std::uint32_t flags);
void ClearWorkScreen(std::uint8_t colour);

//---------------------------------------------------------------
// Host

#define	SHELL_NAME				"Mucky Foot Shell\0"
#define	H_CREATE_LOG			(1<<0)
#define	SHELL_ACTIVE			(LibShellActive())
#define	SHELL_CHANGED			(LibShellChanged())

#define	main(ac,av)				MF_main(ac,av)

struct MFTime
{
	std::int32_t		Hours,
				Minutes,
				Seconds,
				MSeconds;
	std::int32_t		DayOfWeek,		//	0 - 6;		Sunday		=	0
				Day,
				Month,			//	1 - 12;		January		=	1
				Year;
	std::int32_t		Ticks;			// Number of ticks(milliseconds) since windows started.
};

std::int32_t main(std::uint16_t argc, TCHAR** argv);
bool SetupHost(std::uint32_t flags);
void ResetHost();
//void            TraceText(char* error, ...);
void TraceText(const char *error, ...);
bool LibShellActive();
bool LibShellChanged();
bool LibShellMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine);

#define	NoError					0


#ifndef NDEBUG

void DebugText(const char* error, ...);
#define TRACE				TraceText
#define	LogText				DebugText
#define	MFMessage			LibShellMessage
#define	ERROR_MSG(e,m)		{if(!(e)) {LibShellMessage(m,__FILE__,__LINE__);}}
//#define ASSERT(e)			{if (!(e)) { DebugBreak(); }else{/*TRACE("file %s line %d \n",__FILE__,__LINE__);*/}}
#ifndef ASSERT
#define ASSERT(e)			ERROR_MSG(e,"ASSERT TRIGGERED");
#endif

#else

#define DebugText
#define TRACE
#define LogText
#define MFMessage
#define ERROR_MSG(e,m)		{}
#ifndef ASSERT
#define ASSERT(e)			{}
#endif

#endif

//---------------------------------------------------------------
// Input.

#define	MOUSE			DIDEVTYPE_MOUSE
#define	KEYBOARD		DIDEVTYPE_KEYBOARD 
#define	JOYSTICK		DIDEVTYPE_JOYSTICK


#if 0
bool GetInputDevice(std::uint8_t type,std::uint8_t sub_type);
bool ReadInputDevice();
#endif

//---------------------------------------------------------------
// Standard macros.

#define	sgn(a)				(((a)<0) ? -1 : 1)
#define	swap(a,b)			{a^=b;b^=a;a^=b;}

#define	in_range(a,min,max)	{if(a>(max))a=(max);else if(a<(min))a=(min);}
#ifndef	min
#define	min(a,b)			(((a)<(b)) ? (a) : (b))
#endif

#ifndef	max
#define	max(a,b)			(((a)>(b)) ? (a) : (b))
#endif

//---------------------------------------------------------------

//
// Stuff put in by Mark...
//
#undef INFINITY
#define INFINITY		0x7fffffff
#define PI				(3.14159265F)
#define WITHIN(x,a,b)	((x) >= (a) && (x) <= (b))
#define SATURATE(x,a,b)	{if ((x) < (a)) {(x) = (a);} else if ((x) > (b)) {(x) = (b);}}
#define SWAP(a,b)		{std::int32_t temp; temp = (a); (a) = (b); (b) = temp;}
#define SWAP_FL(a,b)	{float temp; temp = (a); (a) = (b); (b) = temp;}
#define MIN(a,b)		(((a) < (b)) ? (a) : (b))
#define MAX(a,b)		(((a) > (b)) ? (a) : (b))
#define SIGN(x)			(((x)) ? (((x) > 0) ? +1 : -1) : 0)

//
// Some maths stuff by mike
//


#define	QDIST3(x,y,z)	((x)>(y) ? ((x)>(z) ? (x)+((y)>>2)+((z)>>2) : (z)+((x)>>2)+((y)>>2)) : ((y)>(z) ? ((y)+((x)>>2)+((z)>>2)) : (z)+((x)>>2)+((y)>>2) ))
#define	QDIST2(x,y)		((x)>(y) ? ((x)+((y)>>1) ):((y)+((x)>>1) ))

#define	SDIST3(x,y,z)	(((x)*(x))+((y)*(y))+((z)*(z)))
#define	SDIST2(x,y)		(((x)*(x))+((y)*(y)))