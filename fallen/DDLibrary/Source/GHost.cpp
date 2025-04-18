// GHost.cpp
// Guy Simmons, 20th November 1997.

#include "DDLib.h"
#include "..\headers\Sound.h"
#include "mfx.h"

#define PAUSE_TIMEOUT 500
#define PAUSE (1 << 0)
#define PAUSE_ACK (1 << 1)

int iGlobalWinMode;
DWORD ShellID;
HACCEL hDDLibAccel;
HANDLE hDDLibThread;
HINSTANCE hGlobalPrevInst,
    hGlobalThisInst;
LPSTR lpszGlobalArgs;
WNDCLASS DDLibClass;

volatile bool ShellActive;
volatile std::uint32_t PauseFlags = 0,
                       PauseCount = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------

DWORD DDLibThread(LPVOID param) {
    MSG msg;

#ifdef TARGET_DC
    // Create a basic window - only used for message passing.
    hDDLibWindow = CreateWindowEx(
        0,
        TEXT("Urban Chaos"),
        TEXT("Urban Chaos"),
        WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        nullptr,
        hGlobalThisInst,
        nullptr);

#else
    hDDLibWindow = CreateWindowEx(
        0,
        "Urban Chaos",
        "Urban Chaos",
#ifndef NDEBUG
        WS_OVERLAPPEDWINDOW,
#else
        WS_POPUP,
#endif
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        nullptr,
        hGlobalThisInst,
        nullptr);
#endif

    ShowWindow(hDDLibWindow, iGlobalWinMode);
    UpdateWindow(hDDLibWindow);

    ShellActive = true;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    ShellActive = false;

    return 0;
}

//---------------------------------------------------------------

bool SetupKeyboard();
void ResetKeyboard();

bool SetupHost(std::uint32_t flags) {
    DWORD id;

    ShellActive = false;

#ifndef NDEBUG
    InitDebugLog();
#endif

    if (!SetupMemory())
        return false;
    if (!SetupKeyboard())
        return false;

#ifdef TARGET_DC
    if (the_input_manager.Init() != DI_OK) {
        ASSERT(false);
        return false;
    }
#endif

    // Create the shell window.
    // Create & register the window class.
    DDLibClass.hInstance = hGlobalThisInst;
    DDLibClass.lpszClassName = TEXT("Urban Chaos");
    DDLibClass.lpfnWndProc = DDLibShellProc;
    DDLibClass.style = 0;
    DDLibClass.hIcon = LoadIcon(hGlobalThisInst, MAKEINTRESOURCE(IDI_ICON2));
    DDLibClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    DDLibClass.lpszMenuName = nullptr;
    DDLibClass.cbClsExtra = 0;
    DDLibClass.cbWndExtra = 0;
    DDLibClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

    RegisterClass(&DDLibClass);

    hDDLibWindow = CreateWindowEx(
        0,
        "Urban Chaos",
        "Urban Chaos",
#ifndef NDEBUG
        WS_OVERLAPPEDWINDOW,
#else
        WS_POPUP,
#endif
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        HWND_DESKTOP,
        nullptr,
        hGlobalThisInst,
        nullptr);

    if (hDDLibWindow) {
        // Init the sound manager.  We're not too fussed about the result.
        MFX_init();

#ifndef TARGET_DC
        // Load the keyboard accelerators.
        hDDLibAccel = LoadAccelerators(hGlobalThisInst, MAKEINTRESOURCE(IDR_MAIN_ACCELERATOR));
#endif

        // Display the window.
        //		ShowWindow(hDDLibWindow,iGlobalWinMode);
        //		UpdateWindow(hDDLibWindow);

        ShellActive = true;
    }

    the_game.DarciStrength = 0;
    the_game.DarciStamina = 0;
    the_game.DarciSkill = 0;
    the_game.DarciConstitution = 0;

    return ShellActive;
}

//---------------------------------------------------------------

void ResetHost() {
    MFX_term();

    ResetKeyboard();
    ResetMemory();

#ifndef NDEBUG
    FiniDebugLog();
#endif

    UnregisterClass(TEXT("Urban Chaos"), GetModuleHandle(nullptr));
}

