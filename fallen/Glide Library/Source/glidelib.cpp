
#define	INITGUID
#define __MSC__

#include <MFStdLib.h>
#include <glide.h>
#include <glideutl.h>
#include <sst1vid.h>

#include "dsmanager.h"


#pragma comment(lib, "glide3x.lib")


std::int32_t DisplayWidth    = 640;
std::int32_t DisplayHeight   = 480;
std::int32_t volatile MouseX = 320;
std::int32_t volatile MouseY = 240;


volatile std::uint8_t	AltFlag,
				ControlFlag,
				ShiftFlag;
volatile std::uint8_t	Keys[256],
				LastKey;


std::int32_t OpenDisplay (std::uint32_t width, std::uint32_t height, std::uint32_t bpp, std::uint32_t flags)
{
	const char *hardware;

	grGlideInit();

	//
	// What sort of hardware do we have?
	//

	hardware = grGetString(GR_HARDWARE);

	if (hardware)
	{
		TRACE("Glide test\n~~~~~~~~~~\n\n");
		TRACE("\tRunning on %s\n\n", hardware);
	}
	else
	{
		TRACE("No 3DFX harware found!\n\n");

		return 1;
	}

	grSstSelect(0);
	grSstWinOpen(
		0,
		GR_RESOLUTION_640x480,
		GR_REFRESH_60Hz,
		GR_COLORFORMAT_ARGB,
		GR_ORIGIN_UPPER_LEFT,
		2,
		1);

	return 0;
}

std::int32_t CloseDisplay()
{
	grGlideShutdown();

	return true;
}

void DebugText(char *, ...)
{
}

bool SetupHost(std::uint32_t flags)
{
	return true;
}

void ResetHost()
{
}

bool LibShellMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine)
{
	return false;
}

void LoadBackImage(std::uint8_t *input)
{
	std::int32_t x;
	std::int32_t y;

	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;

	std::uint8_t *output;
	void  *image;

	//
	// Convert to RGB 565
	//

	output = (std::uint8_t *) malloc(640 * 480 * 4);
	image  = output;

	for (y = 0; y < 480; y++)
	for (x = 0; x < 640; x++)
	{
		red   = *input++;
		green = *input++;
		blue  = *input++;

		*output++ = red;
		*output++ = green;
		*output++ = blue;
		*output++ = 0;

		/*
	   *output  = 0;
	   *output |= (red   & 0x1f) << 11;
	   *output |= (green & 0x3f) << 5;
	   *output |= (blue  & 0x1f) << 0;
		*/
	}

	//
	// Download to the 3dfx back buffer.
	//

	FxBool res = grLfbWriteRegion(
		GR_BUFFER_BACKBUFFER,
		0, 0,
		GR_LFB_SRC_FMT_888,
		640, 480,
		FXFALSE,
		640 * 4,
		image);

	free(image);
}




LRESULT CALLBACK OS_message_handler(
					HWND   window_handle,
					UINT   message_type,
					WPARAM param_w,
					LPARAM param_l)
{
	std::uint8_t scancode;
	std::int32_t newki;

	switch(message_type)
	{
		case WM_PAINT:

			//
			// The user callback function does all the screen drawing.
			// Do enough to appease windows.
			//

			HDC  		device_context_handle;
			PAINTSTRUCT	paintstruct;

			device_context_handle = BeginPaint(window_handle, &paintstruct);
			EndPaint(window_handle, &paintstruct);

			return 0;

		case WM_KEYDOWN: 
		case WM_KEYUP:	 

			//
			// Keyboard stuff.
			//

			scancode  = (param_l >> 16) & 0xff;
			scancode |= (param_l >> 17) & 0x80;

			if (message_type == WM_KEYDOWN)
			{
				Keys[scancode] = 1;
				LastKey        = scancode;
			}
			else
			{
				Keys[scancode] = 0;
			}

			if(Keys[KB_LALT] || Keys[KB_RALT])
				AltFlag		=	1;
			else
				AltFlag		=	0;

			if(Keys[KB_LCONTROL] || Keys[KB_RCONTROL])
				ControlFlag	=	1;
			else
				ControlFlag	=	0;

			if(Keys[KB_LSHIFT] || Keys[KB_RSHIFT])
				ShiftFlag	=	1;
			else
				ShiftFlag	=	0;

			

			return 0;

		default:

			//
			// Just let windows do its normal thing.
			//

			return DefWindowProc(
						window_handle,
						message_type,
						param_w,
						param_l);
	}
}



