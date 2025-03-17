#pragma once

#define KERB_HEIGHT 32.0F
#define KERB_HEIGHTI ((std::int32_t) (KERB_HEIGHT))

// Set this to 1 to enable TomF's new D3D-friendly engine.
// 0 enables the old engine again.
// NOTE! There are other versions of this define dotted around in other header
// files! Make sure they all agree or you'll have grief.
#ifdef TARGET_DC
#define USE_TOMS_ENGINE_PLEASE_BOB 1
#else
#define USE_TOMS_ENGINE_PLEASE_BOB 1
#endif

//
// Call once at the start of the whole program.
//

void AENG_init();

//
// Chooses the texture set to use. If the current texture set is
// different from the new one, then all the textures are released.
//

void TEXTURE_choose_set(std::int32_t number);

//
// Loads only the textures that are used in the prim faces and the current map.
//

// iStartCompletionBar = where it is now.
// iEndCompletionBar = where it needs to get to.
// iNumberTexturesProbablyLoaded = roughly how many textures will be loaded.
void TEXTURE_load_needed(char* fname_level,
                         int iStartCompletionBar = 0,
                         int iEndCompletionBar = 0,
                         int iNumberTexturesProbablyLoaded = 0);

//
// Loads the textures needed for the given prim object.
//

void TEXTURE_load_needed_object(std::int32_t prim_object);

//
// This function makes a local copy of the prim points for
// the engine's own use.
//

typedef struct
{
    float X;
    float Y;
    float Z;

} SVector_F;

extern SVector_F AENG_dx_prim_points[];

void AENG_create_dx_prim_points();

//
// After you have loaded all the prims, call this function. It
// fixes the texture coordinates of the prims if the engine has
// fiddled with the texture pages.
//

void TEXTURE_fix_texture_styles();
void TEXTURE_fix_prim_textures();

//
// Given a texture square coordinate on a page, it returns the
// page and texture square coordinates of the fiddled position.
//
// 'square_u' and 'square_v' should be between 0 and 7, and the
// fiddled position are returned.
//

std::int32_t TEXTURE_get_fiddled_position(
    std::int32_t square_u,
    std::int32_t square_v,
    std::int32_t page,
    float* u,
    float* v);

//
// Initialises a new frame.
// Draws the engine.
//

void AENG_set_camera(
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z,
    std::int32_t yaw,
    std::int32_t pitch,
    std::int32_t roll);

void AENG_set_camera_radians(
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z,
    float yaw,
    float pitch,
    float roll);

//
// Get/set draw distance
//

void AENG_set_draw_distance(std::int32_t dist);
std::int32_t AENG_get_draw_distance();

//
// Draws world lines.
//

// Just for debugging please!
void AENG_world_line(
    std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t width1, std::uint32_t colour1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t width2, std::uint32_t colour2,
    std::int32_t sort_to_front);

// For lines that aren't debug.
void AENG_world_line_nondebug(
    std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t width1, std::uint32_t colour1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t width2, std::uint32_t colour2,
    std::int32_t sort_to_front);

//
// Draws the line as a series of smaller lines so you can draw lines.
// with endpoints out of the view frustrum.
//

void AENG_world_line_infinite(
    std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t width1, std::uint32_t colour1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t width2, std::uint32_t colour2,
    std::int32_t sort_to_front);

void AENG_draw_col_tri(std::int32_t x0, std::int32_t y0, std::int32_t col0, std::int32_t x1, std::int32_t y1, std::int32_t col1, std::int32_t x2, std::int32_t z2, std::int32_t col2, std::int32_t layer);

//
// Older engine compatability.
//

void AENG_e_draw_3d_line(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2);
void AENG_e_draw_3d_line_dir(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2);
void AENG_e_draw_3d_line_col(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t r, std::int32_t g, std::int32_t b);
void AENG_e_draw_3d_line_col_sorted(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t r, std::int32_t g, std::int32_t b);
void AENG_e_draw_3d_mapwho(std::int32_t x1, std::int32_t z1);
void AENG_e_draw_3d_mapwho_y(std::int32_t x1, std::int32_t y1, std::int32_t z1);

void AENG_draw_rect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::int32_t col, std::int32_t layer, std::int32_t page);

#define e_draw_3d_line AENG_e_draw_3d_line
#define e_draw_3d_line_dir AENG_e_draw_3d_line_dir
#define e_draw_3d_line_col AENG_e_draw_3d_line_col
#define e_draw_3d_line_col_sorted AENG_e_draw_3d_line_col_sorted
#define e_draw_3d_mapwho AENG_e_draw_3d_mapwho
#define e_draw_3d_mapwho_y AENG_e_draw_3d_mapwho_y

//
// Sets the type of sky to use.
//

void AENG_set_sky_nighttime();
void AENG_set_sky_daytime(std::uint32_t bottom_colour, std::uint32_t top_colour);

//
// The new engines- you don't have to call these- just call
// AENG_draw() and it will take care of everything for you.
//

void AENG_draw_city();
void AENG_draw_inside();
void AENG_draw_sewer();
void AENG_draw_ns();

//
// Draw the correct engine given the current gamestate. If
// (draw_3d) then it draws the scene red/blue for use with
// 3d glasses. Make sure that the textures are all greyscale.
//

void AENG_draw(std::int32_t draw_3d);

//
// Clear the viewport (moved to just after flip for concurrency)
//

void AENG_clear_viewport();

//
// The scanner.
//