//---------------------------------------------------------------

void ShellPaused() {
    return;

    std::int32_t timeout;

    if (PauseFlags & PAUSE) {
        PauseFlags |= PAUSE_ACK; // Send acknowledgement,
                                 // do_pause_check:
        timeout = GetTickCount();
        while (PauseFlags & PAUSE) // Wait while paused
        {
            if (Keys[KB_L]) {
                LibShellMessage("ShellPauseOff: Timeout", __FILE__, __LINE__);
            }
            /*
                                    if((GetTickCount()-timeout)>PAUSE_TIMEOUT)
                                    {
                                            g_ShellMessage("ShellPaused: Timeout",__FILE__,__LINE__);
                                            goto	do_pause_check;
                                    }
            */
        }
        PauseFlags |= PAUSE_ACK; // Send acknowledgement,
    }
}

//---------------------------------------------------------------

void ShellPauseOn() {
    the_display.toGDI();
    return;

    std::int32_t timeout;

    PauseCount++;
    if (PauseCount == 1) {
        PauseFlags |= PAUSE; // Set pause flag.
                             // do_ack_check:
        timeout = GetTickCount();
        while (!(PauseFlags & PAUSE_ACK)) // Wait for acknowledgement.
        {
            if (Keys[KB_L]) {
                LibShellMessage("ShellPauseOff: Timeout", __FILE__, __LINE__);
            }
            /*
                                    if((GetTickCount()-timeout)>PAUSE_TIMEOUT)
                                    {
                                            g_ShellMessage("ShellPauseOn: Timeout",__FILE__,__LINE__);
                                            goto	do_ack_check;
                                    }
            */
        }
        PauseFlags &= ~PAUSE_ACK; // Clear ack flag.

        the_display.toGDI();
    }
}

//---------------------------------------------------------------

void ShellPauseOff() {
    return;

    std::int32_t timeout;

    if (PauseCount == 0)
        return;

    if (PauseCount == 1) {
        the_display.fromGDI();

        PauseFlags &= ~PAUSE; // Clear pause flag.
                              // do_ack_check:
        timeout = GetTickCount();
        while (!(PauseFlags & PAUSE_ACK)) // Wait for acknowledgement.
        {
            if (Keys[KB_L]) {
                LibShellMessage("ShellPauseOff: Timeout", __FILE__, __LINE__);
            }
            /*
                                    if((GetTickCount()-timeout)>PAUSE_TIMEOUT)
                                    {
                                            g_ShellMessage("ShellPauseOff: Timeout",__FILE__,__LINE__);
                                            goto	do_ack_check;
                                    }
            */
        }
        PauseFlags &= ~PAUSE_ACK; // Clear ack flag.
    }
    PauseCount--;
}

//---------------------------------------------------------------
extern void ClearLatchedKeys();

extern std::int32_t app_inactive;
extern std::int32_t restore_surfaces;

bool LibShellActive() {
    std::int32_t result;
    MSG msg;

    //
    // release any remembered keys
    //
    ClearLatchedKeys();

    while (1) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
            result = (std::int32_t) GetMessage(&msg, nullptr, 0, 0);
#ifndef TARGET_DC
            if (result) {
                if (!TranslateAccelerator(hDDLibWindow, hDDLibAccel, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            } else {
                ShellActive = false;
            }
#endif
        }

        if (app_inactive && the_display.lp_DD4 && the_display.IsFullScreen()) {
            Sleep(100);
        } else {
            break;
        }
    }

    if (restore_surfaces) {
        if (the_display.lp_DD4) {
            the_display.lp_DD4->RestoreAllSurfaces();

            extern void FRONTEND_restore_screenfull_surfaces();

            FRONTEND_restore_screenfull_surfaces();
        }

        restore_surfaces = false;
    }

    return ShellActive;
}

//---------------------------------------------------------------

