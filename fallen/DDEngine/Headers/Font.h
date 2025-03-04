//
// Cheapo font stuff...
//

#ifndef _FONT_
#define _FONT_


//
// The maximum dimension of a character.
// Normally the width will be smaller.
//

#define FONT_HEIGHT 9
#define FONT_WIDTH  8

//
// The maximum number of characters allowed in each message.
//

#define FONT_MAX_LENGTH 512


//
// Adds a buffered message.
//

void FONT_buffer_add(
		std::int32_t  x,
		std::int32_t  y,
		std::uint8_t  red,
		std::uint8_t  green,
		std::uint8_t  blue,
		std::uint8_t  shadowed_or_not,
		char* fmt, ...);

//
// Draws all the buffered up sentences and clears the buffered list.
//
// THIS FUNCTION LOCKS AND UNLOCKS THE SCREEN FOR YOU!
// 

void FONT_buffer_draw();



//
// THE SCREEN MUST BE LOCKED WHEN YOU CALL THESE FUNCTIONS!
//

//
// Draws the given message at (x, y) on the screen.
// The text is draw in yellow with a shadow of red.
// Returns the length of the string in pixels.
//

std::int32_t FONT_draw(std::int32_t x, std::int32_t y, char* fmt, ...);


//
// Draws the texture in the given colour.
// Returns the length of the string in pixels.
//

std::int32_t FONT_draw_coloured_text(
		std::int32_t x,
		std::int32_t y,
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue,
		char* fmt, ...);

//
// Draws the char in the given colour- or does nothing if that character is not
// in the font. Returns the width of the char in pixels.
//

std::int32_t FONT_draw_coloured_char(
		std::int32_t x,
		std::int32_t y,
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue,
		char ch);

//
// Draws the message so that it fits in a reasonably rectangular box shape.
// The bottom-left of the box is the given coordinate.
//

void FONT_draw_speech_bubble_text(
		std::int32_t x,
		std::int32_t y,
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue,
		char* fmt, ...);



#endif
