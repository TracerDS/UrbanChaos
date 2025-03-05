#ifndef PRIM_DRAW_H
#define PRIM_DRAW_H 1

//**********
//*  DATA  *
//**********

//***************
//*  FUNCTIONS  *
//***************

extern void draw_a_prim_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, std::uint8_t shade);
extern void draw_a_multi_prim_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);
extern void draw_a_prim_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, std::uint8_t shade);
extern void draw_a_rot_prim_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t tween, struct PrimMultiAnim *anim_info, struct Matrix33 *rot_mat);

extern void init_matrix33(struct Matrix33 *mat);
extern void matrix_mult33(struct Matrix33 *result, struct Matrix33 *mat1, struct Matrix33 *mat2);
extern void rotate_obj(std::int16_t xangle, std::int16_t yangle, std::int16_t zangle, Matrix33 *r3);

#endif
