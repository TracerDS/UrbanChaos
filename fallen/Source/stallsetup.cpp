//	WptSetup.cpp
//	Matthew Rosenfeld 04th March 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"

#include "EdStrings.h"
#include "GEdit.h"
#include "ticklist.h"

//---------------------------------------------------------------

static std::int32_t which_vehicle;

//---------------------------------------------------------------

#define INIT_COMBO_BOX(i, s, d)                                      \
    the_ctrl = GetDlgItem(hWnd, i);                                  \
    c0 = 1;                                                          \
    lbitem_str = s[0];                                               \
    while (*lbitem_str != '!') {                                     \
        SendMessage(the_ctrl, CB_ADDSTRING, 0, (LPARAM) lbitem_str); \
        lbitem_str = s[c0++];                                        \
    }                                                                \
    SendMessage(the_ctrl, CB_SETCURSEL, d, 0);

//---------------------------------------------------------------

bool CALLBACK stall_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::int32_t c0 = 0;
    HWND the_ctrl;
    LPTSTR lbitem_str;

    switch (message) {
    case WM_INITDIALOG:
        //	Set up the 'speed' spin.
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN1),
            UDM_SETRANGE,
            0,
            MAKELONG(2048, 1));
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN1),
            UDM_SETPOS,
            0,
            MAKELONG(which_vehicle, 0));

        return true;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            which_vehicle = SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS, 0, 0);

        case IDCANCEL:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            return true;
        }
        break;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return true;
    }
    return false;
}

//---------------------------------------------------------------

void do_stall_setup(EventPoint *the_ep) {
    //	Set the dialog.
    which_vehicle = the_ep->Data[0];

    //	Do the dialog.

    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_STALL_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) stall_proc);

    //	Get the data.
    the_ep->Data[0] = which_vehicle;
}

//---------------------------------------------------------------
