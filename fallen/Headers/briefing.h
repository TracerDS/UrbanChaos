//
// briefing.h
// mission selection, briefings
// 14 dec 98
//

#include "MFStdLib.h"

extern char BRIEFING_mission_filename[_MAX_PATH];

std::int8_t BRIEFING_select();
std::int8_t BRIEFING_next_mission(); // 0 = run out of missions

// #define OBEY_SCRIPT