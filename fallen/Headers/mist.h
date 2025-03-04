//
// Mist.
//

#ifndef MIST_H
#define MIST_H


//
// Initialises all the mist.
//

void MIST_init();


//
// Creates a quad-patch of mist over the ground.
//

void MIST_create(
		std::int32_t detail,			// The number of quads-per-row on the quad patch.
		std::int32_t height,			// Above the ground.
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);

//
// A gust of wind that the fog reacts to.
//

void MIST_gust(
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);


//
// Processes all the layers of mist.
//

void MIST_process();


//
// How to get each point of each layer of mist
//

//
// Start getting each layer of mist.
// Returns the detail of the next layer of mist.
// Gives the position and texture coords of a point in the last
//		layer whose detail was returned by MIST_get_detail().
//

void  MIST_get_start ();
std::int32_t MIST_get_detail();	// nullptr => No more layers of mist.
void  MIST_get_point (std::int32_t px, std::int32_t pz,
		std::int32_t *x,
		std::int32_t *y,
		std::int32_t *z);
void  MIST_get_texture(std::int32_t px, std::int32_t pz,
		float *u,
		float *v);


#endif
