//
// widget.h
// Matthew Rosenfeld	24 February 1999
//
// widget provides windows-esque controls such as buttons, checkboxes and stuff
// grouped together into 'forms' (palmpilot style)
//

//
// of course, you _do_ realise this is a fuck of a lot easier with C++ right?
//
// i knew you did.
//

#include "MFStdLib.h"

//----------------------------------------------------------------------------
// Flags and other misc defines
//

#define WIDGET_STATE_FOCUS 1
#define WIDGET_STATE_DISABLED 2
#define WIDGET_STATE_ACTIVATED 4
#define WIDGET_STATE_BLOCKFOCUS 8
#define WIDGET_STATE_ALIGNLEFT 16
#define WIDGET_STATE_SHRINKTEXT 32

#define WS_MOVE 1
#define WS_FADEIN 2
#define WS_FADEOUT 3
#define WS_OK 4
#define WS_FAIL 5
#define WS_BLIP 6

//----------------------------------------------------------------------------
// General type declarations
//

class Widget;
class Form;

typedef void (*WIDGET_Void)(Widget *widget);
typedef bool (*WIDGET_Clik)(Widget *widget, std::int32_t x, std::int32_t y);
typedef bool (*WIDGET_Char)(Widget *widget, char key);
typedef std::int32_t (*WIDGET_Data)(Widget *widget, std::int32_t code, std::int32_t data1, std::int32_t data2);
typedef bool (*FORM_Proc)(Form *form, Widget *widget, std::int32_t message);

//----------------------------------------------------------------------------
// Widget Structures
//

struct Methods {
    WIDGET_Void Init, Free, Draw, Push;
    WIDGET_Char Char;
    WIDGET_Data Data;
    WIDGET_Clik HitTest;
};

struct WidgetPoint {
    std::int32_t x, y;
};

class Widget { // C++ Now!

   public:
    std::int32_t x, y, ox, oy; // parent-relative coords
    std::int32_t tag;          // a value for its parent to use as it sees fit
    std::int32_t state;        // focused, etc
    std::int32_t data[5];      // widget-specific data
    char *caption;             // text label as appropriate
    Form *form;                // parent form
    Methods *methods;          // pointer to its virtual method table (muhahaha)
    Widget *prev, *next;       // linked list pointers
};

class Form {
   public:
    std::int32_t x, y, ox, oy; // screen coords
    std::uint32_t textcolour;  // rgb for text
    std::int32_t returncode;   // if !0 then form returns
    std::int32_t age;          // for fading in forms
    std::int32_t inverse;      // bet this gets turned into flags later
    Widget *children;          // child widgets
    Widget *focus;             // current focus control
    FORM_Proc proc;            // dialog procedure
    char caption[32];          // form text
};

//----------------------------------------------------------------------------
// Externally-called functions
//

// Forms
Form *FORM_Create(char *caption, FORM_Proc proc, std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, std::uint32_t textcolour);
Widget *FORM_AddWidget(Form *form, Widget *widget);
void FORM_DelWidget(Widget *widget);
std::int32_t FORM_Process(Form *form);
void FORM_Draw(Form *form);
void FORM_Free(Form *form);
Widget *FORM_Focus(Form *form, Widget *widget, std::int8_t direction = 0);
Widget *FORM_GetWidgetFromPoint(Form *form, WidgetPoint pt);

// Widgets
void WIDGET_menu(Form *form, ...);
Widget *WIDGET_Create(Methods *widget_class, std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, char *caption);
// void	WIDGET_State(Widget *widget, std::int32_t data, std::int32_t mask=0xFFFF);
inline void WIDGET_SetState(Widget *widget, std::int32_t data, std::int32_t mask = 0) {
    widget->state = (widget->state & ~mask) | data;
}
inline void WIDGET_ToggleState(Widget *widget, std::int32_t data) {
    widget->state ^= data;
}
inline std::int32_t WIDGET_State(Widget *widget, std::int32_t mask = 0xFFFF) {
    return (widget->state & mask);
}

// Utility
inline WidgetPoint TO_WIDGETPNT(std::int32_t x, std::int32_t y) {
    WidgetPoint pt = {x, y};
    return pt;
}
inline WidgetPoint FORM_To_Screen(Form *form, WidgetPoint pt) {
    return TO_WIDGETPNT(pt.x + form->x, pt.y + form->y);
}
inline WidgetPoint WIDGET_Centre(Widget *widget) {
    return FORM_To_Screen(widget->form, TO_WIDGETPNT((widget->x + widget->ox) >> 1, (widget->y + widget->oy) >> 1));
}
void WIDGET_snd(std::int32_t snd);

//----------------------------------------------------------------------------
// Widget "class library"
//

extern Methods BUTTON_Methods;
extern Methods STATIC_Methods;
extern Methods CHECK_Methods;
extern Methods RADIO_Methods;
extern Methods INPUT_Methods;
extern Methods LISTS_Methods;
extern Methods TEXTS_Methods;
extern Methods GLYPH_Methods;
extern Methods SHADE_Methods;

//----------------------------------------------------------------------------
// Widget messages
//
// WxN_* -- Widget <whatever> Notification -- sent to dlg proc
// WxM_* -- Widget <whatever> Message      -- sent to widgets

#define WBN_PUSH 1
#define WIN_ENTER 1
#define WLN_ENTER 1
#define WFN_CHAR 1
#define WFN_FOCUS 2
#define WFN_PAINT 3

#define WLM_ADDSTRING 1
#define WIM_SETSTRING 1
#define WIM_SETMODE 2
#define WTM_ADDSTRING 1
#define WTM_ADDBLOCK 2
