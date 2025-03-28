//	NavSetup.cpp
//	Matthew Rosenfeld, 28 Apr 1998.

#include <MFStdLib.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include "inline.h"

#include "EdStrings.h"
#include "GEdit.h"

//---------------------------------------------------------------

char* nav_text;
std::int32_t nav_person;

extern char* WaypointExtra(EventPoint* ep, char* msg);

//---------------------------------------------------------------

#define STR_LEN 800

bool CALLBACK nav_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::int32_t len;
    EventPoint *ep_ptr, *ep_base = current_mission->EventPoints;
    char msg[STR_LEN], str[STR_LEN];
    HWND the_ctrl, the_ctrl2;
    std::int32_t ep;
    std::int32_t c0 = 0;

    switch (message) {
    case WM_INITDIALOG:
        //	Set up the edit text.
        SendMessage(
            GetDlgItem(hWnd, IDC_EDIT1),
            EM_SETLIMITTEXT,
            2550, 0);

        SendMessage(
            GetDlgItem(hWnd, IDC_EDIT1),
            WM_SETTEXT,
            0, (LPARAM) nav_text);

        // fill the two list boxes
        the_ctrl = GetDlgItem(hWnd, IDC_COMBO1);
        //			the_ctrl2	=	GetDlgItem(hWnd,IDC_COMBO2);
        ep = current_mission->UsedEPoints;
        c0 = 0;
        while (ep) {
            ep_ptr = TO_EVENTPOINT(ep_base, ep);
            if ((ep_ptr->WaypointType == WPT_CREATE_PLAYER) || (ep_ptr->WaypointType == WPT_CREATE_ENEMIES) || (ep_ptr->WaypointType == WPT_CREATE_CREATURE)) {
                WaypointExtra(ep_ptr, msg);
                sprintf(str, "%d%c: %s", ep, 'A' + ep_ptr->Group, msg);
                SendMessage(the_ctrl, CB_ADDSTRING, 0, (LPARAM) str);
                //					SendMessage(the_ctrl2,CB_ADDSTRING,0,(LPARAM)str);
                if (ep == nav_person) SendMessage(the_ctrl, CB_SETCURSEL, c0, 0);
                //					if (ep==converse_p2) SendMessage(the_ctrl2,CB_SETCURSEL,c0,0);
                c0++;
            }

            ep = ep_ptr->Next;
        }

        SetFocus(GetDlgItem(hWnd, IDC_EDIT1));

        return false;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            return true;
        }
        break;

    case WM_CLOSE:
        len = SendMessage(
                  GetDlgItem(hWnd, IDC_EDIT1),
                  WM_GETTEXTLENGTH,
                  0, 0) +
              1;
        if (nav_text) free(nav_text);
        nav_text = (char*) malloc(len);
        ZeroMemory(nav_text, len);

        SendMessage(
            GetDlgItem(hWnd, IDC_EDIT1),
            WM_GETTEXT,
            len,
            (LPARAM) nav_text);

        nav_person = SendMessage(
            GetDlgItem(hWnd, IDC_COMBO1),
            CB_GETCURSEL,
            0, 0);
        // now translate phoney people indices to real one
        if (nav_person == -1) {
            nav_person = 0;
        } else {
            memset(msg, 0, STR_LEN);
            SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETLBTEXT, nav_person, (long) msg);
            sscanf(msg, "%d", &nav_person);
        }

        EndDialog(hWnd, 0);
        return true;
    }
    return false;
}

//---------------------------------------------------------------

void do_nav_setup(EventPoint* the_ep) {
    //	Set the dialog.
    nav_text = (char*) the_ep->Data[0];
    nav_person = the_ep->Data[1];
    if (!nav_text) {
        nav_text = (char*) malloc(STR_LEN);
        ZeroMemory(nav_text, STR_LEN);
        SetEPTextID(the_ep);
    }

    //	Do the dialog.
    DialogBox(
        GEDIT_hinstance,
        MAKEINTRESOURCE(IDD_NAVBEACON_SETUP),
        GEDIT_view_wnd,
        (DLGPROC) nav_proc);

    //	Get the data.
    the_ep->Data[0] = (std::int32_t) nav_text; //
    the_ep->Data[1] = nav_person;
}

//---------------------------------------------------------------

/*
char* get_message_message(EventPoint *ep, char* msg) {
        msg[0]=0;
        if (ep&&ep->Data[0])
                strcpy(msg,(char*)ep->Data[0]);
        return msg;
}
*/