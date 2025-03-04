//
// Cached lighting.
//

#pragma once


#include "pap.h"

#define NIGHT_MAX_SLIGHTS 256


#ifdef TARGET_DC

// My DC monitor is seriously dark, so to be able to see anything (i.e. does the texturing work?)
// I turn the brightness of the game up. But on a sensible monitor/TV it looks awful.
#ifdef DEBUG
#define NIGHT_LIGHT_MULTIPLIER 2.0f
#else
#define NIGHT_LIGHT_MULTIPLIER 1.0f
#endif

#else //#ifdef TARGET_DC

#define NIGHT_LIGHT_MULTIPLIER 1.0f

#endif //#else //#ifdef TARGET_DC


//
// The static lights.
// 

typedef struct
{
	std::int16_t y;
	std::uint8_t x; // I'm grabbing myself the top bit
	std::uint8_t z; //  of these for flags of some sort (inside or something)
	std::int8_t red;
	std::int8_t green;
	std::int8_t blue;
	std::uint8_t radius;

} NIGHT_Slight;

extern NIGHT_Slight *NIGHT_slight;//[NIGHT_MAX_SLIGHTS];
extern std::int32_t        NIGHT_slight_upto;

typedef struct
{
	std::uint8_t index;
	std::uint8_t number;

} NIGHT_Smap;

typedef NIGHT_Smap	NIGHT_Smap_2d[PAP_SIZE_LO];

extern NIGHT_Smap_2d *NIGHT_smap; //[PAP_SIZE_LO][PAP_SIZE_LO];

//
// The dynamic lights.
//

#define NIGHT_DLIGHT_FLAG_USED   (1 << 0)
#define NIGHT_DLIGHT_FLAG_REMOVE (1 << 1)	// Will be removed next gameturn.

typedef struct
{
	std::uint16_t x;
	std::int16_t y;
	std::uint16_t z;
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
	std::uint8_t radius;
	std::uint8_t next;
	std::uint8_t flag;

} NIGHT_Dlight;

#define NIGHT_MAX_DLIGHTS 64

extern NIGHT_Dlight *NIGHT_dlight; //[NIGHT_MAX_DLIGHTS];



//
// Coloured lighting.
//

typedef struct
{
#ifdef	PSX_COMPRESS_LIGHT
	std::uint16_t	Col;
#else
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
#endif

} NIGHT_Colour;



#define	get_red(col)	(((col)>>10)&0x3f)
#define	get_green(col)	(((col)>>5)&0x1f)
#define	get_blue(col)	(((col))&0x1f)

#define	set_red(col,red)	col=(red)<<10
#define	set_green(col,red)	col=(red)<<5
#define	set_blue(col,red)	col=(red)

#define	or_red(col,red)		col|=(red)<<10
#define	or_green(col,red)	col|=(red)<<5
#define	or_blue(col,red)	col|=(red)

#define	nor_red(col,red)	col=(col&(0x3f<<10))|((red)<<10)
#define	nor_green(col,red)	col=(col&(0x1f<<5))|((red)<<5)
#define	nor_blue(col,red)	col=(col&(0x1f<<0))|((red)<<0)

//
// The cached lighting lights all the points and all the prims
// in each lo-res mapsquare.  The first 16 colour entries are
// the colours of the 4x4 points in this mapsquare.  After this
// there are the colours of all the points of the prims as returned
// by the OB module.
//

#define NIGHT_SQUARE_FLAG_USED		(1 << 0)
#define NIGHT_SQUARE_FLAG_WARE		(1 << 1)
#define NIGHT_SQUARE_FLAG_DELETEME	(1 << 2)

typedef struct
{
	std::uint8_t         next;
	std::uint8_t         flag;
	std::uint8_t         lo_map_x;
	std::uint8_t         lo_map_z;
	NIGHT_Colour *colour;
	std::uint32_t		  sizeof_colour;	// In bytes.

} NIGHT_Square;

#define NIGHT_MAX_SQUARES 256

extern NIGHT_Square NIGHT_square[NIGHT_MAX_SQUARES];
extern std::uint8_t        NIGHT_square_free;

//
// The cached lighting mapwho. Contains indices into the
// NIGHT_square array, or nullptr if there is no cached lighting
// for that square.
//

extern std::uint8_t NIGHT_cache[PAP_SIZE_LO][PAP_SIZE_LO];

