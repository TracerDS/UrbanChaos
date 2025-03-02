//
// Quick load and save from within a level.
//

#ifndef _QLS_
#define _QLS_



//
// Clears all savegame info.
//

void QLS_init();


//
// Returns TRUE if a quick-savegame is available to load.
//

void QLS_available();


//
// Saves the current gamestate.
//

void QLS_save();


//
// Loads the last QLS_saved game. Reurns FALSE on failure. In which
// case the whole of gamestate will probably be screwed and you'll
// have to restart the level.
//

SLONG QLS_load();




#endif
