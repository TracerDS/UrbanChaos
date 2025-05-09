//
// Gamut calculations of polygons.
//

#ifndef _NGAMUT_
#define _NGAMUT_

//
// The size of the gamut mapwho.
//

#define NGAMUT_SIZE 128
#define NGAMUT_SIZE_LO 32

//
// The gamut for squares.
//

typedef struct
{
    std::int32_t xmin;
    std::int32_t xmax;

} NGAMUT_Gamut;

extern NGAMUT_Gamut NGAMUT_gamut[NGAMUT_SIZE];
extern std::int32_t NGAMUT_xmin;
extern std::int32_t NGAMUT_zmin;
extern std::int32_t NGAMUT_zmax;

//
// Function to work out a gamut.
//
// Initialises the gamut.
// Pushes out the gamut along the line. The line can go off the gamut mapwho, but
// the gamut always stays within the gamut mapwho.
//

void NGAMUT_init();
void NGAMUT_add_line(
    float px1,
    float pz1,
    float px2,
    float pz2);

//
// Works out a square gamut of the given radius centered at (x,z)
//

void NGAMUT_view_square(float mid_x, float mid_z, float radius);

//
// Once you've worked out the gamut for squares, this function works
// out the gamut for points.
//

extern NGAMUT_Gamut NGAMUT_point_gamut[NGAMUT_SIZE];
extern std::int32_t NGAMUT_point_zmin;
extern std::int32_t NGAMUT_point_zmax;

void NGAMUT_calculate_point_gamut();

//
// This is the square gamut expanded by one in each direction.
//

extern NGAMUT_Gamut NGAMUT_out_gamut[NGAMUT_SIZE];
extern std::int32_t NGAMUT_out_zmin;
extern std::int32_t NGAMUT_out_zmax;

void NGAMUT_calculate_out_gamut();

//
// This is a lower-res gamut. It is guaranteed to encompass all the
// squares in the point gamut.  It is calculated from the out gamut, so
// make sure you have calculated that one already.
//

extern NGAMUT_Gamut NGAMUT_lo_gamut[NGAMUT_SIZE_LO];
extern std::int32_t NGAMUT_lo_zmin;
extern std::int32_t NGAMUT_lo_zmax;

void NGAMUT_calculate_lo_gamut();

#endif
