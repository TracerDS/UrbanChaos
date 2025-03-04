// Engine.h
// Guy Simmons, 22nd October 1997.

#pragma once
#include "../../MFStdLib/Headers/MFStdLib.h"
//---------------------------------------------------------------

#ifdef	VERSION_D3D

typedef struct
{
	float	CameraX,
			CameraY,
			CameraZ;
	std::int32_t	CameraAngle,
			CameraRoll,
			CameraTilt,
			CameraRAngle;
}Camera;

#elif defined(VERSION_GLIDE)

typedef struct
{
}Camera;

#elif defined(VERSION_PSX)

typedef struct
{
}Camera;

#endif

//---------------------------------------------------------------

struct M31
{
	float	R[3];
};

struct M33
{
	M31		R0,
			R1,
			R2;
};

#define		UV_XX					R0.R[0]
#define		UV_YX					R0.R[1]
#define		UV_ZX					R0.R[2]

#define		UV_XY					R1.R[0]
#define		UV_YY					R1.R[1]
#define		UV_ZY					R1.R[2]

#define		UV_XZ					R2.R[0]
#define		UV_YZ					R2.R[1]
#define		UV_ZZ					R2.R[2]

typedef struct
{
	float		HalfViewHeight,
				HalfViewWidth,
				OriginX,
				OriginY,
				OriginZ,
				ViewHeight,
				ViewWidth,
				Lens;
	std::int32_t		BucketSize;
	M33			CameraMatrix;
}Engine;

bool init_3d_engine();
void fini_3d_engine();
void game_engine(Camera *the_view);
void engine_attract();
void engine_win_level();
void engine_lose_level();

//---------------------------------------------------------------

void temp_setup_map();
bool new_init_3d_engine();
void new_engine(Camera *the_view);

//---------------------------------------------------------------

//
// After you have loaded all the prims, call this function. It
// fixes the texture coordinates of the prims if the engine has
// fiddled with the texture pages.
// 

void engine_fiddle_prim_textures();

//
// Given a texture square coordinate on a page, it returns the
// page and texture square coordinates of the fiddled position.
//
// 'square_u' and 'square_v' should be between 0 and 7, and the
// fiddled position are returned.
//

std::int32_t TEXTURE_get_fiddled_position(
		std::int32_t  square_u,
		std::int32_t  square_v,
		std::int32_t  page,
		float *u,
		float *v);

//
// Debugging aids.
//

void e_draw_3d_line           (std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2);
void e_draw_3d_line_dir       (std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2);
void e_draw_3d_line_col       (std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2,std::int32_t r,std::int32_t g,std::int32_t b);
void e_draw_3d_line_col_sorted(std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2,std::int32_t r,std::int32_t g,std::int32_t b);
void e_draw_3d_mapwho         (std::int32_t x1,std::int32_t z1);
void e_draw_3d_mapwho_y       (std::int32_t x1,std::int32_t y1,std::int32_t z1);

//
// Messages drawn straight to the screen.
//

void MSG_add(char* message, ...);

//
// Engine stuff to help you draw straight to the screen...
//

void ENGINE_clear_screen();
void ENGINE_flip();
std::int32_t ENGINE_lock();		// Locks the screen.. returns true on success.
void ENGINE_unlock();

#include "..\ddengine\headers\font.h"



