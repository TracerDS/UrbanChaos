//
// A thrust ship.
//

#pragma once


//
// The size of the arena the ships are in.
//

#define SHIP_ARENA (32.0F)



//
// The ship flags.
// 

#define SHIP_FLAG_USED				(1 << 0)
#define SHIP_FLAG_ACTIVE            (1 << 1)	// If a ship is not active, then it is in the process of joining the game.
#define SHIP_FLAG_LOCAL             (1 << 2)	// true => This is ship belonging to the local player.
#define SHIP_FLAG_TRACTORING		(1 << 3)	// This ship has an active tractor beam.
#define SHIP_FLAG_COLLIDED			(1 << 4)	// Collided with the landscape last process
#define SHIP_FLAG_KEY_THRUST		(1 << 5)
#define SHIP_FLAG_KEY_LEFT			(1 << 6)
#define SHIP_FLAG_KEY_RIGHT			(1 << 7)
#define SHIP_FLAG_KEY_TRACTOR_BEAM	(1 << 8)	// The player has pressed the tractor beam key.


//
// The ships.
//

typedef struct
{
	std::uint16_t flag;
	std::uint8_t tb;		// The tractor beam if (flag & SHIP_FLAG_TRACTOR_BEAM)
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
	std::uint16_t padding;
	char name[32];
	std::int32_t active;	// The gameturn when this ship becomes active.
	float x;
	float y;
	float angle;
	float dx;
	float dy;
	float mass;		// Defaults to 1.0F
	float power;	// Defaults to 1.0F
	float hash;

} SHIP_Ship;

#define SHIP_MAX_SHIPS 16

extern SHIP_Ship SHIP_ship[SHIP_MAX_SHIPS];


//
// Initialises the ship module.
// 

void SHIP_init();


//
// Creates a new ship.  Returns nullptr on failure.
//

SHIP_Ship *SHIP_create(
				float  x,
				float  y,
				float  mass  = 1.0F,
				float  power = 1.0F,
				std::uint8_t  red   = 255,
				std::uint8_t  green = 255,
				std::uint8_t  blue  = 100,
				char* name  = "Player");

//
// Flags all the ship's that come alive on
// the given gameturn as being alive.
//

void SHIP_flag_active(std::int32_t gameturn);


//
// Processes one gameturn of all the ships. The gameturn the
// ships are being processed at. It isn't necessarily GAME_turn
// if rollback has occured.
//

void SHIP_process_all();


//
// Draws all the ships.
//

void SHIP_draw_all(float mid_x, float mid_y, float zoom);



