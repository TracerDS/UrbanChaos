//	WaypointSetup.cpp
//	Matthew Rosenfeld, 11th November 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"
#include "GEditEdStrings.h"

#include "GEdit.h"

//---------------------------------------------------------------

std::int32_t waypoint_delay;

//---------------------------------------------------------------

bool CALLBACK ways_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HWND the_ctrl;

    switch (message) {
    case WM_INITDIALOG:
        SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETRANGE, 0, MAKELONG(0, 10000));
        SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETPOS, 0, waypoint_delay);
        return true;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            waypoint_delay = SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS, 0, 0);
        case IDCANCEL:
            EndDialog(hWnd, 0);
            return true;
        }
        break;
    }
    return false;
}

//---------------------------------------------------------------

void do_waypoint_setup(EventPoint *the_ep) {
    waypoint_delay = the_ep->Data[0];
    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_WAYPOINT_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) ways_proc);

    the_ep->Data[0] = waypoint_delay;
}

//---------------------------------------------------------------

char *get_waypoint_message(EventPoint *ep, char *msg) {
    float f;

    if ((!ep) || !ep->Data[0])
        msg[0] = 0;
    else {
        f = ep->Data[0];
        f /= 10;
        sprintf(msg, "%.1f seconds delay", f);
    }
    return msg;
}
