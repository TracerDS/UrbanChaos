// propedit.h
// windowsy Property Editor
// you must create a window List View (NOT a list _box_) on your
// dialog like the one in the cutscene editor in order for it to work

#include "MFStdLib.h"
#include "windows.h"
#include "commctrl.h"

#define PROPTYPE_STRING (0)
#define PROPTYPE_INT (1)
#define PROPTYPE_BOOL (2)
#define PROPTYPE_MULTI (3)
#define PROPTYPE_READONLY (4)
#define PROPTYPE_BUTTON (5)

#define UM_DROP (WM_USER + 1)

#define TBCB_DBLCLK (1)
#define TBCB_DRAG (2)

#define TLCB_GETBARINFO (1)
#define TLCB_SELECT (2)

#define PECB_UPDATE (1)
#define PECB_BUTTON (2)
#define PECB_EDITMODE (3)

class PropertyEditor;
class TreeBrowser;
class DragServer;
class TimeLine;
class TimeLineRuler;
class TimeLineScroll;

// typedef void (*PROPEDIT_notify)(PropertyEditor *pe);

typedef bool (*TreeBrowserCB)(TreeBrowser *tb, int reason, int index, HTREEITEM item, char *txt);

typedef bool (*TimeLineCB)(TimeLine *tb, int reason, int index, int subline, int cell);

typedef bool (*PropEditCB)(PropertyEditor *tb, int reason, int index, char *value);

class GadgetBase {
   public:
    HWND hWnd;
    void Repaint();
};

class PropertyEditor : public GadgetBase {
   private:
    int property_count;
    PropEditCB callback;

   public:
    PropertyEditor(HWND nhWnd);

    void Clear();
    int Add(char *name, char *value, std::uint8_t type);
    bool Process(HWND parent, WPARAM wParam, LPARAM lParam);
    int Type(std::uint16_t index);
    void Update(std::uint16_t index, char *value);
    bool Verify(std::uint8_t type, char *value);
    void SetCallback(PropEditCB cb);
};

class TreeBrowser : public GadgetBase {
   private:
    HTREEITEM itemstack[10];
    DragServer *drag;
    int selection;
    HIMAGELIST image_list;
    TreeBrowserCB callback;

   public:
    TVITEM drag_item;
    TreeBrowser(HWND nhWnd);
    ~TreeBrowser();
    HTREEITEM Add(char *name, HTREEITEM parent, std::uint8_t indent, std::int32_t param, std::int32_t img);
    int AddDir(char *path, bool subdirs, HTREEITEM parent, std::uint8_t indent, std::int32_t param, std::int32_t img, std::int32_t imgfld);
    bool Process(HWND parent, WPARAM wParam, LPARAM lParam);
    void SetImageList(HINSTANCE inst, std::int32_t idx);
    void SetDraggable(DragServer *ndrag);
    void SetCallback(TreeBrowserCB cb);
    int GetSelection();
    int GetImageFromItem(HTREEITEM hItem);
    void GetTextFromItem(HTREEITEM hItem, char *txt, int max);
    HTREEITEM GetItemFromOffset(HTREEITEM hItem, int ofs);
    HTREEITEM GetChildFromItem(HTREEITEM hItem, int ofs);
    void Clear();
};

class DragServer {
   private:
    HCURSOR cursor;
    HWND source, parent, target;

   public:
    DragServer(HWND nparent, HINSTANCE inst);
    ~DragServer();
    bool Process(UINT message, WPARAM wParam, LPARAM lParam);
    void Begin(HWND src);
    HWND Target() { return target; };
};

class TimeLine : public GadgetBase {
   private:
    HIMAGELIST image_list;
    int read_head;
    int scroll_offset;
    TimeLineRuler *ruler;
    TimeLineScroll *scroll;
    TimeLineCB callback;

   public:
    TimeLine(HWND nhWnd, TimeLineRuler *nrule, TimeLineScroll *nscroll);
    ~TimeLine();
    int GetWidth();
    void Measure(LPARAM lParam);
    void Add(char *str);
    void Del(std::uint16_t index);
    void MarkEntry(std::uint16_t index, std::uint16_t start, std::uint16_t length, std::uint8_t which);
    void Draw(LPARAM lParam);
    bool Process(HWND parent, WPARAM wParam, LPARAM lParam);
    void SetImageList(HINSTANCE inst, std::int32_t idx);
    void SetReadHead(int newpos);
    int GetReadHead() { return read_head; };
    void SetScrollPos(int newpos);
    int GetScrollPos() { return scroll_offset; };
    int GetCellFromX(int x);
    int GetRowFromY(int y);
    int GetSelectedRow();
    void SetCallback(TimeLineCB cb) { callback = cb; };
    char *GetText(int chan, char *buf);
};

class TimeLineRuler : public GadgetBase {
   private:
    TimeLine *owner;

   public:
    TimeLineRuler(HWND nhWnd);
    void Draw(LPARAM lParam);
    bool Process(HWND parent, WPARAM wParam, LPARAM lParam);
    void SetOwner(TimeLine *nown);
};

class TimeLineScroll : public GadgetBase {
   private:
    TimeLine *owner;

   public:
    TimeLineScroll(HWND nhWnd);
    bool Process(HWND parent, WPARAM wParam, LPARAM lParam);
    void SetOwner(TimeLine *nown);
};