void AENG_draw_scanner(
    std::int32_t screen_x1,
    std::int32_t screen_y1,
    std::int32_t screen_x2,
    std::int32_t screen_y2,
    std::int32_t map_x,
    std::int32_t map_z,
    std::int32_t map_zoom, // The number of pixels per mapsquare in fixed-point 8.
    std::int32_t map_angle);

void AENG_draw_power(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, std::int32_t val, std::int32_t max);

//
// Draws the messages and the FPS stuff to the screen.
//

void AENG_draw_FPS();
void AENG_draw_messages();

//
// Fades in from black via the MF logo.
// Fades out to the mucky foot logo.
//

void AENG_fade_in(std::uint8_t amount);
void AENG_fade_out(std::uint8_t amount);

//
// Flips/blits the back-buffer.
//

void AENG_flip();
void AENG_blit();

//
// Adds a message to the message system.
//

void MSG_add(char* message, ...);
// #define MSG_add

//
// Drawing stuff straight to the screen...
//

void AENG_clear_screen();
std::int32_t AENG_lock();
std::int32_t FONT_draw(std::int32_t x, std::int32_t y, char* text, ...);
void AENG_unlock();

//
// Call once at the end of the whole program.
//

void AENG_fini();

// ========================================================
//
// EDITOR SUPPORT FUNCTIONS
//
// ========================================================

//
// Returns the position of the ray going through the
// given screen pixel.  Return false if no intersection
// happened!
//

std::int32_t AENG_raytraced_position(
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t* world_x,
    std::int32_t* world_y,
    std::int32_t* world_z,
    std::int32_t indoors = 0);

//
// Returns the position of the point above (wx,wz) in
// the world whose screeny coordinate is the same as
// the given point.
//

void AENG_raytraced_y_position(
    std::int32_t sy,
    std::int32_t wx,
    std::int32_t wz,
    std::int32_t* world_y);

//
// Draws a light. Returns bitfield to say if the given screen coord
// is over any part of the light.
//

#define AENG_MOUSE_OVER_LIGHT_BOT (1 << 0)
#define AENG_MOUSE_OVER_LIGHT_TOP (1 << 1)

std::uint32_t AENG_light_draw(
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t colour,
    std::uint8_t highlight); // How much to expand the light ball by...

//
// Draws the editor mode of the sewer editor.
//

void AENG_draw_sewer_editor(
    std::int32_t cam_x,
    std::int32_t cam_y,
    std::int32_t cam_z,
    std::int32_t cam_yaw,
    std::int32_t cam_pitch,
    std::int32_t cam_roll,
    std::int32_t mouse_x,
    std::int32_t mouse_y,
    std::int32_t* mouse_over_valid,
    std::int32_t* mouse_over_x,
    std::int32_t* mouse_over_y,
    std::int32_t* mouse_over_z,
    std::int32_t draw_prim_at_mouse,
    std::int32_t prim_object,
    std::int32_t prim_yaw);

//
// Draws text at the given point by calling FONT_buffer_add().
// So remember to call FONT_buffer_draw() before you flip the screen.
//

void AENG_world_text(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::uint8_t red,
    std::uint8_t blue,
    std::uint8_t green,
    std::uint8_t shadowed_or_not,
    char* fmt, ...);

//---------------------------------------------------------------
//	GUY.
//---------------------------------------------------------------

#define AENG_MOUSE_OVER_WAYPOINT (1 << 0)

std::uint32_t AENG_waypoint_draw(
    std::int32_t mx,
    std::int32_t my,
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t colour,
    std::uint8_t highlight);

std::uint32_t AENG_rad_trigger_draw(
    std::int32_t mx,
    std::int32_t my,
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t rad,
    std::uint32_t colour,
    std::uint8_t highlight);

//---------------------------------------------------------------
//	CANIS.
//---------------------------------------------------------------

void AENG_groundsquare_draw(
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t colour,
    std::uint8_t polyinit);

//---------------------------------------------------------------

//
// Drawing buffered up text anywhere on the screen.
//

#include "font.h"
#include "console.h"

//
// panel stuff
//
extern void PANEL_draw_health_bar(std::int32_t x, std::int32_t y, std::int32_t percentage);
extern void PANEL_draw_timer(std::int32_t time_in_hundreths, std::int32_t x, std::int32_t y);
extern void PANEL_draw_buffered(); // Actually draws the timers....
extern void PANEL_finish();

//
// detail level stuff
//

// read detail levels from disc
void AENG_read_detail_levels();

#ifdef TARGET_DC
// get the prevaling settings
void AENG_get_detail_levels( // int* stars,
    int* shadows,
    // int* moon_reflection,
    // int* people_reflection,
    int* puddles,
    int* dirt,
    int* mist,
    int* rain,
    int* skyline,
    // int* filter,
    // int* perspective,
    int* crinkles);

// change the prevaling settings
void AENG_set_detail_levels( // int stars,
    int shadows,
    // int moon_reflection,
    // int people_reflection,
    int puddles,
    int dirt,
    int mist,
    int rain,
    int skyline,
    // int filter,
    // int perspective,
    int crinkles);

#else
// get the prevaling settings
void AENG_get_detail_levels(int* stars,
                            int* shadows,
                            int* moon_reflection,
                            int* people_reflection,
                            int* puddles,
                            int* dirt,
                            int* mist,
                            int* rain,
                            int* skyline,
                            int* filter,
                            int* perspective,
                            int* crinkles);

// change the prevaling settings
void AENG_set_detail_levels(int stars,
                            int shadows,
                            int moon_reflection,
                            int people_reflection,
                            int puddles,
                            int dirt,
                            int mist,
                            int rain,
                            int skyline,
                            int filter,
                            int perspective,
                            int crinkles);

#endif