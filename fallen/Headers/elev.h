//
// The new ECTS level stuff!
//

#ifndef _ELEV_
#define _ELEV_

#ifndef PSX

//
// This are the last ones to be loaded...
//

extern char ELEV_last_map_loaded[MAX_PATH];

extern char ELEV_fname_map[_MAX_PATH];
extern char ELEV_fname_lighting[_MAX_PATH];
extern char ELEV_fname_citsez[_MAX_PATH];
extern char ELEV_fname_level[_MAX_PATH];

#endif

//
// Prompts for a level and loads it. Returns false on failure.
// Loads the given level in. Returns false on failure.
//

std::int32_t ELEV_load_user(std::int32_t game);
std::int32_t ELEV_load_name(char* fname);
std::int32_t ELEV_game_init(
    char* fname_map,
    char* fname_lighting,
    char* fname_sewers,
    char* fname_level);

#endif
