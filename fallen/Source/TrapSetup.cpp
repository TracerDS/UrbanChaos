//	TrapSetup.cpp
//	Matthew Rosenfeld, 15th October 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <ddlib.h>
#include <commctrl.h>
#include "resource.h"
#include "inline.h"
#include "gi.h"
#include "ticklist.h"
#include "EdStrings.h"
#include "GEdit.h"

//---------------------------------------------------------------

std::int32_t trap_type, trap_speed, trap_steps, trap_mask, trap_axis, trap_range;

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

void InitSteps(HWND hWnd, char steps, std::int32_t mask) {
    char i, j;
    std::int32_t c0 = 1;
    HWND the_ctrl = GetDlgItem(hWnd, IDC_LIST1);
    char lbitem_str[300];
    SendMessage(the_ctrl, LB_RESETCONTENT, 0, 0);
    for (i = 0; i < steps; i++) {
        j = i + 1;
        sprintf(lbitem_str, "Step %d", j);
        SendMessage(the_ctrl, LB_ADDSTRING, 0, (LPARAM) lbitem_str);
        SendMessage(the_ctrl, LB_SETITEMDATA, i, (mask & (1 << (i))) ? 1 : 0);
    }
}

char* blank_string[] = {"!"};

bool CALLBACK traps_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::int32_t c0 = 0;
    HWND the_ctrl;
    LPTSTR lbitem_str;
    NM_UPDOWN* lp_ntfy;

    switch (message) {
    case WM_INITDIALOG:
        //	Set up the combo box.
        INIT_COMBO_BOX(IDC_COMBO1, wtraptype_strings, trap_type);
        INIT_COMBO_BOX(IDC_COMBO2, wtrapaxis_strings, trap_axis);

        ticklist_init(hWnd, IDC_LIST1, blank_string, 0);

        InitSteps(hWnd, trap_steps, trap_mask);

        //	Set up the 'speed' spin.
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN1),
            UDM_SETRANGE,
            0,
            MAKELONG(32, 1));
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN1),
            UDM_SETPOS,
            0,
            MAKELONG(trap_speed, 0));

        //	Set up the 'steps' spin.
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN2),
            UDM_SETRANGE,
            0,
            MAKELONG(32, 1));
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN2),
            UDM_SETPOS,
            0,
            MAKELONG(trap_steps, 0));

        //	Set up the 'range' spin.
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN3),
            UDM_SETRANGE,
            0,
            MAKELONG(32, 1));
        SendMessage(
            GetDlgItem(hWnd, IDC_SPIN3),
            UDM_SETPOS,
            0,
            MAKELONG(trap_range, 0));

        return true;

    case WM_MEASUREITEM:
        return ticklist_measure(hWnd, wParam, lParam);
    case WM_DRAWITEM:
        return ticklist_draw(hWnd, wParam, lParam);

    case WM_NOTIFY:
        lp_ntfy = (NM_UPDOWN*) lParam;

        //	Make the 'constitution' spin go up/down in steps of 5.
        if (lp_ntfy->hdr.idFrom == IDC_SPIN2 && lp_ntfy->hdr.code == UDN_DELTAPOS) {
            InitSteps(hWnd, lp_ntfy->iPos + lp_ntfy->iDelta, ticklist_bitmask(hWnd, IDC_LIST1));
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            trap_type = SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);
            trap_axis = SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_GETCURSEL, 0, 0);
            trap_speed = SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS, 0, 0);
            trap_steps = SendMessage(GetDlgItem(hWnd, IDC_SPIN2), UDM_GETPOS, 0, 0);
            trap_mask = ticklist_bitmask(hWnd, IDC_LIST1);
            trap_range = SendMessage(GetDlgItem(hWnd, IDC_SPIN3), UDM_GETPOS, 0, 0);
        case IDCANCEL:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            return true;
        }
        break;

    case WM_CLOSE:
        ticklist_close(hWnd, IDC_LIST1);
        EndDialog(hWnd, 0);
        return true;
    }
    return false;
}

//---------------------------------------------------------------

void do_trap_setup(EventPoint* the_ep) {
    if (!the_ep) return;
    //	Set the dialog.
    trap_type = the_ep->Data[0];
    trap_speed = the_ep->Data[1];
    trap_steps = the_ep->Data[2];
    trap_mask = the_ep->Data[3];
    trap_axis = the_ep->Data[4];
    trap_range = the_ep->Data[5];
    if (!trap_steps) trap_steps = 1;

    //	Do the dialog.
    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_TRAP_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) traps_proc);

    //	Set the data.
    the_ep->Data[0] = trap_type;
    the_ep->Data[1] = trap_speed;
    the_ep->Data[2] = trap_steps;
    the_ep->Data[3] = trap_mask;
    the_ep->Data[4] = trap_axis;
    the_ep->Data[5] = trap_range;
}

//---------------------------------------------------------------

char* get_trap_message(EventPoint* ep, char* msg) {
    if (!ep)
        strcpy(msg, "Unknown");
    else
        strcpy(msg, wtraptype_strings[ep->Data[0]]);
    return msg;
}
