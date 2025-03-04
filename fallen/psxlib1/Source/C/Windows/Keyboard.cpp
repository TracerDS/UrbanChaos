// Keyboard.cpp
// Guy Simmons, 11th February 1997.


#include	<MFHeader.h>

volatile std::uint8_t	AltFlag,
				ControlFlag,
				ShiftFlag;
volatile std::uint8_t	Keys[256],
				LastKey;
HHOOK KeyboardHook;


//---------------------------------------------------------------

bool SetupKeyboard()
{
	AltFlag		=	0;
	ControlFlag	=	0;
	ShiftFlag	=	0;
	LastKey		=	0;
	memset((char*)&Keys[0],0,256);

#ifdef	_RELEASE
	KeyboardHook	=	NULL;
/*
	KeyboardHook	=	SetWindowsHookEx(
											WH_KEYBOARD,
											(HOOKPROC)KeyboardProc,
											NULL,
											0
										);
	ERROR_MSG(KeyboardHook,"Can't setup the keyboard.")
	if(KeyboardHook==NULL)
	{
		// Unable to set up keyboard.
		return	false;
	}
*/
#endif
	return	true;
}

//---------------------------------------------------------------

void ResetKeyboard()
{
#ifdef	_RELEASE
	if(KeyboardHook)
		UnhookWindowsHookEx(KeyboardHook);
#endif
}

//---------------------------------------------------------------

#define	KEYMASK_REPEAT		(0x0000ffff)
#define	KEYMASK_SCAN		(0x00ff0000)
#define	KEYMASK_EXTENDED	(0x01000000)
#define	KEYMASK_RESERVED	(0x1e000000)
#define	KEYMASK_CONTEXT		(0x20000000)
#define	KEYMASK_PSTATE		(0x40000000)
#define	KEYMASK_TSTATE		(0x80000000)

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	std::uint8_t		key_code;
	std::uint32_t		virtual_keycode	=	wParam;


	if(code<0)
	{
		return	CallNextHookEx(KeyboardHook,code,wParam,lParam);
	}

	// Get key scan code.
	key_code	=	(std::uint8_t)((lParam&KEYMASK_SCAN)>>16);

	// Extended key press?
	if(lParam&KEYMASK_EXTENDED)
	{
		key_code	+=	0x80;
	}

	// Key up?
	if(lParam&KEYMASK_TSTATE)
	{
		Keys[key_code]	=	0;
	}
	// No ,down.
	else
	{
		Keys[key_code]	=	1;
		LastKey			=	key_code;
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

	return	false;
}

//---------------------------------------------------------------

