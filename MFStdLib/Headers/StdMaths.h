// StdMaths.h
// Guy Simmons, 18th December 1997

#pragma once

//---------------------------------------------------------------

#define	SIN(a)				SinTable[a]
#define	COS(a)				CosTable[a]

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

std::int32_t Arctan(std::int32_t X,std::int32_t Y);
std::int64_t Root(std::int64_t square);

static inline std::int32_t Hypotenuse(std::int32_t x, std::int32_t y)	
{											
	x = abs(x);
	y = abs(y);
	if (x > y)
		return((PROP((y<<8)/x)*x)>>13);
		
	if (y)
		return((PROP((x<<8)/y)*y)>>13);
	else
		return 0;
}