//
// Sofware renderer hacked in!
//

#ifndef _SW_
#define _SW_


//
// The maximum software screen dimensions.
//

#define SW_MAX_WIDTH  640
#define SW_MAX_HEIGHT 480

extern std::uint32_t *SW_buffer;

#ifndef TARGET_DC

//
// Reloads all textures. Looks at the D3D Texture system and
// loads a copy of each texture.
//

void SW_reload_textures();


//
// Initialises a new frame and clears the SW_buffer.
//

void SW_init(
		std::int32_t width,
		std::int32_t height);


//
// Sets the way each page is drawn.
//

#define SW_PAGE_IGNORE   0	// This page is not drawn.
#define SW_PAGE_NORMAL   1
#define SW_PAGE_MASKED   2
#define SW_PAGE_ALPHA    3
#define SW_PAGE_ADDITIVE 4

void SW_set_page(std::int32_t page, std::int32_t type);




//
// Adds a triangle. (x,y) are given in 8-bit fixed point and z
// is a value from 1 to 65535.
//

void SW_add_triangle(
		std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t r1, std::int32_t g1, std::int32_t b1, std::int32_t u1, std::int32_t v1,
		std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t r2, std::int32_t g2, std::int32_t b2, std::int32_t u2, std::int32_t v2,
		std::int32_t x3, std::int32_t y3, std::int32_t z3, std::int32_t r3, std::int32_t g3, std::int32_t b3, std::int32_t u3, std::int32_t v3,
		std::int32_t page,
		std::int32_t alpha = 255);	// Only for certain pages...

//
// Copies the SW_buffer onto the back buffer.
//

void SW_copy_to_bb();


#else //#ifndef TARGET_DC

// Not used - it just shuts the compiler up.
#define SW_PAGE_IGNORE   0	// This page is not drawn.
#define SW_PAGE_NORMAL   1
#define SW_PAGE_MASKED   2
#define SW_PAGE_ALPHA    3
#define SW_PAGE_ADDITIVE 4

#endif //#else //#ifndef TARGET_DC



#endif
