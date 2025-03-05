//	MapExitSetup.cpp
//	Matthew Rosenfeld, 7th October 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"
#include "fmatrix.h"
#include "inline.h"
#include "gi.h"
#include "EdStrings.h"

#include "GEdit.h"

//---------------------------------------------------------------

char *filename;

//---------------------------------------------------------------

bool CALLBACK mapexit_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HWND the_ctrl;
    std::int32_t c0 = 0;

    switch (message) {
        case WM_INITDIALOG:
            DlgDirListComboBox(hWnd, "levels\\*.ucm", IDC_COMBO1, 0, DDL_ARCHIVE);
            the_ctrl = GetDlgItem(hWnd, IDC_COMBO1);
            c0 = SendMessage(the_ctrl, CB_FINDSTRINGEXACT, -1, (long) filename);
            if (c0 == CB_ERR)
                c0 = SendMessage(the_ctrl, CB_ADDSTRING, 0, (long) filename);
            if (c0 != CB_ERR)
                SendMessage(the_ctrl, CB_SETCURSEL, c0, 0);

            return true;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    the_ctrl = GetDlgItem(hWnd, IDC_COMBO1);
                    SendMessage(the_ctrl, WM_GETTEXT, _MAX_PATH, (long) filename);
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

void do_mapexit_setup(EventPoint *the_ep) {
    filename = (char *) the_ep->Data[0];
    if (!filename) {
        filename = (char *) malloc(_MAX_PATH);
        ZeroMemory(filename, _MAX_PATH);
    }
    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_MAPEXIT_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) mapexit_proc);
    the_ep->Data[0] = (std::int32_t) filename;
}

//---------------------------------------------------------------

char *get_mapexit_message(EventPoint *ep, char *msg) {
    if ((!ep) || (!ep->Data[0]))
        strcpy(msg, "Unknown");
    else
        strcpy(msg, (char *) ep->Data[0]);
    return msg;
}
