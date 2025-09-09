//
// An 'intelligent' camera?
//

#ifndef _CAM_
#define _CAM_

//
// Set up the camera. Make sure the camera has a sensible focus,
// zoom and mode before you call CAM_process().
//

#define CAM_MODE_CRAPPY 0
#define CAM_MODE_NORMAL 1
#define CAM_MODE_STATIONARY 2
#define CAM_MODE_BEHIND 3
#define CAM_MODE_FIRST_PERSON 4
#define CAM_MODE_THIRD_PERSON 5
#define CAM_MODE_SHOOT_NORMAL 6
#define CAM_MODE_FIGHT_NORMAL 7

#define CAM_LENS_NORMAL 0
#define CAM_LENS_WIDEANGLE 1
#define CAM_LENS_ZOOM 2

extern std::int32_t CAM_lens;

void CAM_set_mode(std::int32_t mode);
void CAM_set_focus(Entity *focus);
void CAM_set_zoom(std::int32_t zoom);
void CAM_set_collision(std::int32_t boolean);
void CAM_set_behind_up(std::int32_t behind_up);
void CAM_set_pos(
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z);
void CAM_set_angle(
    std::int32_t yaw,
    std::int32_t pitch,
    std::int32_t roll);

//
// Predefined camera types.
//

#define CAM_TYPE_STANDARD 1
#define CAM_TYPE_LOWER 2
#define CAM_TYPE_BEHIND 3
#define CAM_TYPE_WIDE 4
#define CAM_TYPE_ZOOM 5

void CAM_set_type(std::int32_t type); // Sets up the mode, focus and zoom stuff to the given type.
std::int32_t CAM_get_type();          // Returns either the last type set or nullptr if no type has been set.

//
// Gives the camera the shakes...
//

void CAM_set_shake(std::uint8_t amount);

//
// In first person mode.
//

void CAM_set_dangle(std::int32_t dyaw, std::int32_t dpitch);
void CAM_get_dangle(std::int32_t *dyaw, std::int32_t *dpitch);

//
// Returns the current camera mode.
//

std::int32_t CAM_get_mode();
std::int32_t CAM_get_zoom();

//
// Processes the camera.
//

void CAM_process();

//
// In normal mode, these functions tell the camera to rotate left
// and right about the focus thing.
//

void CAM_rotate_left();
void CAM_rotate_right();

//
// Returns where the camera is.
//

float CAM_get_ryaw();

void CAM_get(
    std::int32_t *world_x,
    std::int32_t *world_y,
    std::int32_t *world_z,
    std::int32_t *yaw,
    std::int32_t *pitch,
    std::int32_t *roll,
    float *radians_yaw,
    float *radians_pitch,
    float *radians_roll);

void CAM_get_psx(
    std::int32_t *world_x,
    std::int32_t *world_y,
    std::int32_t *world_z,
    std::int32_t *yaw,
    std::int32_t *pitch,
    std::int32_t *roll,
    std::int32_t *radians_yaw,
    std::int32_t *radians_pitch,
    std::int32_t *radians_roll);

void CAM_get_pos(
    std::int32_t *world_x,
    std::int32_t *world_y,
    std::int32_t *world_z);

//
// Camera position relative to the focus thing.
//

void CAM_get_dpos(
    std::int32_t *dpos_x,
    std::int32_t *dpos_y,
    std::int32_t *dpos_z,
    std::int32_t *yaw,
    std::int32_t *pitch);

void CAM_set_dpos(
    std::int32_t dpos_x,
    std::int32_t dpos_y,
    std::int32_t dpos_z,
    std::int32_t yaw,
    std::int32_t pitch);

//
// Puts the camera in a good position to watch the given
// person climb out of the sewers.
//

void CAM_set_to_leave_sewers_position(Entity *);

//
// Sets the camera to look at the focus thing.
//

void CAM_look_at_thing(std::int32_t swoop);

#endif