bool LibShellChanged() {
    if (the_display.IsDisplayChanged()) {
        the_display.DisplayChangedOff();
        return true;
    }
    return false;
}

//---------------------------------------------------------------

bool LibShellMessage(const char *pMessage, const char *pFile, std::uint32_t dwLine) {
    bool result;
    char buff1[512],
        buff2[512];
    std::uint32_t flag;

    if (!pMessage) {
        pMessage = "Looks like a coder has caught a bug.";
    }

    TRACE("LbShellMessage: %s\n", pMessage);

#ifndef TARGET_DC
    wsprintf(buff1, "Uh oh, something bad's happened!");
    wsprintf(buff2, "%s\n\n%s\n\nIn   : %s\nline : %u", buff1, pMessage, pFile, dwLine);
    strcat(buff2, "\n\nAbort=Kill Application, Retry=Debug, Ignore=Continue");
    flag = MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_DEFBUTTON3;

    result = false;
    the_display.toGDI();
    switch (MessageBox(hDDLibWindow, buff2, "Mucky Foot Message", flag)) {
    case IDABORT:
        exit(1);
        break;
    case IDCANCEL:
    case IDIGNORE:
        break;
    case IDRETRY:
        DebugBreak();
        break;
    }

    the_display.fromGDI();
#else
    result = true;
#endif

    return result;
}

//---------------------------------------------------------------

void Time(MFTime *the_time) {
    SYSTEMTIME new_time;

    GetLocalTime(&new_time);
    the_time->Hours = new_time.wHour;
    the_time->Minutes = new_time.wMinute;
    the_time->Seconds = new_time.wSecond;
    the_time->MSeconds = new_time.wMilliseconds;
    the_time->DayOfWeek = new_time.wDayOfWeek;
    the_time->Day = new_time.wDay;
    the_time->Month = new_time.wMonth;
    the_time->Year = new_time.wYear;
    the_time->Ticks = GetTickCount(); // Identified as an overflow issue that occurs every 49 days
}

//---------------------------------------------------------------
//
// WinMain - Entry point for windows application.
//
//---------------------------------------------------------------

static std::uint16_t argc;
static LPTSTR argv[MAX_PATH];

#ifdef TARGET_DC
// Include this again in just one file - this one.
#include "dtags.h"
#endif

void init_best_found();

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPTSTR lpszArgs, int iWinMode) {
    // Store WinMain parameters.
#ifdef TARGET_DC
    // This malloc has to be a malloc, not a MemAlloc - the heap has not yet been set up.
    lpszGlobalArgs = (char *) malloc((_tcslen(lpszArgs) + 1) * sizeof(*lpszGlobalArgs));
    ASSERT(lpszGlobalArgs != nullptr);
    textConvertUniToChar(lpszGlobalArgs, lpszArgs);
#else
    lpszGlobalArgs = lpszArgs;
#endif
    iGlobalWinMode = iWinMode;
    hGlobalThisInst = hThisInst;
    hGlobalPrevInst = hPrevInst;

    init_best_found();

#ifdef TARGET_DC
    // Init the DTrace logging.
    LOG_INIT
#endif

#if 0 // Someone already done it! :)
#ifdef NDEBUG

	//
	// So you can't have multiple release builds of fallen running at once!
	//

	CreateMutex(nullptr, true, "This is your friendly Urban Chaos mutex!");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//
		// Fallen is already running!
		//

		return 0;
	}

#endif
#endif

#ifndef FINAL
//	extern void CONSOLE_TCP();
//	CONSOLE_TCP();
#endif

    // try and create an event
    // this always succeeds
    // if event existed before (still succeeds) and ERROR_ALREADY_EXISTS is returned, so die
    // note the event is automatically deleted by the system when the app exits (even if it crashes)

#ifdef FINAL
    HANDLE hEvent = CreateEventA(nullptr, false, false, "UrbanChaosExclusionZone");
    if (GetLastError() != ERROR_ALREADY_EXISTS)
#endif
    {
        return MF_main(argc, argv);
    }

    return ERROR_ALREADY_EXISTS;
}

//---------------------------------------------------------------
