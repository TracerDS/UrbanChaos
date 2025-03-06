//
// Texture handling is wierd!
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "crinkle.h"

//
// The shadow texture page is generated by you! This is its size.
// The is the texture page for video playback.
//

#define TEXTURE_SHADOW_SIZE 64
#define TEXTURE_VIDEO_SIZE 64

#define FACE_PAGE_OFFSET (8 * 64)

//
// Chooses the texture set to use. If the current texture set is
// different from the new one, then all the textures are released.
//

void TEXTURE_choose_set(std::int32_t number);

//
// Fixed prims to work with the individual texture pages. Call this function BEFORE
// you load the textures! It works out which pages are needed.
//

void TEXTURE_fix_prim_textures(std::int32_t flag);
void TEXTURE_fix_texture_styles();

//
// The number of standard texture pages.
//

extern std::int32_t TEXTURE_page_num_standard;

#ifdef TARGET_DC
extern std::int32_t TEXTURE_page_background_use_instead;
extern std::int32_t TEXTURE_page_background_use_instead2;

extern std::int32_t TEXTURE_page_joypad_a;
extern std::int32_t TEXTURE_page_joypad_b;
extern std::int32_t TEXTURE_page_joypad_c;
extern std::int32_t TEXTURE_page_joypad_x;
extern std::int32_t TEXTURE_page_joypad_y;
extern std::int32_t TEXTURE_page_joypad_z;
extern std::int32_t TEXTURE_page_joypad_l;
extern std::int32_t TEXTURE_page_joypad_r;
extern std::int32_t TEXTURE_page_joypad_pad_l;
extern std::int32_t TEXTURE_page_joypad_pad_r;
extern std::int32_t TEXTURE_page_joypad_pad_d;
extern std::int32_t TEXTURE_page_joypad_pad_u;
#endif

extern std::int32_t TEXTURE_page_snowflake;
extern std::int32_t TEXTURE_page_sparkle;
extern std::int32_t TEXTURE_page_explode2;
extern std::int32_t TEXTURE_page_explode1;
extern std::int32_t TEXTURE_page_bigbang;
extern std::int32_t TEXTURE_page_face1;
extern std::int32_t TEXTURE_page_face2;
extern std::int32_t TEXTURE_page_face3;
extern std::int32_t TEXTURE_page_face4;
extern std::int32_t TEXTURE_page_face5;
extern std::int32_t TEXTURE_page_face6;
extern std::int32_t TEXTURE_page_fog;
extern std::int32_t TEXTURE_page_moon;
extern std::int32_t TEXTURE_page_clouds;
extern std::int32_t TEXTURE_page_water;
extern std::int32_t TEXTURE_page_puddle;
extern std::int32_t TEXTURE_page_drip;
extern std::int32_t TEXTURE_page_shadow;
extern std::int32_t TEXTURE_page_bang;
extern std::int32_t TEXTURE_page_font;
extern std::int32_t TEXTURE_page_logo;
extern std::int32_t TEXTURE_page_sky;
extern std::int32_t TEXTURE_page_flames;
extern std::int32_t TEXTURE_page_smoke;
extern std::int32_t TEXTURE_page_flame2;
extern std::int32_t TEXTURE_page_steam;
extern std::int32_t TEXTURE_page_menuflame;
extern std::int32_t TEXTURE_page_barbwire;
extern std::int32_t TEXTURE_page_font2d;
extern std::int32_t TEXTURE_page_dustwave;
extern std::int32_t TEXTURE_page_flames3;
extern std::int32_t TEXTURE_page_bloodsplat;
extern std::int32_t TEXTURE_page_bloom1;
extern std::int32_t TEXTURE_page_bloom2;
extern std::int32_t TEXTURE_page_hitspang;
extern std::int32_t TEXTURE_page_lensflare;
extern std::int32_t TEXTURE_page_tyretrack;
extern std::int32_t TEXTURE_page_envmap; // The environment map on metal
extern std::int32_t TEXTURE_page_winmap; // The environment map on windows
extern std::int32_t TEXTURE_page_leaf;
extern std::int32_t TEXTURE_page_raindrop;
extern std::int32_t TEXTURE_page_footprint;
extern std::int32_t TEXTURE_page_angel;
extern std::int32_t TEXTURE_page_devil;
extern std::int32_t TEXTURE_page_smoker;
extern std::int32_t TEXTURE_page_target;
extern std::int32_t TEXTURE_page_newfont;
extern std::int32_t TEXTURE_page_droplet;
extern std::int32_t TEXTURE_page_press1;
extern std::int32_t TEXTURE_page_press2;
extern std::int32_t TEXTURE_page_ic;
extern std::int32_t TEXTURE_page_ic2;
extern std::int32_t TEXTURE_page_lcdfont;
extern std::int32_t TEXTURE_page_smokecloud;
extern std::int32_t TEXTURE_page_menulogo;
extern std::int32_t TEXTURE_page_polaroid;
extern std::int32_t TEXTURE_page_bigbutton;
extern std::int32_t TEXTURE_page_bigleaf;
extern std::int32_t TEXTURE_page_bigrain;
extern std::int32_t TEXTURE_page_finalglow;
extern std::int32_t TEXTURE_page_tinybutt;
extern std::int32_t TEXTURE_page_lcdfont_alpha;
extern std::int32_t TEXTURE_page_flames_alpha;
extern std::int32_t TEXTURE_page_tyretrack_alpha;
extern std::int32_t TEXTURE_page_people3;
extern std::int32_t TEXTURE_page_ladder;
extern std::int32_t TEXTURE_page_fadecat;
extern std::int32_t TEXTURE_page_fade_MF;
extern std::int32_t TEXTURE_page_shadowoval;
extern std::int32_t TEXTURE_page_rubbish;
extern std::int32_t TEXTURE_page_lastpanel;
extern std::int32_t TEXTURE_page_lastpanel2;
extern std::int32_t TEXTURE_page_sign;
extern std::int32_t TEXTURE_page_pcflamer;
extern std::int32_t TEXTURE_page_shadowsquare;
extern std::int32_t TEXTURE_page_litebolt;
extern std::int32_t TEXTURE_page_ladshad;
extern std::int32_t TEXTURE_page_meteor;
extern std::int32_t TEXTURE_page_splash;