//---------------------------------------------------------------
//
// WinMain - Entry point for windows application.
//
//---------------------------------------------------------------

int		  iGlobalWinMode;
LPSTR	  lpszGlobalArgs;
HINSTANCE hGlobalPrevInst;
HINSTANCE hGlobalThisInst;

volatile HWND		hDDLibWindow	=	nullptr;

static std::uint16_t	argc;
static LPTSTR	argv[MAX_PATH];

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int iWinMode)
{
	// Store WinMain parameters.
	lpszGlobalArgs	=	lpszArgs;
	iGlobalWinMode	=	iWinMode;
	hGlobalThisInst	=	hThisInst;
	hGlobalPrevInst	=	hPrevInst;


	WNDCLASS    wc;
	bool        rc;

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = OS_message_handler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon( nullptr, IDI_APPLICATION);    /* generic icon */
	wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground = GetStockObject( GRAY_BRUSH );
	wc.lpszMenuName =  nullptr;
	wc.lpszClassName = "WinGlideClass";
	rc = RegisterClass( &wc );

	if( !rc )
	{
		return -1;
	}

	hDDLibWindow = CreateWindowEx(
					   WS_EX_APPWINDOW,
					   "WinGlideClass",
					   "Glide Library",
					   WS_OVERLAPPED |     
					   WS_CAPTION  |     
					   WS_THICKFRAME | 
					   WS_MAXIMIZEBOX | 
					   WS_MINIMIZEBOX | 
					   WS_VISIBLE | /* so we don't have to call ShowWindow */
					   WS_POPUP | /* non-app window */
					   WS_SYSMENU, /* so we get an icon in the tray */
					   CW_USEDEFAULT, 
					   CW_USEDEFAULT,
					   0x100, /* GetSystemMetrics(SM_CXSCREEN), */
					   0x40, /* GetSystemMetrics(SM_CYSCREEN), */
					   nullptr,
					   nullptr,
					   hThisInst,
					   nullptr);
  
	if (!hDDLibWindow)
	{
		return -1;
	}

	ShowWindow(hDDLibWindow, SW_NORMAL);
	UpdateWindow(hDDLibWindow);

	the_sound_manager.Init();
	SetupMemory();
	MF_main(argc,argv);
	ResetMemory();
	the_sound_manager.Fini();

	DestroyWindow(hDDLibWindow);

	return 0;
}


void	Time(MFTime *the_time)
{
	SYSTEMTIME	new_time;


	GetLocalTime(&new_time);
	the_time->Hours		=	new_time.wHour;
	the_time->Minutes	=	new_time.wMinute;
	the_time->Seconds	=	new_time.wSecond;
	the_time->MSeconds	=	new_time.wMilliseconds;
	the_time->DayOfWeek	=	new_time.wDayOfWeek;
	the_time->Day		=	new_time.wDay;
	the_time->Month		=	new_time.wMonth;
	the_time->Year		=	new_time.wYear;
	the_time->Ticks		=	GetTickCount();
}


void dd_error(HRESULT dd_err)
{
}

void di_error(HRESULT dd_err)
{
}

bool LibShellActive()
{
	MSG msg;

	while(PeekMessage(&msg,nullptr,0,0,PM_NOREMOVE))
	{
		if(GetMessage(&msg, nullptr, 0, 0))
		{
			//
			// Pass the message to the message handler.
			//

			DispatchMessage(&msg);
		}
	}

	return true;
}

#include "ddlib.h"
