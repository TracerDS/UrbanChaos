//
// Matrix stuff...
//

#pragma once

//
// ALL ANGLES ARE IN RADIANS.
//

//
// Fills in the matrix appropriately given the three angles (yaw, pitch, roll)
// for the eye along the z-axis.
//

void MATRIX_calc(std::int32_t matrix[9], std::int32_t yaw, std::int32_t pitch, std::int32_t roll);

//
// Calculates a vector from yaw and pitch...
//

void MATRIX_vector(std::int32_t vector[3], std::int32_t yaw, std::int32_t pitch);

//
// Skews the view matrix so that it is correct for 3d clipping.
//
// The skew value should be one over the horizontal view-window resolution
// divided by the vertical view-window resolution.
//
// The x and y vectors are multiplied by the zoom.
//
// The whole matrix is scaled by scale.
//

void MATRIX_skew(std::int32_t matrix[9], std::int32_t skew, std::int32_t zoom, std::int32_t scale);

//
// Multiplies points x,y,z by matrix m.
//

#define MATRIX_MUL(m,x,y,z) 	   		\
{										\
	std::int32_t xnew, ynew, znew;	   			\
							   			\
	xnew  = (x) * (m)[0];		   		\
	ynew  = (x) * (m)[3];		   		\
	znew  = (x) * (m)[6];		   		\
								   		\
	xnew += (y) * (m)[1];		   		\
	ynew += (y) * (m)[4];		   		\
	znew += (y) * (m)[7];		   		\
								   		\
	xnew += (z) * (m)[2];		   		\
	ynew += (z) * (m)[5];		   		\
	znew += (z) * (m)[8];		   		\
							   			\
	(x) = xnew; (y) = ynew; (z) = znew;	\
}

#define MATRIX24_MUL(m,x,y,z) 	   		\
{										\
	std::int32_t xnew, ynew, znew;	   			\
							   			\
	xnew  = ((x) * (m)[0])>>8;	   		\
	ynew  = ((x) * (m)[3])>>8;	   		\
	znew  = ((x) * (m)[6])>>8;	   		\
								   		\
	xnew += ((y) * (m)[1])>>8;	   		\
	ynew += ((y) * (m)[4])>>8;	   		\
	znew += ((y) * (m)[7])>>8;	   		\
								   		\
	xnew += ((z) * (m)[2])>>8;	   		\
	ynew += ((z) * (m)[5])>>8;	   		\
	znew += ((z) * (m)[8])>>8;	   		\
							   			\
	(x) = xnew; (y) = ynew; (z) = znew;	\
}

#define MATRIX_MUL_INT(m,x,y,z)	   		\
{										\
	std::int32_t xnew, ynew, znew;	   			\
							   			\
	xnew  = (x) * (m)[0];		   		\
	ynew  = (x) * (m)[3];		   		\
	znew  = (x) * (m)[6];		   		\
								   		\
	xnew += (y) * (m)[1];		   		\
	ynew += (y) * (m)[4];		   		\
	znew += (y) * (m)[7];		   		\
								   		\
	xnew += (z) * (m)[2];		   		\
	ynew += (z) * (m)[5];		   		\
	znew += (z) * (m)[8];		   		\
							   			\
	(x) = xnew>>16; (y) = ynew>>16; (z) = znew>>16;	\
}

//
// Multiplies points x,y,z by the transpose of matrix m.
//

#define MATRIX_MUL_BY_TRANSPOSE(m,x,y,z) \
{										 \
	std::int32_t xnew, ynew, znew;	   			 \
							   			 \
	xnew  = (x) * (m)[0];		   		 \
	ynew  = (x) * (m)[1];		   		 \
	znew  = (x) * (m)[2];		   		 \
								   		 \
	xnew += (y) * (m)[3];		   		 \
	ynew += (y) * (m)[4];		   		 \
	znew += (y) * (m)[5];		   		 \
								   		 \
	xnew += (z) * (m)[6];		   		 \
	ynew += (z) * (m)[7];		   		 \
	znew += (z) * (m)[8];		   		 \
							   			 \
	(x) = xnew; (y) = ynew; (z) = znew;	 \
}

//
// Multiplies two 3x3 together.  A = MN.
//

void MATRIX_3x3mul(std::int32_t a[9], std::int32_t m[9], std::int32_t n[9]);

//
// Transposes the matrix m.
//

#define MATRIX_TRANSPOSE(m) {SWAP_FL(m[1], m[3]); SWAP_FL(m[2], m[6]); SWAP_FL(m[5], m[7]);}

//
// Rotates a matrix about its x,y or z vector.
//

void MATRIX_rotate_about_its_x(std::int32_t *matrix, std::int32_t angle);
void MATRIX_rotate_about_its_y(std::int32_t *matrix, std::int32_t angle);
void MATRIX_rotate_about_its_z(std::int32_t *matrix, std::int32_t angle);

//
// Convert a matrix into its equivalent yaw, pitch and roll.
//

typedef struct
{
	std::int32_t yaw;
	std::int32_t pitch;
	std::int32_t roll;

} Direction;

Direction MATRIX_find_angles(std::int32_t matrix[9]);



