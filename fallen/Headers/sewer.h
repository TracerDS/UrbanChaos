//
// Sewers.. yuck!
//

#ifndef _SEWER_
#define _SEWER_


//
// The size of the sewage system.
// How deep the sewers are.
//

#define SEWER_SIZE	128
#define SEWER_DEPTH 0x100


//
// How to create the sewage system.
//

#define SEWER_EDGE_XS	0
#define SEWER_EDGE_XL	1
#define SEWER_EDGE_ZS	2
#define SEWER_EDGE_ZL	3

void SEWER_init     ();
void SEWER_square_on(std::int32_t x, std::int32_t z);
void SEWER_ladder_on(std::int32_t x, std::int32_t z, std::int32_t edge);
void SEWER_pillar_on(std::int32_t x, std::int32_t z);
void SEWER_precalc  ();
void SEWER_save     (char* filename);
void SEWER_load     (char* filename);


//
// Returns true if the given square is an entrance into the
// sewage system.
//

std::int32_t SEWER_can_i_enter(std::uint8_t x, std::uint8_t z);


//
// Inserts colvects aroung the sewer system.
// Removes sewer colvects from the map.
//

void SEWER_colvects_insert();
void SEWER_colvects_remove();


//
// Returns the height of the sewers at (x,z).
//

std::int32_t SEWER_calc_height_at(std::int32_t x, std::int32_t z);



//
// How to draw a square of the sewage system.
//

#define SEWER_PAGE_FLOOR	0
#define SEWER_PAGE_WALL		1
#define SEWER_PAGE_PILLAR	2
#define SEWER_PAGE_WATER	3
#define SEWER_PAGE_NUMBER	4

typedef struct
{
	std::int32_t x[4];
	std::int32_t y[4];
	std::int32_t z[4];
	std::int32_t u[4];	// 16-bit fixed point from 0.0 to 256.0
	std::int32_t v[4];	// 16-bit fixed point from 0.0 to 256.0
	std::uint32_t c[4];

	std::uint8_t page;	// The sewer page... NOT the texturepage!
	
} SEWER_Face;

void SEWER_get_start(std::int32_t x, std::int32_t z);
SEWER_Face *SEWER_get_next ();	// nullptr => there are no more faces.
SEWER_Face *SEWER_get_water();	// Returns the water faces...


#endif
