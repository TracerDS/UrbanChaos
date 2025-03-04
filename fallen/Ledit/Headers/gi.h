//
// The interface between the game and the light editor.
//

#ifndef _GI_
#define _GI_

//
// Call once at the start of the light editor.
//

void GI_init();


//
// Releases any old stuff and loads the given map file.
// Enters city view automatically.
// Returns true on success.
//

std::int32_t GI_load_map(char* name);


//
// Changes the view. The view default to CITY view.
//

#define GI_VIEW_CITY	1
#define GI_VIEW_SEWERS	2

void GI_set_view(std::int32_t view);

//
// Renders the view into the the_display.lp_DD_BackSurface.
//

void GI_render_view_into_backbuffer(
		std::int32_t cam_x,
		std::int32_t cam_y,
		std::int32_t cam_z,
		std::int32_t cam_yaw,
		std::int32_t cam_pitch,
		std::int32_t cam_roll);

//
// Gives the position of the given screen pixel in the world.
// Returns false if there was no intersection.
//

std::int32_t GI_get_pixel_world_pos(
		std::int32_t  sx,
		std::int32_t  sy,
		std::int32_t *world_x,
		std::int32_t *world_y,
		std::int32_t *world_z,
		std::int32_t inside=0);


//
// Draws a light into the backbuffer and returns which bits of the
// light the mouse is over.
//

#define GI_MOUSE_OVER_LIGHT_BOT (1 << 0)
#define GI_MOUSE_OVER_LIGHT_TOP (1 << 1)

std::uint32_t GI_light_draw(
		std::int32_t sx,
		std::int32_t sy,
		std::int32_t lx,
		std::int32_t ly,
		std::int32_t lz,
		std::uint32_t colour,
		std::uint8_t highlight);	// Not a bool!


//
// Call once at the end of the light editor.
//

void GI_fini();

//---------------------------------------------------------------
//	GUY.
//---------------------------------------------------------------

std::uint32_t GI_waypoint_draw(
		std::int32_t sx,
		std::int32_t sy,
		std::int32_t lx,
		std::int32_t ly,
		std::int32_t lz,
		std::uint32_t colour,
		std::uint8_t highlight);


std::uint32_t GI_rad_trigger_draw(
		std::int32_t sx,
		std::int32_t sy,
		std::int32_t lx,
		std::int32_t ly,
		std::int32_t lz,
		std::uint32_t rad,
		std::uint32_t colour,
		std::uint8_t highlight);

//---------------------------------------------------------------
//  CANIS
//---------------------------------------------------------------

void GI_groundsquare_draw(
		std::int32_t lx,
		std::int32_t ly,
		std::int32_t lz,
		std::uint32_t colour,
		std::uint8_t polyinit);

//---------------------------------------------------------------

//---------------------------------------------------------------

#endif
