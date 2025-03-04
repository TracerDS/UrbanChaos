//
// Hypermatter!
//

#ifndef HM_H
#define HM_H



//
// Initialises all the hypermatter objects.
//

void HM_init();



//
// Loads in the hm prim data from the editor. Expects a file
// that start with a header followed by an array
//

typedef struct
{
	std::int32_t version;			// Must be set to 1
	std::int32_t num_primgrids;	// The number of prim grids.
 
} HM_Header;

#define HM_MAX_RES 8

typedef struct
{
	std::uint8_t prim;
	std::uint8_t x_res;
	std::uint8_t y_res;
	std::uint8_t z_res;

	std::int32_t x_point[HM_MAX_RES];	// No point of the prim should lie outside the hypermatter
	std::int32_t y_point[HM_MAX_RES];
	std::int32_t z_point[HM_MAX_RES];

	float x_dgrav;
	float y_dgrav;
	float z_dgrav;

} HM_Primgrid;

void HM_load(char* fname);

//
// Returns an HM_Primgrid for the given prim. If one has not
// been defined, then it returns a 1x1x1 box around the
// entire prim.  Ignore the 'prim' field of what you get back.
//

HM_Primgrid *HM_get_primgrid(std::int32_t prim);


//
// Creates a new hypermatter object around the given mesh or returns
// HM_NO_MORE_OBJECTS
//

#define HM_NO_MORE_OBJECTS 255

std::uint8_t HM_create(
		std::int32_t prim,
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t yaw,
		std::int32_t pitch,
		std::int32_t roll,

		std::int32_t dx,
		std::int32_t dy,
		std::int32_t dz,

		// Between 2 and HM_MAX_RES

		std::int32_t x_res,		// The number of points along the x-axis
		std::int32_t y_res,		// The number of points along the y-axis
		std::int32_t z_res,		// The number of points along the z-axis

		std::int32_t x_point[],	// The position of each point along the x-axis, 0 => The bounding box min, 0x10000 => the bb max.
		std::int32_t y_point[],
		std::int32_t z_point[],

		//
		// How the mass varies across the HM_object. If either of them
		// has an absolute value of > 1.0F then part of the object will
		// have negative weight!
		//

		float x_dgrav,
		float y_dgrav,
		float z_dgrav,

		//
		// These go from 0 to 1.
		// 

		float elasticity,
		float bounciness,
		float friction,
		float damping);

//
// Destroys the given HM_object.
//

void HM_destroy(std::uint8_t hm_index);


//
// Sets off a shockwave that effect the given hm object.
//

void HM_shockwave(
		std::uint8_t hm_index,
		float x,
		float y,
		float z,
		float range,
		float force);

//
// Returns where the hm_object's centre of gravity is.
//

void HM_find_cog(
		std::uint8_t  hm_index,
		float *x,
		float *y,
		float *z);

//
// Collides hypermatter object1 against hypermatter object2
//

void HM_collide_all();
void HM_collide(std::uint8_t hm_index1, std::uint8_t hm_index2);

//
// Collision of hypermatter objects with the colvects. Colvects
// are assumed to be infinitely tall.
//

void HM_colvect_clear(std::uint8_t hm_index);
void HM_colvect_add(
		std::uint8_t hm_index,
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2);

//
// Processes all the hypermatter objects.
//

void HM_process();

//
// Returns the given point of the mesh the hypermatter object
// was derived from.
// 

void HM_find_mesh_point(
		std::uint8_t  hm_index,
		std::int32_t  point,
		float *x,
		float *y,
		float *z);

//
// Returns the position and angle of the given hypermatter object.
//

void HM_find_mesh_pos(
		std::uint8_t  hm_index,
		std::int32_t *x,
		std::int32_t *y,
		std::int32_t *z,
		std::int32_t *yaw,
		std::int32_t *pitch,
		std::int32_t *roll);

//
// Returns true if the given hypermatter object is stationary.
//

std::int32_t HM_stationary(std::uint8_t hm_index);

//
// Draws the hypermatter objects.
//

void HM_draw();







#endif
