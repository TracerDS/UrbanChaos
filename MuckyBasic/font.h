//
// A font! That's all there is to it.
//

#ifndef _FONT_
#define _FONT_


//
// Loads in the font texture and calculates the uv's of the letters.
//

void FONT_init();



//
// Returns true if the FONT module can draw the ASCII character.
//

std::int32_t FONT_char_is_valid(char ch);


//
// Draws some text.  A scale of 1.0F is normal!  If cursor is >= 0, then a cursor
// draw after the 'cursor'th character.
//

#define FONT_FLAG_JUSTIFY_LEFT   (1 << 0)
#define FONT_FLAG_JUSTIFY_CENTRE (1 << 1)
#define FONT_FLAG_JUSTIFY_RIGHT  (1 << 2)
#define FONT_FLAG_DROP_SHADOW    (1 << 3)

void FONT_draw(float start_x, float start_y, std::uint32_t colour, std::uint32_t flag, float scale, std::int32_t cursor, char* fmt, ...);


#endif
