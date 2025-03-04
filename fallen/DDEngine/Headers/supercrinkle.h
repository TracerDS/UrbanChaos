//
// SUPERCRINKLES!
//

#ifndef _SUPERCRINKLE_
#define _SUPERCRINKLE_


// Set to 0 to remove all crinkle stuff.
#define SUPERCRINKLES_ENABLED 0




#if SUPERCRINKLES_ENABLED

//
// Loads all the crinkles for the current level and creates
// their lighting textures.
//

void SUPERCRINKLE_init();


//
// Does this page have a crinkle?
//

extern std::uint8_t SUPERCRINKLE_is_crinkled[512];

#define SUPERCRINKLE_IS_CRINKLED(num) SUPERCRINKLE_is_crinkled[num]


//
// Draws the given crinkle. It assumes the POLY_local_rotation
// has been setup properly already.  Returns true if it drew
// a SUPERCRINKLE.
//

std::int32_t SUPERCRINKLE_draw(std::int32_t page, std::uint32_t colour[4], std::uint32_t specular[4]);


#else //#if SUPERCRINKLES_ENABLED


// Dummy routs.
inline void SUPERCRINKLE_init() {}

#define SUPERCRINKLE_IS_CRINKLED(num) false

inline std::int32_t SUPERCRINKLE_draw(std::int32_t page, std::uint32_t colour[4], std::uint32_t specular[4]){ return false; }


#endif //#else //#if SUPERCRINKLES_ENABLED


#endif