#ifdef	PSX
extern std::uint16_t	floor_psx_col[PAP_SIZE_HI][PAP_SIZE_HI];
#endif

//
// The cached lighting for dfacets.
//

typedef struct
{
	std::uint8_t         next;
	std::uint8_t         counter;
	std::uint16_t         dfacet;
	NIGHT_Colour *colour;
	std::uint16_t         sizeof_colour;

} NIGHT_Dfcache;

#define NIGHT_MAX_DFCACHES 256

extern NIGHT_Dfcache NIGHT_dfcache[NIGHT_MAX_DFCACHES];
extern std::uint8_t         NIGHT_dfcache_free;
extern std::uint8_t		 NIGHT_dfcache_used;

// ========================================================
//
// NEW LIGHTING FUNCTIONS
//
// ========================================================

//
// Initialises all the lighting: Removes all static and dynamic lights,
// and clears all cached info.
//

void NIGHT_init();

//
// Converts a colour to its D3D equivalents.
//

#define NIGHT_MAX_BRIGHT 64

//
// Make bright light create specular.
//

#ifdef TARGET_DC
static const std::int32_t NIGHT_specular_enable = 0;
#else
extern std::int32_t NIGHT_specular_enable;
#endif

inline void NIGHT_get_d3d_colour(NIGHT_Colour col, D3DCOLOR* colour, D3DCOLOR* specular)
{
	std::int32_t red   = col.red;
	std::int32_t green = col.green;
	std::int32_t blue  = col.blue;

	red   *= (256 / NIGHT_MAX_BRIGHT);
	green *= (256 / NIGHT_MAX_BRIGHT);
	blue  *= (256 / NIGHT_MAX_BRIGHT);

	if (NIGHT_specular_enable)
	{
		std::int32_t wred   = 0;
		std::int32_t wgreen = 0;
		std::int32_t wblue  = 0;

		if (red   > 255) {wred   = (red   - 255) >> 1; red   = 255; if (wred   > 255) {wred   = 255;}}
		if (green > 255) {wgreen = (green - 255) >> 1; green = 255; if (wgreen > 255) {wgreen = 255;}}
		if (blue  > 255) {wblue  = (blue  - 255) >> 1; blue  = 255; if (wblue  > 255) {wblue  = 255;}}

	   *colour    = (red  << 16) | (green  << 8) | (blue  << 0);
	   *specular  = (wred << 16) | (wgreen << 8) | (wblue << 0);
	   *specular |= 0xff000000;		// No fog by default...
	}
	else
	{
		if (red   > 255) {red   = 255;}
		if (green > 255) {green = 255;}
		if (blue  > 255) {blue  = 255;}

	   *colour   = (red << 16) | (green << 8) | (blue << 0);
	   *specular = 0xff000000;		// No fog by default...
	}
	*colour |= 0xff000000;		// No fog by default...
}

//
// z in range 0->1.0
//

#ifndef	POLY_FADEOUT_START
//
// sneak it in here
//
#define POLY_FADEOUT_START	(0.60F)
#define POLY_FADEOUT_END	(0.95F)
#endif

inline void NIGHT_get_d3d_colour_and_fade(NIGHT_Colour col, std::uint32_t *colour, std::uint32_t *specular,float z)
{
	std::int32_t red   = col.red;
	std::int32_t green = col.green;
	std::int32_t blue  = col.blue;
	std::int32_t multi = 255 - (std::int32_t)((z - POLY_FADEOUT_START) * (256.0F / (POLY_FADEOUT_END - POLY_FADEOUT_START)));

//red   *= (256 / NIGHT_MAX_BRIGHT);
//green *= (256 / NIGHT_MAX_BRIGHT);
//blue  *= (256 / NIGHT_MAX_BRIGHT);

	if(multi>255)
	{
		multi=255;
	}

	if (multi < 0)
	{
		multi = 0;
	}
	multi*=4; //replaces above

	red		= red	*multi>>8;  // fade out on distance
	green	= green	*multi>>8; 
	blue	= blue	*multi>>8;


/* //dead I believe
	if (NIGHT_specular_enable)
	{
		std::int32_t wred   = 0;
		std::int32_t wgreen = 0;
		std::int32_t wblue  = 0;

		if (red   > 255) {wred   = (red   - 255) >> 1; red   = 255; if (wred   > 255) {wred   = 255;}}
		if (green > 255) {wgreen = (green - 255) >> 1; green = 255; if (wgreen > 255) {wgreen = 255;}}
		if (blue  > 255) {wblue  = (blue  - 255) >> 1; blue  = 255; if (wblue  > 255) {wblue  = 255;}}

	   *colour    = (red  << 16) | (green  << 8) | (blue  << 0);
	   *specular  = (wred << 16) | (wgreen << 8) | (wblue << 0);
	   *specular |= 0xff000000;		// No fog by default...
	}
	else
*/
	{
		if (red   > 255) {red   = 255;}
		if (green > 255) {green = 255;}
		if (blue  > 255) {blue  = 255;}

	   *colour   = (red << 16) | (green << 8) | (blue << 0);
	   *specular = 0xff000000;		// No fog by default...
	}
	*colour |= 0xff000000;		// No fog by default...
}

