// Director.h
//
// Director object

// MenuItem
//
// a menu item

struct MenuItem {
    TCHAR Text[MAX_PATH]; // text for menu

    TCHAR Verb[MAX_PATH];      // verb for command - open, explore, menu, uninstall, back
    TCHAR Document[MAX_PATH];  // document/executable for command
    TCHAR Directory[MAX_PATH]; // directory for command

    std::uint32_t Spacing; // number of extra lines after menu item
    MenuItem* Next;        // next item in menu
    RECT Area;             // area of menu item in window
};

// Menu
//
// a menu

struct Menu {
    TCHAR Name[MAX_PATH];      // name of menu
    TCHAR Bitmap[MAX_PATH];    // name of bitmap for menu
    TCHAR FontName[MAX_PATH];  // font name for menu text
    std::uint32_t FontSize;    // font size (pixels)
    std::uint32_t FontWeight;  // font weight
    COLORREF ColourNormal;     // normal colour (0x00BBGGRR)
    COLORREF ColourSelected;   // selected colour (0x00BBGGRR)
    std::uint32_t LeftBorder;  // x of left border (pixels)
    std::uint32_t TopBorder;   // y of top border (pixels)
    std::uint32_t LineSpacing; // spacing between lines (pixels)
    MenuItem* Item;            // first item in menu
    Menu* Next;                // next menu in list

    Menu();         // set defaults
    Menu(FILE* fd); // load from file
    ~Menu();        // delete
};

// Director
//
// top-level control

struct Director {
    // data loaded from file
    TCHAR WindowTitle[MAX_PATH]; // title for main window
    TCHAR AppName[MAX_PATH];     // app name in registry
    TCHAR ExeName[MAX_PATH];     // executable name in registry

    TCHAR UninstallString[MAX_PATH]; // uninstall string
    TCHAR AppPath[MAX_PATH];         // application path

    Menu* MenuList;    // list of menus
    Menu* PreInstall;  // preinstall menu
    Menu* PostInstall; // postinstall menu
    Menu* Active;      // active menu

    Director();         // create default menus
    Director(FILE* fd); // load from file
    ~Director();        // delete

    void LoadRegistryStrings(); // load UninstallString[] and RunString[] from the registry

    Menu* FindMenu(TCHAR* name); // find a named menu
};
