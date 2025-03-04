//
// Converts facets to draw indexed primitive calls...
//

#ifndef _SUPERFACET_
#define _SUPERFACET_



//
// Call at the start of the game_loop()- after everything has
// been loaded. Sets up memory.
//

void SUPERFACET_init();


//
// Sets up the frame for drawing SUPERFACET
//

void SUPERFACET_start_frame();



//
// Draws a super-fast facet if it can, otherwise returns false!
//

std::int32_t SUPERFACET_draw(std::int32_t facet);



//
// Call at the end of the game_loop()- frees up memory.
//

void SUPERFACET_fini();




#endif
