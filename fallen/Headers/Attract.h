// Attract.h
// Guy Simmons, 20th November 1997

#pragma once

//---------------------------------------------------------------

void	game_attract_mode();
void	level_won();
void	level_lost();


//
// The loading screen counter to completion.
//

void ATTRACT_loadscreen_init();
void ATTRACT_loadscreen_draw(std::int32_t completion);	// completion is in 8-bit fixed point from 0 to 256.



//---------------------------------------------------------------

