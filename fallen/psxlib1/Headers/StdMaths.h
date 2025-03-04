// StdMaths.h
// Guy Simmons, 18th December 1997

#ifndef	STD_MATHS_H
#define	STD_MATHS_H

#include	"libgte.h"

//---------------------------------------------------------------

#define	SIN(a)				(rsin((a)<<1)<<4)
#define	COS(a)				(rcos((a)<<1)<<4)

#define	SIN_F(a)			SinTableF[a]
#define	COS_F(a)			CosTableF[a]

#define	PROPTABLE_SIZE		256
#define	PROP(x)				Proportions[(x)+PROPTABLE_SIZE]

extern float				*CosTableF,
							SinTableF[];
extern std::int16_t				AtanTable[];
extern std::int32_t				*CosTable,
							SinTable[];
extern std::int32_t				Proportions[];

//std::int32_t						Arctan(std::int32_t X,std::int32_t Y);
#define Arctan(y,x)			((2048+(1024-(ratan2(y,x)>>1)))&2047)
//std::int32_t						Root(std::int32_t square);

static inline std::int32_t			Hypotenuse(std::int32_t x,std::int32_t y)	
{											
	x	=	abs(x);
	y	=	abs(y);
	if(x>y)
		return((PROP((y<<8)/x)*x)>>13);
	else
		if(y)
			return((PROP((x<<8)/y)*y)>>13);
		else
			return(0);
}


//---------------------------------------------------------------

#endif
