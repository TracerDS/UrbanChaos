//	BarrelSetup.cpp
//	Matthew Rosenfeld 04th March 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"

#include "GEditEdStrings.h"
#include "GEdit.h"
#include "ticklist.h"

//---------------------------------------------------------------

std::int32_t barrel_type;

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

bool CALLBACK barrel_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::int32_t c0 = 0;
    HWND the_ctrl;
    LPTSTR lbitem_str;

    switch (message) {
    case WM_INITDIALOG:
        INIT_COMBO_BOX(IDC_COMBO1, wbarrel_type_strings, barrel_type);
        return true;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            barrel_type = SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);

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

void do_barrel_setup(EventPoint* the_ep) {
    //	Set the dialog.
    barrel_type = the_ep->Data[0];

    //	Do the dialog.

    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_BARREL_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) barrel_proc);

    //	Get the data.
    the_ep->Data[0] = barrel_type;
}

//---------------------------------------------------------------
