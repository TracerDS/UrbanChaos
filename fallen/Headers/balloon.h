//
// Balloons.
//

#ifndef _BALLOON_
#define _BALLOON_



//
// The points of a balloon.
//

typedef struct
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t dx;
	std::int32_t dy;
	std::int32_t dz;

} BALLOON_Point;

#define BALLOON_POINTS_PER_BALLOON 4


#define BALLOON_TYPE_UNUSED	0
#define BALLOON_TYPE_YELLOW 1
#define BALLOON_TYPE_RED    2
#define BALLOON_TYPE_NUMBER 3

typedef struct
{
	std::uint8_t type;
	std::uint8_t next;		// The next balloon in the linked list of balloons attached to this person.
	std::uint16_t yaw;		// 0xffff => This balloon is unused.
	std::uint16_t pitch;
	std::uint16_t thing;	// The thing this balloon is attached to.
	BALLOON_Point bp[BALLOON_POINTS_PER_BALLOON];

} BALLOON_Balloon;

#define BALLOON_MAX_BALLOONS 32

extern BALLOON_Balloon *BALLOON_balloon;//[BALLOON_MAX_BALLOONS];
extern std::int32_t           BALLOON_balloon_upto;



//
// Deletes all the balloons.
//

void BALLOON_init();


//
// Attaches a balloon to a thing.
// 

std::uint8_t BALLOON_create(
		std::uint16_t thing,
		std::uint8_t type);

//
// Releases the balloon. When the balloon has floated up enough it
// dissapears and a new person can use it.
//

void BALLOON_release(std::uint8_t balloon);

//
// Sees if there is a spare balloon for the given person to grab.
// If there is it makes the person grab it.
//

void BALLOON_find_grab(std::uint16_t thing);


//
// Processes all the balloons.
//

void BALLOON_process();



#endif