extern std::int32_t TEXTURE_num_textures; // The total number of pages.

//
// Returns the texture handle of the given page.
// DX6: returns a D3DTexture ptr to give to SET_TEXTURE macro
//

// D3DTEXTUREHANDLE TEXTURE_get_handle(std::int32_t page);
LPDIRECT3DTEXTURE2 TEXTURE_get_handle(std::int32_t page);

#ifdef TEX_EMBED
class D3DTexture;
D3DTexture *TEXTURE_get_D3DTexture(std::int32_t page);
#endif

//
// The handle of each crinkle. nullptr => No crinkle for that page.
//

extern CRINKLE_Handle TEXTURE_crinkle[22 * 64];

//
// To update the shadow texture page, first lock it, then
// write into it, then unlock it, then update it... phew!
//

//
// These are only valid after a successful lock.
//

extern std::uint16_t *TEXTURE_shadow_bitmap;
extern std::int32_t TEXTURE_shadow_pitch; // In bytes!
extern std::int32_t TEXTURE_shadow_mask_red;
extern std::int32_t TEXTURE_shadow_mask_green;
extern std::int32_t TEXTURE_shadow_mask_blue;
extern std::int32_t TEXTURE_shadow_mask_alpha;
extern std::int32_t TEXTURE_shadow_shift_red;
extern std::int32_t TEXTURE_shadow_shift_green;
extern std::int32_t TEXTURE_shadow_shift_blue;
extern std::int32_t TEXTURE_shadow_shift_alpha;

std::int32_t TEXTURE_shadow_lock(); // False on failure.
void TEXTURE_shadow_unlock();
void TEXTURE_shadow_update();

std::int32_t TEXTURE_86_lock(); // False on failure.
void TEXTURE_86_unlock();
void TEXTURE_86_update();

//
// Returns the four texture coordinates of the given MiniTextureBits structure
//

void TEXTURE_get_minitexturebits_uvs(
    std::uint16_t texture,
    std::int32_t *page,
    float *u0,
    float *v0,
    float *u1,
    float *v1,
    float *u2,
    float *v2,
    float *u3,
    float *v3);

//
// Gives the uv of the top left of the given texture square and
// returns the page.
//

std::int32_t TEXTURE_get_fiddled_position(
    std::int32_t square_u,
    std::int32_t square_v,
    std::int32_t page,
    float *u,
    float *v);

//
// Makes black the colour key for the given texture.
//

#ifndef TARGET_DC
void TEXTURE_set_colour_key(std::int32_t page);
#endif

//
// Makes all the textures greyscale.
//

void TEXTURE_set_greyscale(std::int32_t is_greyscale);

//
// Change a specific texture to a different tga
//

void TEXTURE_set_tga(std::int32_t page, char *fn);

//
// Frees all the texture pages.
//

void TEXTURE_free();

// Frees all the non-frontend texture pages.
void TEXTURE_free_unneeded();

//
// Returns what sort it thinks the given page looks like.
//

#define TEXTURE_LOOK_ROAD 0
#define TEXTURE_LOOK_GRASS 1
#define TEXTURE_LOOK_DIRT 2
#define TEXTURE_LOOK_SLIPPERY 3

extern std::int32_t TEXTURE_liney;
extern std::int32_t TEXTURE_av_r;
extern std::int32_t TEXTURE_av_g;
extern std::int32_t TEXTURE_av_b;

std::int32_t TEXTURE_looks_like(std::int32_t page);

#endif
