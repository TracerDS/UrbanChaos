//
// Light editor.
//

#ifndef _ED_
#define _ED_

#define LIGHT_FLAGS_INSIDE (1)

typedef struct
{
    std::uint8_t range;
    std::int8_t red;
    std::int8_t green;
    std::int8_t blue;
    std::uint8_t next;
    std::uint8_t used;
    std::uint8_t flags;
    std::uint8_t padding;
    //	std::uint16_t padding;
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;

} ED_Light;

#define ED_MAX_LIGHTS 256

extern ED_Light ED_light[ED_MAX_LIGHTS];
extern std::int32_t ED_light_free;

//
// Clears all the light info.
//

void ED_init();

//
// Creates a new light.
//
// Range goes from 1 - 255
// (R,G,B) are signed values (+/- 127)
//

std::int32_t ED_create(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t range,
    std::int32_t red,
    std::int32_t green,
    std::int32_t blue);

//
// Deletes one light or all the lights.
//

void ED_delete(std::int32_t light);
void ED_delete_all();

//
// Updates the given light.
//

void ED_light_move(
    std::int32_t light,
    std::int32_t x,
    std::int32_t y,
    std::int32_t z);

void ED_light_change(
    std::int32_t light,
    std::int32_t range,
    std::int32_t red,
    std::int32_t green,
    std::int32_t blue);

//
// Get/Set the ambient light colour.  Values go from 0 - 255
//

void ED_amb_get(
    std::int32_t *red,
    std::int32_t *green,
    std::int32_t *blue);

void ED_amb_set(
    std::int32_t red,
    std::int32_t green,
    std::int32_t blue);

//
// Do we have light under the lamposts? Is it nightime?
//

std::int32_t ED_lampost_on_get();
void ED_lampost_on_set(std::int32_t lamposts_are_on);

std::int32_t ED_night_get();
void ED_night_set(std::int32_t its_night_time);

std::int32_t ED_darken_bottoms_on_get();
void ED_darken_bottoms_on_set(std::int32_t darken_bottoms_on);

//
// Get/Set the lights under lamposts.
//
// Range goes from 1 - 255
// (R,G,B) are signed values (+/- 127)
//
//

void ED_lampost_get(
    std::int32_t *range,
    std::int32_t *red,
    std::int32_t *green,
    std::int32_t *blue);

void ED_lampost_set(
    std::int32_t range,
    std::int32_t red,
    std::int32_t green,
    std::int32_t blue);

//
// Get/Set the sky light colour.  Values go from 0 - 255
//

void ED_sky_get(
    std::int32_t *red,
    std::int32_t *green,
    std::int32_t *blue);

void ED_sky_set(
    std::int32_t red,
    std::int32_t green,
    std::int32_t blue);

//
// Undo / Redo functions.
//

void ED_undo_store(); // Remember the current state.

void ED_undo_undo();
void ED_undo_redo();

std::int32_t ED_undo_undo_valid();
std::int32_t ED_undo_redo_valid();

//
// Loading and saving. Return true on success.
//

std::int32_t ED_load(char *name);
std::int32_t ED_save(char *name);

//
// The format that everything is saved out in.
//
//  std::int32_t        sizeof(ED_Light)
//  std::int32_t        ED_MAX_LIGHTS
//  std::int32_t        sizeof(NIGHT_Colour)
//
//	ED_Light     ed_light[ED_MAX_LIGHTS];
//	std::int32_t        ed_light_free;
//	std::uint32_t        night_flag;
//	std::uint32_t        night_amb_d3d_colour;
//	std::uint32_t        night_amb_d3d_specular;
//	std::int32_t        night_amb_red;
//	std::int32_t        night_amb_green;
//	std::int32_t        night_amb_blue;
//  std::int8_t        night_lampost_red;
//  std::int8_t        night_lampost_green;
//  std::int8_t        night_lampost_blue;
//  std::uint8_t        padding;
//  std::int32_t        night_lampost_radius;
//	NIGHT_Colour night_sky_colour;
//

#endif
