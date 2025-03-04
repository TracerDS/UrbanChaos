//
// For establishing a common time with the server.
//

#pragma once



//
// Sends pings to the server for a while until a common time has
// been established with the server.  
//
// After calling this function GAME_turn, GAME_process and GAME_tick are
// all set to the same values are the server (theoretically).
//
// Returns false on failure.
//

std::int32_t PING_do();




