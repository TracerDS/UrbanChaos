// Structs.h
// Guy Simmons, 27th March 1997.


#pragma once

#include	"DarkCity.h"


//---------------------------------------------------------------

typedef	struct
{
	std::int32_t		X,
				Y,
				Z;
}Coord;

//---------------------------------------------------------------

/*
typedef	struct
{
	Matrix33		*TheMatrix;
}Object3D;

typedef	struct
{
	
}Object2D;


typedef	struct
{
	std::uint8_t				DrawType;
	KeyFrameElement		*AnimElements,
						*NextAnimElements;
	union
	{
		Object3D		
		Object	
	}DrawType;
}Draw;
*/

typedef	struct
{
	std::uint32_t		DrawType;
}Draw;

//---------------------------------------------------------------

typedef	struct
{
	std::int8_t		Class,
				State;
	std::uint32_t		Flags;
	std::int32_t		Child,
				Parent;
	std::int32_t		LinkChild,
				LinkParent;

	Draw		Draw;
}Thing;

//---------------------------------------------------------------

typedef	struct
{
	float			Altitude;
	void			*MapWho;

}MapElement;

//---------------------------------------------------------------

