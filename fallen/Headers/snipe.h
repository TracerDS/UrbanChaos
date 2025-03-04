//
// Snipe rifle-mode stuff
//

#ifndef _SNIPE_
#define _SNIPE_


//
// The snipe camera.
//

extern std::int32_t SNIPE_on;			// true => Snipe mode is active
extern std::int32_t SNIPE_cam_x;
extern std::int32_t SNIPE_cam_y;
extern std::int32_t SNIPE_cam_z;
extern std::int32_t SNIPE_cam_yaw;		// 16-bit fixed
extern std::int32_t SNIPE_cam_pitch;	// 16-bit fixed
extern std::int32_t SNIPE_cam_lens;	// 16-bit fixed


//
// Turns on/off snipe mode.
//

void SNIPE_mode_on (std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t initial_yaw);	// yaw from 0 - 2047
void SNIPE_mode_off();

//
// Changes the sniping direction.
//

#define SNIPE_TURN_LEFT  (1 << 0)
#define SNIPE_TURN_RIGHT (1 << 1)
#define SNIPE_TURN_UP    (1 << 2)
#define SNIPE_TURN_DOWN  (1 << 3)

void SNIPE_turn(std::int32_t turn);

//
// Processes the snipe-view
//

void SNIPE_process();


//
// Shoots someone from the sniper view.
//

void SNIPE_shoot();


#endif
