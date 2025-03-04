//
// Quick load and save from within a level.
//

#pragma once



//
// Clears all savegame info.
//

void QLS_init();


//
// Returns true if a quick-savegame is available to load.
//

void QLS_available();


//
// Saves the current gamestate.
//

void QLS_save();


//
// Loads the last QLS_saved game. Reurns false on failure. In which
// case the whole of gamestate will probably be screwed and you'll
// have to restart the level.
//

std::int32_t QLS_load();




