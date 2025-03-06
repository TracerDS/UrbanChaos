//
// Interface between the game and the light editor.
//

#include <MFStdLib.h>
#include "game.h"
#include "gi.h"
#include "morph.h"
#include "night.h"
#include "ob.h"
#include "trip.h"
#include "io.h"
#include "animtmap.h"
#include "dirt.h"
#include "mist.h"
#include "puddle.h"
#include "road.h"
#include "drip.h"
#include "shadow.h"
#include "interfac.h"
#include "mav.h"
#include "ns.h"
#include "elev.h"
#include "fc.h"

//
// The current view.
//

std::int32_t GI_view;

void GI_init() {
    FC_init();
    AENG_init();
    MORPH_load();
    ANIM_init();

    GI_view = 0;
}

std::int32_t GI_load_map(char *name) {
    //
    // This is important stuff... I guess.
    //

    void global_load(); // Defined in game.cpp loads stuff that gets screwed up by the editor.

    //	global_load(); // this gets called almost immediately in ELEV_game_init()

    //
    // Load our map.
    //

    ELEV_game_init(
        name,
        nullptr,
        nullptr,
        nullptr);

    //
    // Start off looking at the city.
    //

    GI_view = GI_VIEW_CITY;

    return true; // Nothing ever goes wrong... honest!
}

void GI_set_view(std::int32_t view) {
    switch (view) {
        case GI_VIEW_CITY:

            if (GI_view == GI_VIEW_SEWERS) {
                //
                // Clean up sewer stuff and initialise the city view.
                //

                GAME_FLAGS &= ~GF_SEWERS;

                NS_cache_fini();
                DIRT_init(100, 3, 3, INFINITY, INFINITY, INFINITY, INFINITY);
            }

            break;

        case GI_VIEW_SEWERS:

            if (GI_view == GI_VIEW_CITY) {
                //
                // Clean up city stuff and initialise the sewer view.
                //

                GAME_FLAGS |= GF_SEWERS;

                DIRT_init(0, 0, 0, INFINITY, INFINITY, INFINITY, INFINITY);
                NIGHT_destroy_all_cached_info();
                NS_cache_init();
            }

            break;

        default:
            ASSERT(0);
            break;
    }

    GI_view = view;
}

void GI_render_view_into_backbuffer(
    std::int32_t cam_x,
    std::int32_t cam_y,
    std::int32_t cam_z,
    std::int32_t cam_yaw,
    std::int32_t cam_pitch,
    std::int32_t cam_roll) {
    if (GAME_FLAGS & GF_SEWERS) {
        //
        // Animate the water.
        //

        DIRT_set_focus(cam_x, cam_z, 0x800);
        DIRT_process();
    }

    //
    // Increase the gameturn, otherwise facets aren't drawn!
    //

    GAME_TURN += 1;

    AENG_set_camera(
        cam_x,
        cam_y,
        cam_z,
        cam_yaw,
        cam_pitch,
        cam_roll);

    AENG_draw(false);
}

std::int32_t GI_get_pixel_world_pos(
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t *world_x,
    std::int32_t *world_y,
    std::int32_t *world_z,
    std::int32_t inside) {
    return AENG_raytraced_position(
        sx,
        sy,
        world_x,
        world_y,
        world_z,
        inside);
}

std::uint32_t GI_light_draw(
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t colour,
    std::uint8_t highlight) {
    std::uint32_t ans;

    ans = AENG_light_draw(
        sx,
        sy,
        lx,
        ly,
        lz,
        colour,
        highlight);

    return ans; // Make sure our flags are the same as the AENG flags!
}

void GI_fini() {
    AENG_fini();
}

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
    std::uint8_t highlight) {
    std::uint32_t ans;

    ans = AENG_waypoint_draw(
        sx,
        sy,
        lx,
        ly,
        lz,
        colour,
        highlight);

    return ans; // Make sure our flags are the same as the AENG flags!
}

//---------------------------------------------------------------

std::uint32_t GI_rad_trigger_draw(
    std::int32_t sx,
    std::int32_t sy,
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t rad,
    std::uint32_t colour,
    std::uint8_t highlight) {
    std::uint32_t ans;

    ans = AENG_rad_trigger_draw(
        sx,
        sy,
        lx,
        ly,
        lz,
        rad,
        colour,
        highlight);

    return ans; // Make sure our flags are the same as the AENG flags!
}

//---------------------------------------------------------------

void GI_groundsquare_draw(
    std::int32_t lx,
    std::int32_t ly,
    std::int32_t lz,
    std::uint32_t colour,
    std::uint8_t polyinit) {
    AENG_groundsquare_draw(lx, ly, lz, colour, polyinit);
}

//---------------------------------------------------------------