inline void NIGHT_get_d3d_colour_dim(NIGHT_Colour col, D3DCOLOR* colour, D3DCOLOR* specular)
{
	std::int32_t red   = col.red;
	std::int32_t green = col.green;
	std::int32_t blue  = col.blue;

	red   *= (256 / NIGHT_MAX_BRIGHT);
	green *= (256 / NIGHT_MAX_BRIGHT);
	blue  *= (256 / NIGHT_MAX_BRIGHT);

	red>>=1;
	green>>=1;
	blue>>=1;

	if (NIGHT_specular_enable)
	{
		std::int32_t wred   = 0;
		std::int32_t wgreen = 0;
		std::int32_t wblue  = 0;

		if (red   > 255) {wred   = (red   - 255) >> 1; red   = 255; if (wred   > 255) {wred   = 255;}}
		if (green > 255) {wgreen = (green - 255) >> 1; green = 255; if (wgreen > 255) {wgreen = 255;}}
		if (blue  > 255) {wblue  = (blue  - 255) >> 1; blue  = 255; if (wblue  > 255) {wblue  = 255;}}

	   *colour    = (red  << 16) | (green  << 8) | (blue  << 0);
	   *specular  = (wred << 16) | (wgreen << 8) | (wblue << 0);
	   *specular |= 0xff000000;		// No fog by default...
	}
	else
	{
		if (red   > 255) {red   = 255;}
		if (green > 255) {green = 255;}
		if (blue  > 255) {blue  = 255;}

	   *colour   = (red << 16) | (green << 8) | (blue << 0);
	   *specular = 0xff000000;		// No fog by default...
	}
	*colour |= 0xff000000;		// No fog by default...
}

//
// Returns the amount of light at the given spot.
//

NIGHT_Colour NIGHT_get_light_at(
				std::int32_t x,
				std::int32_t y,
				std::int32_t z);


#ifndef PSX

//
// Fills the array with all the lights that effect the given point.
// (not including the ambient light)
//

typedef struct
{
	std::int32_t r;	// (r,g,b) falling on the point.
	std::int32_t g;
	std::int32_t b;
	std::int32_t dx;	// Normalised vector from the point to the light (256 long)
	std::int32_t dy;
	std::int32_t dz;

} NIGHT_Found;

#define NIGHT_MAX_FOUND 4

extern NIGHT_Found NIGHT_found[NIGHT_MAX_FOUND];
extern std::int32_t       NIGHT_found_upto;

void NIGHT_find(std::int32_t x, std::int32_t y, std::int32_t z);

#endif


//
// Initialises the heap and gets rid of all cached lighting.
//

void NIGHT_destroy_all_cached_info();

//
// Lighting flags and variables.
//

#define NIGHT_FLAG_LIGHTS_UNDER_LAMPOSTS	(1 << 0)
#define NIGHT_FLAG_DARKEN_BUILDING_POINTS	(1 << 1)
#define NIGHT_FLAG_DAYTIME					(1 << 2)

extern std::uint32_t        NIGHT_flag;
extern NIGHT_Colour NIGHT_sky_colour;
extern std::uint8_t        NIGHT_lampost_radius;
extern std::int8_t        NIGHT_lampost_red;
extern std::int8_t        NIGHT_lampost_green;
extern std::int8_t        NIGHT_lampost_blue;

//
// Loads a lighting file saved from the ED module.  Return true
// on success.
//

extern std::int32_t NIGHT_load_ed_file(char* name);


// ========================================================
//
// AMBIENT LIGHT
//
// ========================================================

