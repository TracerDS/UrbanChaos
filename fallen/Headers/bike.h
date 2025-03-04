//
// Motobikes.
//

#pragma once

#ifdef BIKE

//
// The different modes the bikes can be in.
//

#define BIKE_MODE_PARKED		0
#define BIKE_MODE_MOUNTING		1		// Somebody is getting on this bike.
#define BIKE_MODE_DRIVING		2		// Somebody is driving this bike.
#define BIKE_MODE_DISMOUNTING	3		// Somebody is getting off this bike.


//
// So we dont have to have to BIKE Genus in the header file.
//

typedef struct 
{
	std::uint16_t yaw;
	std::uint16_t pitch;

	std::uint8_t flag;
	std::uint8_t mode;
	std::int8_t accel;
	std::int8_t steer;

	std::int32_t back_x;
	std::int32_t back_y;
	std::int32_t back_z;

	std::int32_t back_dx;
	std::int32_t back_dy;
	std::int32_t back_dz;

	std::int32_t front_x;
	std::int32_t front_y;
	std::int32_t front_z;

	std::int32_t front_dy;

	//
	// Suspension on the wheels.
	//

	std::int32_t wheel_y_back;
	std::int32_t wheel_y_front;

	std::int32_t wheel_dy_back;
	std::int32_t wheel_dy_front;

	//
	// The wheel rotations.
	//

	std::uint16_t wheel_rot_front;
	std::uint16_t wheel_rot_back;

	// Tyre tracks

	std::uint16_t tyrelast;
	std::uint16_t ribbon;
	std::uint16_t ribbon2;

	//
	// Who is driving the bike.
	//

	std::uint16_t driver;

	std::int16_t SlideTimer;
	std::uint8_t dirt;				// If the bike is kicking up dirt...
	std::uint8_t padding;

} BIKE_Bike;


//struct bike_bike;
//typedef struct bike_bike BIKE_Bike;
typedef BIKE_Bike *BikePtr;

#define BIKE_MAX_BIKES 2 //8

#define TO_BIKE(t)				(&BIKE_bike[t])
#define	BIKE_NUMBER(t)			(COMMON_INDEX)(t-TO_BIKE(0))


extern BIKE_Bike *BIKE_bike; //[BIKE_MAX_BIKES];


//
// Initialises all the bike structures.
//

void BIKE_init();


//
// Creates a new bike. Bikes are always on the ground.
//

std::uint16_t BIKE_create(
		std::int32_t x,
		std::int32_t z,
		std::int32_t yaw);

//
// Returns the THING_INDEX of a bike the person can get onto.
//

std::int32_t BIKE_person_can_mount(Thing *p_person);


//
// Tells the bike that somebody is getting on it.  It starts animating.
//

void BIKE_set_mounting(Thing *p_bike, Thing *p_person);

//
// Tells the bike that nobody is on it any more.
// 

void BIKE_set_parked(Thing *p_bike);


//
// How to steer a bike.
//

typedef struct
{
	std::int8_t steer;
	std::int8_t accel;
	std::int8_t brake;
	std::uint8_t wheelie;

} BIKE_Control;

BIKE_Control BIKE_control_get(Thing *p_bike);
void         BIKE_control_set(Thing *p_bike, BIKE_Control bc);


//
// Returns the roll of the bike.
//

std::int32_t BIKE_get_roll(Thing *p_bike);


//
// Returns the speed of the bike.
//

std::int32_t BIKE_get_speed(Thing *p_bike);


//
// Bike drawing info
// 

typedef struct
{
	std::uint16_t yaw;
	std::uint16_t pitch;
	std::uint16_t roll;
	std::uint16_t steer;

	std::uint16_t front_x;
	std::int16_t front_y;
	std::uint16_t front_z;
	std::uint16_t front_rot;

	std::uint16_t back_x;
	std::int16_t back_y;
	std::uint16_t back_z;
	std::uint16_t back_rot;

} BIKE_Drawinfo;

BIKE_Drawinfo BIKE_get_drawinfo(Thing *p_bike);


#endif

