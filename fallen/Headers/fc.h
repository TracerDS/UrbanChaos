//
// The final camera?
//

#ifndef _FC_
#define _FC_




//
// The camera.
//

typedef struct
{
	//
	// What the camera is looking at.
	//

	Thing *focus;			// nullptr => Camera is not used.
	std::int32_t  focus_x;
	std::int32_t  focus_y;
	std::int32_t  focus_z;
	std::int32_t  focus_yaw;
	std::uint8_t  focus_in_warehouse;

	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t want_x;
	std::int32_t want_y;
	std::int32_t want_z;
	std::int32_t dx;
	std::int32_t dy;
	std::int32_t dz;
	std::int32_t yaw;
	std::int32_t pitch;
	std::int32_t roll;
	std::int32_t want_yaw;
	std::int32_t want_pitch;
	std::int32_t want_roll;
	std::int32_t lens;
	std::int32_t toonear;
	std::int32_t rotate;
	std::int32_t nobehind;
	std::int32_t lookabove;
	std::uint8_t shake;
	std::int32_t cam_dist;
	std::int32_t cam_height;
	std::int32_t toonear_dist;
	std::int32_t toonear_x;
	std::int32_t toonear_y;
	std::int32_t toonear_z;
	std::int32_t toonear_yaw;
	std::int32_t toonear_pitch;
	std::int32_t toonear_roll;
	std::int32_t toonear_focus_yaw;
	std::int32_t smooth_transition;

} FC_Cam;

#define FC_MAX_CAMS 2

extern FC_Cam FC_cam[FC_MAX_CAMS];


//
// Creates nice nullptr cameras.
//

void FC_init();


//
// Sets up the camera.
//

void FC_look_at(std::int32_t cam, std::uint16_t thing_index);
void FC_move_to(std::int32_t cam, std::int32_t world_x, std::int32_t world_y, std::int32_t world_z);	// 8-bits per mapsquare.
void FC_change_camera_type(std::int32_t cam,std::int32_t cam_type);


//
// Rotation about the focus thing.
// 

void FC_rotate_left (std::int32_t cam);
void FC_rotate_right(std::int32_t cam);
void FC_kill_player_cam(Thing *p_thing);


//
// Processes the cameras.
//

void FC_process();


//
// A fast LOS reject. Can the camera see this person?
//

std::int32_t FC_can_see_person(std::int32_t cam, Thing *p_person);


//
// Positions the camera over the shoulder of the focus thing and
// at the given pitch.
//

void FC_position_for_lookaround(std::int32_t cam, std::int32_t pitch);


//
// Forces the camera to behind the player.
//

void FC_force_camera_behind(std::int32_t cam);


//
// Initialises the camera for the start of the game.
// Make sure you have already call FC_look_at()
//

void FC_setup_initial_camera(std::int32_t cam);


//
// Tells the FC module that an explosion went off at (x,y,z) and that
// any nearby cameras should start to shake.
//

void FC_explosion(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t force);	// 0 <= force <= 400 ish


#endif