extern D3DCOLOR NIGHT_amb_d3d_colour;
extern D3DCOLOR NIGHT_amb_d3d_specular;
extern std::int32_t NIGHT_amb_red;
extern std::int32_t NIGHT_amb_green;
extern std::int32_t NIGHT_amb_blue;
extern std::int32_t NIGHT_amb_norm_x;
extern std::int32_t NIGHT_amb_norm_y;
extern std::int32_t NIGHT_amb_norm_z;

//
// The normal should be normalised to 256.
//

void NIGHT_ambient(
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue,
		std::int32_t norm_x,
		std::int32_t norm_y,
		std::int32_t norm_z);

//
// Returns the amount of light captured by a point with the
// given normal. (256 long)
// 

NIGHT_Colour NIGHT_ambient_at_point(
				std::int32_t norm_x,
				std::int32_t norm_y,
				std::int32_t norm_z);

// ========================================================
//
// STATIC LIGHTS
//
// ========================================================

std::int32_t NIGHT_slight_create(		// Returns false on failure.
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::uint8_t radius,
		std::int8_t red,
		std::int8_t green,
		std::int8_t blue);

void NIGHT_slight_delete(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::uint8_t radius,
		std::int8_t red,
		std::int8_t green,
		std::int8_t blue);

void NIGHT_slight_delete_all();


// ========================================================
// 
// DYNAMIC LIGHTS
//
// ========================================================

std::uint8_t NIGHT_dlight_create(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::uint8_t radius,
		std::uint8_t red,
		std::uint8_t green,
		std::uint8_t blue);

void NIGHT_dlight_destroy(std::uint8_t dlight_index);
void NIGHT_dlight_move   (std::uint8_t dlight_index, std::int32_t x, std::int32_t y, std::int32_t z);
void NIGHT_dlight_colour (std::uint8_t dlight_index, std::uint8_t red, std::uint8_t green, std::uint8_t blue);

//
// Lights all the used cached lighting info from the dynamic lights.
// Undoes all the dynamic lighting.
//

void NIGHT_dlight_squares_up  ();
void NIGHT_dlight_squares_down();


// ========================================================
//
// CACHED LIGHTING OF THE MAP AND PRIMS
//
// ========================================================

//
// Recalculates all the cache entries.
// Creates a mapwho entry in the given lo-res mapwho square.
// Frees up all the memory.
//

void NIGHT_cache_recalc ();
void NIGHT_cache_create (std::uint8_t lo_map_x, std::uint8_t lo_map_z, std::uint8_t inside_warehouse = false);
void NIGHT_cache_create_inside(std::uint8_t lo_map_x, std::uint8_t lo_map_z,std::int32_t floor_y);
void NIGHT_cache_destroy(std::uint8_t square_index);


// ========================================================
//
// CACHED LIGHTING OF DFACETS ON THE SUPERMAP
//
// ========================================================

void NIGHT_dfcache_recalc ();
std::uint8_t NIGHT_dfcache_create (std::uint16_t dfacet_index);
void NIGHT_dfcache_destroy(std::uint8_t dfcache_index);


// ========================================================
//
// STATIC LIGHTING OF THE WALKABLE FACES' POINTS
//
// ========================================================

//
// The first few thousand prim points dont belong to the walkable faces.
// This is the index of the first walkable prim point.  
// Walkable primpoint x is index as NIGHT_walkable[x - NIGHT_first_walkable_point]...
//

#define NIGHT_MAX_WALKABLE 15000

extern std::int32_t NIGHT_first_walkable_prim_point;
extern NIGHT_Colour NIGHT_walkable[NIGHT_MAX_WALKABLE];
extern NIGHT_Colour NIGHT_roof_walkable[];

#define NIGHT_ROOF_WALKABLE_POINT(f,p) (NIGHT_roof_walkable[f*4+p])

#ifndef NDEBUG
std::int32_t NIGHT_check_index(std::int32_t walkable_prim_point_index);
#define NIGHT_WALKABLE_POINT(p) (NIGHT_check_index(p), NIGHT_walkable[p - NIGHT_first_walkable_prim_point])
#else

//
// Returns the colour of the given walkable prim_point.
//

#define NIGHT_WALKABLE_POINT(p) (NIGHT_walkable[(p) - NIGHT_first_walkable_prim_point])

#endif

//
// Generates the walkable info for the current map.
//

void NIGHT_generate_walkable_lighting();









