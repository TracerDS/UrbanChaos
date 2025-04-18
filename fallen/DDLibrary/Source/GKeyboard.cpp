// Keyboard.cpp
// Guy Simmons, 11th February 1997.

#include "DDLib.h"

#ifdef TARGET_DC
volatile std::uint8_t Keys[256],
    LastKey;
#else
volatile std::uint8_t AltFlag,
    ControlFlag,
    ShiftFlag;
volatile std::uint8_t Keys[256],
    LastKey;
#endif

std::uint8_t key_turn[256];

#define MAX_RELEASE 10
std::uint8_t Released[MAX_RELEASE];
std::int16_t release_count = 0;
std::uint8_t game_turn = 0;

HHOOK KeyboardHook;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------

bool SetupKeyboard() {
#ifndef TARGET_DC
    AltFlag = 0;
    ControlFlag = 0;
    ShiftFlag = 0;
#endif
    LastKey = 0;
    memset((char*) &Keys[0], 0, 256);
    memset((char*) &key_turn[0], 0, 256);

#ifdef _RELEASE
    KeyboardHook = nullptr;
/*
        KeyboardHook	=	SetWindowsHookEx(
                                                                                        WH_KEYBOARD,
                                                                                        (HOOKPROC)KeyboardProc,
                                                                                        nullptr,
                                                                                        0
                                                                                );
        ERROR_MSG(KeyboardHook,"Can't setup the keyboard.")
        if(KeyboardHook==nullptr)
        {
                // Unable to set up keyboard.
                return	false;
        }
*/
#endif
    return true;
}

//---------------------------------------------------------------

void ResetKeyboard() {
#if defined(_RELEASE) && !defined(TARGET_DC)
    if (KeyboardHook)
        UnhookWindowsHookEx(KeyboardHook);
#endif
}

//---------------------------------------------------------------

#define KEYMASK_REPEAT (0x0000ffff)
#define KEYMASK_SCAN (0x00ff0000)
#define KEYMASK_EXTENDED (0x01000000)
#define KEYMASK_RESERVED (0x1e000000)
#define KEYMASK_CONTEXT (0x20000000)
#define KEYMASK_PSTATE (0x40000000)
#define KEYMASK_TSTATE (0x80000000)

//
// set game flags from keyboard state array
//

inline void SetFlagsFromKeyArray() {
#ifndef TARGET_DC
    AltFlag = Keys[KB_LALT] || Keys[KB_RALT];
    ControlFlag = Keys[KB_LCONTROL]; // || Keys[KB_RCONTROL];
    ShiftFlag = Keys[KB_LSHIFT] || Keys[KB_RSHIFT];
#endif
}

//
// KeyboardProc for WM_KEYUP & WM_KEYDOWN
//

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    std::uint8_t key_code;
    std::uint32_t virtual_keycode = wParam;

#ifndef TARGET_DC
    if (code < 0) {
        return CallNextHookEx(KeyboardHook, code, wParam, lParam);
    }
#endif

    // Get key scan code.
    key_code = (std::uint8_t)((lParam & KEYMASK_SCAN) >> 16);

    // Extended key press?
    if (lParam & KEYMASK_EXTENDED) {
        key_code += 0x80;
    }

    // Key up?
    if (lParam & KEYMASK_TSTATE) {
        if (key_turn[key_code] == game_turn && release_count < MAX_RELEASE) {
            //
            // released the same game turn it was pressed  (1 in 256 chance of doing this in error)
            //

            //
            // remember which keys have been release so you can clear them next gameturn
            //
            Released[release_count++] = key_code;
        } else

        {
            Keys[key_code] = 0;
        }
    }
    // No ,down.
    else {
#if 0
		// only set key down bit if it's been pressed, rather than
		// autorepeating
		if (!(lParam & KEYMASK_PSTATE))
		{
			key_turn[key_code]	=	game_turn;
			Keys[key_code]		=	1;
			LastKey				=	key_code;
		}
#else // oops ... the level editor needs autorepeat keys
      // better get Mark to fix his own bugs from now on
        key_turn[key_code] = game_turn;
        Keys[key_code] = 1;
        LastKey = key_code;
#endif
    }

    SetFlagsFromKeyArray();

    return false;
}

//
// clear any latched keys
//

void ClearLatchedKeys() {
    game_turn++;

    while (release_count) {
        release_count--;
        Keys[Released[release_count]] = 0;
    }

    ASSERT(release_count == 0);

    SetFlagsFromKeyArray();
}
