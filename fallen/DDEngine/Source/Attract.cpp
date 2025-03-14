// Attract.cpp
// Guy Simmons, 20th November 1997.

#include "DDlib.h"
#include "Engine.h"
#include "GDisplay.h"
#include "font.h"

extern Display the_display;
//---------------------------------------------------------------

void engine_attract() {
    static int count = 0;
    count++;
    if (the_display.screen_lock()) {
        char str[100];
        sprintf(str, "%d", count);
        FONT_draw_coloured_text(220, 5, 128, 128, 128, str);
        the_display.screen_unlock();
    }

    FLIP(nullptr, DDFLIP_WAIT);
    return;
}

//---------------------------------------------------------------

extern Camera test_view;
void game_engine(Camera *the_view);

void engine_win_level() {
}

//---------------------------------------------------------------

void engine_lose_level() {
}

//---------------------------------------------------------------
