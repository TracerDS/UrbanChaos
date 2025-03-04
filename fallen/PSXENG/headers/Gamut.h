//
// Gamut calculations of polygons.
//

#pragma once


//
// The size of the gamut mapwho.
//

#define NGAMUT_SIZE 128
#define NGAMUT_SIZE_LO	32


//
// The gamut for squares.
//

typedef struct
{
	std::uint8_t xmin;
	std::uint8_t xmax;

} NGAMUT_Gamut;

typedef struct
{
	std::uint8_t zmin;
	std::uint8_t zmax;

} NGAMUT_Gamut2;

extern NGAMUT_Gamut2	NGAMUT_gamut2[NGAMUT_SIZE];
extern NGAMUT_Gamut NGAMUT_gamut[NGAMUT_SIZE];
extern std::int32_t        NGAMUT_zmin;
extern std::int32_t        NGAMUT_zmax;
extern std::int32_t        NGAMUT_Ymin;
extern std::int32_t        NGAMUT_Ymax;
extern std::int32_t			NGAMUT_xmin,NGAMUT_xmax;

//
// Function to work out a gamut.
//
// Initialises the gamut.
// Pushes out the gamut along the line. The line can go off the gamut mapwho, but
// the gamut always stays within the gamut mapwho.
//

void NGAMUT_init    ();
void NGAMUT_add_line(
		std::int32_t px1,
		std::int32_t pz1,
		std::int32_t px2,
		std::int32_t pz2);

//
// Works out a square gamut of the given radius centered at (x,z)
//

void NGAMUT_view_square(std::int32_t mid_x, std::int32_t mid_z, std::int32_t radius);


//
// Once you've worked out the gamut for squares, this function works
// out the gamut for points.
//

extern NGAMUT_Gamut NGAMUT_point_gamut[NGAMUT_SIZE];
extern std::int32_t        NGAMUT_point_zmin;
extern std::int32_t        NGAMUT_point_zmax;

void NGAMUT_calculate_point_gamut();

									 //
// This is a lower-res gamut. It is guaranteed to encompass all the
// squares in the point gamut.  It is calculated from the out gamut, so
// make sure you have calculated that one already.
//

extern NGAMUT_Gamut NGAMUT_lo_gamut[NGAMUT_SIZE_LO];
extern std::int32_t        NGAMUT_lo_zmin;
extern std::int32_t        NGAMUT_lo_zmax;

void NGAMUT_calculate_lo_gamut();


