//
// Game-wide stuff
//

#pragma once


//
// Game related variables.
//

#define GAME_TICKS_PER_SECOND	200
#define GAME_TICKS_PER_PROCESS	(1000 / GAME_TICKS_PER_SECOND)

extern std::int32_t GAME_turn;		// There is one gameturn every 16th processs.
extern std::int32_t GAME_process;	// These happen every 5 milliseconds.
extern std::int32_t GAME_tick;		// The millisecond counter for processing that tries to keep up with
							// OS_ticks().






//
// The whole game is inside here!
//

void GAME_do();





