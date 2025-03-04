// vehicle.h
//
// vehicle physics

#pragma once
#include "../Headers/State.h"

#define	FLAG_VEH_DRIVING	 (1<<0)
#define	FLAG_VEH_WHEEL1_GRIP (1<<1)
#define	FLAG_VEH_WHEEL2_GRIP (1<<2)
#define	FLAG_VEH_WHEEL3_GRIP (1<<3)
#define	FLAG_VEH_WHEEL4_GRIP (1<<4)
#define	FLAG_VEH_ANIMATING	 (1<<5)
#define FLAG_VEH_FX_STATE	 (1<<6)
#define FLAG_VEH_SHOT_AT	 (1<<7)
#define FLAG_VEH_STALLED	 (1<<8)	// Not allowed to be driven any more.
#define FLAG_VEH_IN_AIR		 (1<<9)

// Vehicle structure

typedef struct	
{
	std::uint16_t	Compression;
	std::uint16_t	Length;
} Suspension;

// DButtons bits

#define	VEH_ACCEL		1
#define VEH_DECEL		2
#define VEH_FASTER		4
#define VEH_SIREN		8

// speed limits

#define	VEH_SPEED_LIMIT		750		// the speed limit ("35 mph")
#define VEH_REVERSE_SPEED	300		// the speed limit for reversing

typedef struct 
{
	DrawTween	Draw;

	Suspension	Spring[4];		// spring data
	std::int32_t		DY[4];			// DY for each car point

	std::int32_t		Angle;
	std::int32_t		Tilt;
	std::int32_t		Roll;

	std::int8_t		Steering;		// -64 to +64 or -1 to +1
	std::uint8_t		IsAnalog;		// specifies range of steering
	std::uint8_t		DControl;		// digital control input
	std::uint8_t		GrabAction;		// set if we've grabbed the action key

	std::int16_t		Wheel;			// steering wheel position
	std::uint16_t		Allocated;		// 0 if not allocated

	std::uint16_t		Flags;
	std::uint16_t		Driver;

	std::uint16_t		Passenger;		// A linked list of passengers in this vehicle.
	std::uint16_t		Type;

	std::uint8_t		still;			// How long has the car been at zero velocity for?
	std::uint8_t		dlight;			// dynamic light thingy
	std::uint8_t		key;			// key to unlock, or SPECIAL_NONE
	std::uint8_t		Brakelight;		// brakelight state

	std::uint8_t		damage[6];		// damage counts
	std::int16_t		Spin;

	std::int32_t		VelX;			// X velocity
	std::int32_t		VelY;			// Y velocity
	std::int32_t		VelZ;			// Z velocity
	std::int32_t		VelR;			// rotational velocity

	std::int16_t		WheelAngle;		// wheel angle
	std::uint8_t		Siren;			// siren on/off?
	std::uint8_t		LastSoundState;	// for comparison with the current one

	std::int8_t		Dir;			// +2 = driving forwards, +1 = driving forwards but braking, -1,-2 same for backwards, 0 = stopped
	std::uint8_t		Skid;			// skidding? 0 = no, 1 - SKID_START-1 = maybe; >=SKID_START = yes
	std::uint8_t		Stable;			// stable?
	std::uint8_t		Smokin;			// smoking?

	std::uint8_t		Scrapin;		// scraping metal against a wall?
	std::uint8_t		OnRoadFlags;	// flag for each wheel - is it on the road?
	std::int16_t		Health;			// How close the car is to blowing up... (Starts at 200 like people)

//#ifndef PSX
	std::int32_t	oldX[4],oldZ[4];
//#endif


}Vehicle;

typedef Vehicle* VehiclePtr;

#define RMAX_VEHICLES	40
#define MAX_VEHICLES	(save_table[SAVE_TABLE_VEHICLE].Maximum)
#define	VEH_NULL		65000

// state

void VEH_driving(Thing *);
extern StateFunction VEH_statefunctions[];

void init_vehicles();

#define VEH_TYPE_VAN		0
#define VEH_TYPE_CAR		1
#define VEH_TYPE_TAXI		2
#define VEH_TYPE_POLICE		3
#define VEH_TYPE_AMBULANCE	4
#define VEH_TYPE_JEEP		5
#define	VEH_TYPE_MEATWAGON	6
#define VEH_TYPE_SEDAN		7
#define VEH_TYPE_WILDCATVAN 8
#define VEH_TYPE_NUMBER		9

THING_INDEX VEH_create(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t yaw,
		std::int32_t pitch,
		std::int32_t roll,
		std::int32_t type,
		std::uint8_t key,		// The special needed to unlock the car or SPECIAL_NONE if you don't need a key
		std::uint8_t colour);	// The tint colour of the car.


//
// The main prim used for the body of the given type of vehicle.
//

std::uint16_t get_vehicle_body_prim(std::int32_t type);


//
// The offset in Y of the positino of the body prim relative to the (x,y,z) of the vehicle.
// This is returned in large 16-bits per mapsquare coordinates.
//

std::int32_t get_vehicle_body_offset(std::int32_t type);


//
// What is this vehicle going to run-over?  This function fills out
// the given array with the thing it thinks it is going to run over.
// Returns the number of runover candidates.
//

//
// THIS FUNCTION WORKS FOR ALL BIKES AS WELL AS CARS!
// 

std::int32_t VEH_find_runover_things(Thing *p_vehicle, std::uint16_t thing_index[], std::int32_t max_number, std::int32_t dangle);


// hit a person with a car

extern std::int32_t GetRunoverHP(Thing* p_car, Thing* p_person, std::int32_t min_hp);

//
// This collide function will be useful elsewhere.
//

#define VEH_COL_TYPE_BBOX     0
#define VEH_COL_TYPE_CYLINDER 1

typedef struct
{
	std::uint16_t type;
	std::uint16_t ob_index;	// If this collision came from an OB this is its OB_index and position.
	Thing* veh;		// if this collision came from a vehicle or a Balrog, this is a pointer to it
	std::uint16_t mid_x;
	std::uint16_t mid_y;
	std::uint16_t mid_z;
	std::uint16_t height;
	std::int16_t min_x;
	std::int16_t max_x;
	std::int16_t min_z;
	std::int16_t max_z;
	std::uint16_t radius_or_yaw;

} VEH_Col;

#define VEH_MAX_COL 8

extern VEH_Col VEH_col[VEH_MAX_COL];
extern std::int32_t   VEH_col_upto;

//
// Finds all the things that will be collided with by VEH_collide_line().
//

void VEH_collide_find_things(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t radius,
		std::int32_t ignore_thing_index,
		std::int32_t ignore_prims = false);

//
// Returns the position where one enter/exists a vehicle.
//

void VEH_find_door(
		Thing *p_vehicle,
		std::int32_t  i_am_a_passenger,
		std::int32_t *x,
		std::int32_t *z);



//
// Returns nullptr if there isn't a driver...
//

Thing *get_vehicle_driver(Thing *p_vehicle);

//
// initialize crumple zones - calc_prim_info() must have been called
//

void VEH_init_vehinfo();

//
// get vertex -> zone assignment map
//

std::uint8_t* VEH_get_assignments(std::uint32_t prim);

//
// reinit after teleport
//

void reinit_vehicle(Thing* p_thing);



//
// Getting the position of each of the vehicle wheels.
//

void vehicle_wheel_pos_init(Thing *p_vehicle);
void vehicle_wheel_pos_get(
		std::int32_t  which,
		std::int32_t *wx,
		std::int32_t *wy,
		std::int32_t *wz);













