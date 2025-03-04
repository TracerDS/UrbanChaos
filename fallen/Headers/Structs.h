// Structs.h
// Guy Simmons, 17th October 1997.

#ifndef	STRUCTS_H
#define	STRUCTS_H

#include	"..\Editor\Headers\Anim.h"

//---------------------------------------------------------------

typedef struct
{
	std::int32_t		X,
				Y,
				Z;
}GameCoord;

struct SVector
{
	std::int32_t	X,Y,Z;
};

struct SmallSVector
{
	std::int16_t	X,Y,Z;
};

#ifndef	PSX
struct SVECTOR
{
	std::int32_t	X,Y,Z;
};

#endif

struct TinyXZ
{
	std::int8_t	Dx,Dz;
	std::int16_t	Angle;
};

struct MiniTextureBits
{
	std::uint16_t 	X:3;
	std::uint16_t 	Y:3;
	std::uint16_t	Page:4;
	std::uint16_t	Rot:2;
	std::uint16_t	Flip:2;
	std::uint16_t	Size:2;
};

typedef struct
{
	std::uint8_t action;
	std::uint8_t dir;
	std::uint8_t dest_x;
	std::uint8_t dest_z;
	
} MAV_Action;


//---------------------------------------------------------------

struct Thing;			//	Prototype the 'Thing' structure.
struct CommandList;		//	Prototype 'CommandList' structure.

#define	COMMON(TYPE)	std::uint8_t			TYPE;			\
						std::uint8_t			padding;		\
						THING_INDEX		Thing;			\
						std::uint32_t			Flags;			

/*
#define	COMMON(TYPE)	std::uint8_t			TYPE,			\
										State;			\
						std::uint16_t			CommandRef,		\
										Command;		\
						std::int16_t			Velocity;		\
						std::uint32_t			Flags;			\
						std::int32_t			Timer;			\
						CommandList		*ComList;		\
						THING_INDEX		Thing,padding;
*/

//---------------------------------------------------------------


#define	FLAGS_DRAW_SHADOW	(1<<0)


//---------------------------------------------------------------

#endif
