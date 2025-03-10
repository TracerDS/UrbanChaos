//	VfxSetup.cpp
//	Guy Simmons, 3rd September 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"
#include "fmatrix.h"
#include "inline.h"
#include <Ledit/Headers/gi.h>

#include "EdStrings.h"
#include "GEdit.h"
#include "ticklist.h"

//---------------------------------------------------------------

std::int32_t vfx_types, vfx_scale;

//---------------------------------------------------------------

bool CALLBACK vfx_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::int32_t c0 = 0;
    HWND the_ctrl;
    LPTSTR lbitem_str;

    switch (message) {
        case WM_INITDIALOG:
            ticklist_init(hWnd, IDC_LIST1, wvfx_strings, vfx_types);

            SendMessage(GetDlgItem(hWnd, IDC_SPIN1),
                        UDM_SETRANGE,
                        0,
                        MAKELONG(1024, 0));
            SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETPOS, 0, MAKELONG(vfx_scale, 0));

            return true;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    vfx_types = ticklist_bitmask(hWnd, IDC_LIST1);
                    vfx_scale = SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS, 0, 0);

                case IDCANCEL:
                    ticklist_close(hWnd, IDC_LIST1);
                    EndDialog(hWnd, 0);
                    return true;
            }
            break;

        case WM_MEASUREITEM:
            return ticklist_measure(hWnd, wParam, lParam);
        case WM_DRAWITEM:
            return ticklist_draw(hWnd, wParam, lParam);
    }
    return false;
}

//---------------------------------------------------------------

void do_vfx_setup(EventPoint *the_ep) {
    //	Set the dialog.
    vfx_types = the_ep->Data[0];
    vfx_scale = the_ep->Data[1];

    //	Do the dialog.
    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_VEFFECT_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) vfx_proc);

    //	Get the data.
    the_ep->Data[0] = vfx_types;
    the_ep->Data[1] = vfx_scale;
}

//---------------------------------------------------------------
