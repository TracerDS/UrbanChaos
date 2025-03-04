// MFStdLib.h
// Guy Simmons, 18th December 1997.

#pragma once

//---------------------------------------------------------------

// Standard 'C' includes.
//psx#include	<iostream.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<string.h>
//psx#include	<time.h>

// Library defines.
#define	_MF_WINDOWS

#ifndef	_WIN32
	#define	_WIN32
#endif

#ifndef	WIN32
	#define	WIN32
#endif

// Specific Windows includes.
/*
#include	<windows.h>
#include	<windowsx.h>
#include	<ddraw.h>
#include	<dinput.h>
#include	<dplay.h>
#include	<dsound.h>
#include	<d3d.h>
#include	<d3dtypes.h>
*/

//---------------------------------------------------------------

typedef unsigned char		std::uint8_t;
typedef signed char			std::int8_t;
typedef char				char;
typedef unsigned short		std::uint16_t;
typedef signed short		std::int16_t;
typedef unsigned long		std::uint32_t;
typedef signed long			std::int32_t;
typedef unsigned char		bool;

typedef signed long			FIX_8;
typedef signed long			FIX_9;
typedef signed long			FIX_10;
typedef signed long			FIX_11;
typedef signed long			FIX_12;
typedef signed long			FIX_13;
typedef signed long			FIX_14;
typedef signed long			FIX_15;
typedef signed long			FIX_16;
typedef signed long			FIX_17;
typedef signed long			FIX_18;
typedef signed long			FIX_19;
typedef signed long			FIX_20;
typedef signed long			FIX_21;
typedef signed long			FIX_22;
typedef signed long			FIX_23;
typedef signed long			FIX_24;
typedef signed long			FIX_25;
typedef signed long			FIX_26;
typedef signed long			FIX_27;
typedef signed long			FIX_28;
typedef signed long			FIX_29;
typedef signed long			FIX_30;

typedef struct
{
	std::int32_t		X,
				Y;
}MFPoint;

typedef struct
{
	std::int32_t		Left,
				Top,
				Right,
				Bottom,
				Width,
				Height;
}MFRect;



//
// PSX Bodges
//


#define	GetTickCount()		(0)
//#define	ApproxRoot(x,div)		( ( (x/div)+div )>>1)
//#define	Root(x)				(ApproxRoot(x,ApproxRoot(x,ApproxRoot(x,ApproxRoot(x,128)) )))
#define		Root(x)		SquareRoot0(x)





//---------------------------------------------------------------
// MF Standard includes.

//#include	"StdFile.h"
#include	"StdKeybd.h"
#include	"StdMaths.h"
#include	"StdMem.h"
//#include	"StdMouse.h"

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

std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags);
std::int32_t SetDisplay(std::uint32_t width,std::uint32_t height,std::uint32_t depth);
std::int32_t CloseDisplay();
void ClearDisplay();
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

//#define	main(ac,av)				MF_main(ac,av)

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

std::int32_t main(std::uint16_t argc, char** argv);
bool SetupHost(std::uint32_t flags);
void ResetHost();
void DebugText(char* error, ...);
void TraceText(char* error, ...);
bool LibShellActive();
bool LibShellChanged();
bool LibShellMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine);

#define	NoError					0


#ifndef NDEBUG

//#define TRACE				TraceText
//#define	LogText				DebugText
//#define	MFMessage			LibShellMessage
//#define	ERROR_MSG(e,m)		{if(!(e)) {}}
//#define ASSERT(e)			{if (!(e)) {}}

//void			DebugText(char* error, ...);
//void            TraceText(char* error, ...);
#define	DebugText
#define	TraceText
#define TRACE
#define LogText
#define MFMessage
#define ERROR_MSG(e,m)		{}
#define ASSERT(e)			{if(!(e))asm("break 0");}

#else

#define DebugText
#define TrackText
#define TRACE
#define LogText
#define MFMessage
#define ERROR_MSG(e,m)		{}
#define ASSERT(e)			{}

#endif

//---------------------------------------------------------------
// Input.

#define	MOUSE			0
#define	KEYBOARD		0 //DIDEVTYPE_KEYBOARD 
#define	JOYSTICK		1 //DIDEVTYPE_JOYSTICK


#ifdef PSX
bool GetInputDevice(std::uint8_t type,std::uint8_t sub_type);
bool ReadInputDevice();
#endif

//---------------------------------------------------------------
//---------------------------------------------------------------
// Sound.

#define	SAMPLE_VOL_MIN		1
#define	SAMPLE_VOL_MAX		2

#define	SAMPLE_PAN_LEFT		3
#define	SAMPLE_PAN_RIGHT	4
#define	SAMPLE_PAN_CENTER	5

#define	SAMPLE_FREQ_MIN		6
#define	SAMPLE_FREQ_MAX		7
#define	SAMPLE_FREQ_ORIG	8


void LoadSampleList(char* sample_file);
void PlaySample(std::int32_t ref,std::int16_t sample_no,std::int32_t vol,std::int32_t pan,std::int32_t freq,std::int32_t pri);




//	New Sound

#define	WAVE_STEREO				(1<<0)
#define	WAVE_POLAR				(1<<1)
#define	WAVE_CARTESIAN			(1<<2)
#define	WAVE_PAN_RATE			(1<<3)
#define	WAVE_DISTANCE_MAPPING	(1<<4)
#define	WAVE_LOOP				(1<<5)
#define	WAVE_SET_LOOP_POINTS	(1<<6)

#define	WAVE_TYPE_MASK		(WAVE_STEREO|WAVE_POLAR|WAVE_CARTESIAN)

#define	WAVE_PLAY_INTERUPT		0
#define	WAVE_PLAY_NO_INTERUPT	1
#define	WAVE_PLAY_OVERLAP		2
#define	WAVE_PLAY_QUEUE			3

struct WaveParams
{
	std::uint32_t		Flags,
				LoopStart,
				LoopEnd,
				Priority;
	union
	{
		//	Stereo.
		struct
		{
			std::int32_t	Pan,
					Volume;
		}Stereo;

		//	Polar.
		struct
		{
			std::int32_t	Azimuth,
					Elevation,
					Range;
		}Polar;

		//	Cartesian.
		struct
		{
			std::int32_t	Scale,
					X,
					Y,
					Z;
		}Cartesian;
	}Mode;
};
void LoadWaveList(char* path,char* file);
void FreeWaveList();
void PlayWave(std::int32_t ref,std::int32_t wave_id,std::int32_t play_type,WaveParams *the_params);
void StopWave(std::int32_t ref,std::int32_t wave_id);
void SetListenerPosition(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t scale);

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



