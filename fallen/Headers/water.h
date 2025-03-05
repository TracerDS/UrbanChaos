//
// Water.
//

#ifndef WATER_H
#define WATER_H

//
// Gets rid of all water.
//

void WATER_init();

//
// Creates a square of water at the given height.
//

void WATER_add(std::int32_t map_x, std::int32_t map_z, std::int32_t height);

//
// Effects the water as if something was moving through it.
//

void WATER_gush(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2);

//
// Processes the water.
//

void WATER_process();

//
// Drawing the water...
//

//
// For storing the 'transformed point index' with each
// of the water points.
//

void WATER_point_index_clear_all();
std::uint16_t WATER_point_index_get(std::uint16_t p_index);
void WATER_point_index_set(std::uint16_t p_index, std::uint16_t index);

//
// Returns the first water face in the linked list above a square.
// Returns the next face... nullptr => the end of the list.
//

std::uint16_t WATER_get_first_face(std::int32_t x, std::int32_t z);
std::uint16_t WATER_get_next_face(std::uint16_t f_index);

//
// Returns the points of the given face by filling in the array.
//

void WATER_get_face_points(std::uint16_t f_index, std::uint16_t p_index[4]);

//
// Returns the given point.
//

void WATER_get_point_pos(std::uint16_t p_index, float *x, float *y, float *z);
void WATER_get_point_uvs(std::uint16_t p_index, float *u, float *v, std::uint32_t *colour);

#endif
