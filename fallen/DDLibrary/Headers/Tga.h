//
// Loads in 32-bit RGBA uncompressed TGAs.
//

#ifndef _TGA_
#define _TGA_

#include "FileClump.h"

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

typedef struct
{
    std::int32_t valid;
    std::int32_t width;
    std::int32_t height;
    std::int32_t contains_alpha;

} TGA_Info;

//
// If the width and height of the tga exceed the maximums, then the tga is not loaded.
//

TGA_Info TGA_load(
    const char* file,
    std::int32_t max_width,
    std::int32_t max_height,
    TGA_Pixel* data,
    std::uint32_t id,
    bool bCanShrink = true);

//
// Saves out a tga.
//

void TGA_save(
    const char* file,
    std::int32_t width,
    std::int32_t height,
    TGA_Pixel* data,
    std::int32_t contains_alpha); // false => Save without the alpha data.

// Clump management

void OpenTGAClump(const char* clumpfn, size_t maxid, bool readonly);
bool DoesTGAExist(const char* filename, std::uint32_t id);
void CloseTGAClump();
FileClump* GetTGAClump();

#endif
