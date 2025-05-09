//
// Menus from within the game.
//

#ifndef _GAMEMENU_
#define _GAMEMENU_

//
// Initialises all the gamemenu stuff. Call at the start of the
// game loop.
//

void GAMEMENU_init();

//
// Processes the game menu keypresses.  Returns what action should
// be taken.
//

#define GAMEMENU_DO_NOTHING 0
#define GAMEMENU_DO_RESTART 1
#define GAMEMENU_DO_CHOOSE_NEW_MISSION 2
#define GAMEMENU_DO_NEXT_LEVEL 3

std::int32_t GAMEMENU_process();

//
// Returns true if the game is paused.
//

std::int32_t GAMEMENU_is_paused();

//
// Returns a multiplier to be used to slow down the game. It returns
// an 8-bit fixed point number. 0 means that the game should not be
// processed at all.
//

std::int32_t GAMEMENU_slowdown_mul();

//
// When you lose the level, this is the message that will be displayed.
//

void GAMEMENU_set_level_lost_reason(char* reason);

//
// Draws all the game menu stuff.
//

void GAMEMENU_draw();

#endif
