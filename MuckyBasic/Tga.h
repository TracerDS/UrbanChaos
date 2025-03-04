//
// Loads in 32-bit RGBA uncompressed TGAs.
//

#pragma once


//
// The format of a TGA pixel.
//

typedef struct
{
	std::uint8_t blue;
	std::uint8_t green;
	std::uint8_t red;
	std::uint8_t alpha;

} TGA_Pixel;

//
// Info describing the tga.
//

#define TGA_FLAG_CONTAINS_ALPHA	(1 << 0)
#define TGA_FLAG_ONE_BIT_ALPHA  (1 << 1)		// Alpha is only 255 or 0
#define TGA_FLAG_GRAYSCALE		(1 << 2)		// For all pixels r == g == b.

typedef struct
{
	std::int32_t valid;
	std::int32_t width;
	std::int32_t height;
	std::uint32_t flag;
	
} TGA_Info;

//
// If the width and height of the tga exceed the maximums, then the tga is not loaded.
//

TGA_Info TGA_load(
			const char* file,
			std::int32_t        max_width,
			std::int32_t        max_height,
			TGA_Pixel   *data);


//
// Saves out a tga.
//

void TGA_save(
		const char* file,
		std::int32_t        width,
		std::int32_t        height,
		TGA_Pixel   *data,
		std::int32_t        contains_alpha);	// false => Save without the alpha data.



//
// Returns the colour of the given pixel.
//

#define TGA_COLOUR_DONTKNOW 0
#define TGA_COLOUR_BLACK	1
#define TGA_COLOUR_GREEN	2
#define TGA_COLOUR_BROWN	3
#define TGA_COLOUR_WHITE	4
#define TGA_COLOUR_RED		5
#define TGA_COLOUR_GREY		6

std::int32_t TGA_colour(TGA_Pixel tp);




