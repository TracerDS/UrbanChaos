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
// Draws some text. When you shimmer the text, it gets drawn more transparent too...
//

#define FONT_FLAG_JUSTIFY_LEFT 0 // Default...
#define FONT_FLAG_JUSTIFY_CENTRE (1 << 0)
#define FONT_FLAG_JUSTIFY_RIGHT (1 << 1)
#define FONT_FLAG_ITALIC (1 << 2)

//
// After the function returns these values say where the next character
// would have been drawn.
//

extern float FONT_end_x;
extern float FONT_end_y;

void FONT_draw(
    std::int32_t flag,
    float start_x,
    float start_y,
    std::uint32_t colour,
    float scale,         // 1.0F => Normal scale
    std::int32_t cursor, // If cursor is >= 0, then a cursor is drawn after the 'cursor'th character
    float shimmer,       // How much to shimmer. 0.0F => no shimmering, 1.0F => maximum shimmering.
    char* fmt, ...);

#endif
