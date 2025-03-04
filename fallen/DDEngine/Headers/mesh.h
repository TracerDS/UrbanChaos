//
// Drawing rotating prims.
//

#ifndef MESH_H
#define MESH_H


#include "night.h"


//
// If a prim face has its FACE_FLAG_TINTED flag set, then
// the colours the face is drawn with is ANDED with this
// value
// 

extern std::uint32_t MESH_colour_and;

//
// Call once at the start of the whole game. Calculates the
// crumple look-up tables.
//

void MESH_init();


//
// Draws a mesh using the POLY module.  If 'lpc' is nullptr then, the ambient
// light colour is used.  Returns the address of after the end of the lpc array.
//

NIGHT_Colour *MESH_draw_poly(
				std::int32_t         prim,
				std::int32_t	      at_x,
				std::int32_t         at_y,
				std::int32_t	      at_z,
				std::int32_t         i_yaw,
				std::int32_t         i_pitch,
				std::int32_t         i_roll,
				NIGHT_Colour *lpc,
				std::uint8_t         fade,
				std::int32_t         crumple = 0);

//
// Sets car crumple parameters before a call to MESH_draw_poly(..., -1)
//

void MESH_set_crumple(std::uint8_t* assignments, std::uint8_t* crumples);

//
// Draws an environment map over this given prim.
//

void MESH_draw_envmap(
		std::int32_t prim,
		std::int32_t at_x,
		std::int32_t at_y,
		std::int32_t at_z,
		std::int32_t i_yaw,
		std::int32_t i_pitch,
		std::int32_t i_roll);


//
// This version uses a flipped matrix compares to MESH_draw_poly
// It's useful for helicopters. Trust me. I'm a programmer.
//

NIGHT_Colour *MESH_draw_poly_inv_matrix(
				std::int32_t         prim,
				std::int32_t	      at_x,
				std::int32_t       at_y,
				std::int32_t	      at_z,
				std::int32_t         i_yaw,
				std::int32_t         i_pitch,
				std::int32_t         i_roll,
				NIGHT_Colour *lpc);

//
// The relfections are calculated once and then reused. The init() function clears
// all old reflection data.
//

void MESH_init_reflections();

//
// Draws the reflection of the given object. The reflection is always
// at the bottom of the prim.
//

void MESH_draw_reflection(
		std::int32_t         prim,
		std::int32_t         at_x,
		std::int32_t         at_y,
		std::int32_t         at_z,
		std::int32_t         at_yaw,
		NIGHT_Colour *lpc);


//
// Draws a mesh using the faces and textures from the given prim, but the
// points from the two given morphs.
//

void MESH_draw_morph(
		std::int32_t         prim,
		std::uint8_t         morph1,
		std::uint8_t         morph2,
		std::uint16_t		  tween,		// 0 - 256         
		std::int32_t	      at_x,
		std::int32_t       at_y,
		std::int32_t	      at_z,
		std::int32_t         i_yaw,
		std::int32_t         i_pitch,
		std::int32_t         i_roll,
		NIGHT_Colour *lpc);




#endif


