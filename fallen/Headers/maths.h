//
// Mathy stuff...
//

#ifndef MATHS_H
#define MATHS_H



//
// Returns true if vector v intersects vector w
// If the two lines touch then that counts as an intersection.
//
// Make sure that the cross product of the two vectors doesn't overflow!
//

std::int32_t MATHS_seg_intersect(
			std::int32_t vx1, std::int32_t vz1, std::int32_t vx2, std::int32_t vz2,
			std::int32_t wx1, std::int32_t wz1, std::int32_t wx2, std::int32_t wz2);




#endif
