#ifndef _ALWAYS_
#define _ALWAYS_

//
// Turn off the warning about [] arrays at the end of structures.
//

#pragma warning( disable : 4200 )


typedef signed		long			std::int32_t;
typedef unsigned	long			std::uint32_t;
typedef signed		short	int		std::int16_t;
typedef unsigned	short	int		std::uint16_t;
typedef signed		char   			std::int8_t;
typedef unsigned 	char			std::uint8_t;
typedef char			char;


//
// Standard libraries most often used.
//

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//
// Constants.
//

#define PI         (3.14159265F)
#define ROOT2      (1.41421356F)
#define INFINITY   (0x7fffffff)


//
// Useful defines.
//

#define MIN(mnx,mny)  		(((mnx) < (mny)) ? (mnx) : (mny))
#define MAX(mnx,mny)  		(((mnx) > (mny)) ? (mnx) : (mny))
#define MAX3(mnx,mny,mnz)	(((mnx) > (mny)) ? MAX(mnx,mnz) : MAX(mny,mnz))
#define MIN3(mnx,mny,mnz)	(((mnx) < (mny)) ? MIN(mnx,mnz) : MIN(mny,mnz))
#define MIN4(a,b,c,d)		(MIN(MIN(a,b), MIN(c,d)))
#define MAX4(a,b,c,d)		(MAX(MAX(a,b), MAX(c,d)))
#define SWAP(x,y)	   		{std::int32_t   SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_UB(x,y)   		{std::uint8_t   SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_UW(x,y)   		{std::uint16_t   SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_FL(x,y)   		{float   SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_DB(x,y)   		{double  SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_HF(x,y)   		{Pointhf SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_P3(x,y)		{Point3d SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_VD(x,y)		{void   *SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_UWP(x,y)		{std::uint16_t  *SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define SWAP_UBP(x,y)		{std::uint8_t  *SWAP_spare; SWAP_spare = x; x = y; y = SWAP_spare;}
#define WITHIN(x,a,b)  		((x) >= (a) && (x) <= (b))
#define SATURATE(x,a,b)		{if ((x) < (a)) (x) = (a); if ((x) > (b)) (x) = (b);}
#define SIGN(x)		   		(((x)) ? (((x) > 0) ? 1 : -1) : 0)
#define QLEN(x,y) 	   		(((x) > (y)) ? (x) + ((y) >> 1) : (y) + ((x) >> 1))
#define XOR(a,b)  	   		((!(a) && (b)) || ((a) && !(b)))
#define SQRT16(x)  			((sqrt((double) (x)) * 256))
#define SHOW(a)				{TRACE(#a" = %d\n", (a));}
#define SHOW_X(a)			{TRACE(#a" = 0x%x\n", (a));}
#define SHOW_FL(a)			{TRACE(#a" = %f\n", (a));}

//
// Error checking macros
// 

#ifndef NDEBUG

void OS_string(char* fmt, ...);

#define ASSERT(x) {assert(x);}
#define TRACE	  OS_string
#define VERIFY(x) ASSERT(x)
#else
#define ASSERT(x) {__assume(x);}
#define TRACE	  
#define VERIFY(x) x

//
// In release build, get rid of the x == 1; warning message otherwise
// VERIFY generates it all the time!
//

#pragma warning( disable : 4553 )
#endif


//
// Assembler fixed point thingies from T.A.C.
//

static inline std::int32_t DIV16(std::int32_t a, std::int32_t b)
{
	std::int32_t v;
	_asm
	{
		mov		edx,a
		mov		ebx,b
		mov		eax,edx
		shl		eax,16
		sar		edx,16
		idiv	ebx
		mov		v,eax
	}
	return v;
}

#pragma warning( disable : 4035 )	
// stop warning of no return value : eax is valid
static inline std::int32_t MUL16(std::int32_t a, std::int32_t b)
// MSVC++ version, params:ecx,edx, return:eax
// this is as fast on 486/Pentium as the old version
// and 2.5* faster on Pentium Pro
{
	__asm
	{
		mov		eax, a
		imul	b				// result in edx:eax
		shl		edx,16
		shr		eax,16
		or		eax,edx
		// return in eax
	}
}
#pragma warning( default : 4035 )


//
// Sets the precision of the floating point unit.
//

#define FPP_SINGLE   0x00		// 32-bits
#define FPP_DOUBLE   0x20		// 53-bits
#define FPP_EXTENDED 0x30		// 64-bits

static inline void fpp_set(std::uint16_t precision)
{
	short control;

	__asm
	{
		wait
		fnstcw	control
		wait
		mov		ax, control
		and     ax, 0xfcff
		or		ax, precision
		mov		control, ax
		fldcw	control
	}
}



//
// For doing fast floating-point to long conversion...
//

//
// Sets the rounding to truncate automatically.
//

static inline void ftol_init()
{
	short control;

	__asm
	{
		wait
		fnstcw	control
		wait
		mov		ax, control
		or		ah, 0Ch
		mov		control, ax
		fldcw	control
	}
}

//
// Converts a float to an int using the current rounding. For normal
// C-style rounding, make sure you call ftol_init() at the start
// of your program.
//

static inline int ftol(float f)
{
	int ans;

	__asm
	{
		fld		f
		fistp	ans
	}

	return ans;
}



//
// Finds the sign of the cross product x1y2 - x2y1 also from T.A.C.
//

static inline std::int8_t CROSS_PROD_SIGN(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2)
{
	std::int8_t ans;

	__asm
	{
		mov		eax, x1
		mov		ebx, y1
		mov		ecx, x2
		mov		edx, y2

		imul	edx		
		xchg	eax,ebx
		xchg	edx,ecx
		imul	edx		
		sub		ebx,eax
		setnz	al		
		sbb		ecx,edx
		jz		done	
		setl	ah		
		setg	al		
		sub		al,ah	
	  done:

		mov		ans, al
	}

	return ans;	
}


//
// Returns the number of the first bit set in the given value.  Returns
// -1 if there is no bit set.
//

static inline first_bit(std::int32_t search)
{
	std::int32_t ans;

	__asm
	{
		mov		eax, search
		mov		ebx, 0ffffffffh
		bsf		ebx, eax
		mov		ans, ebx
	}

	return ans;
}


//
// Returns the number of the last bit set in the given value.  Returns
// -1 if there is no bit set.
//

static inline last_bit(std::int32_t search)
{
	std::int32_t ans;

	__asm
	{
		mov		eax, search
		mov		ebx, 0ffffffffh
		bsr		ebx, eax
		mov		ans, ebx
	}

	return ans;
}


//
// Find the cross product.
//

#define CROSS_PROD(x1,y1,x2,y2) (((x1)*(y2)) - ((x2)*(y1)))


//
// Globally useful structures.
//

typedef struct {float x, y, z;} Point3d;
typedef struct {float x, y;   } Point2d;
typedef struct {float x, z;   } Pointhf;
typedef struct {float u, v;   } Pointuv;

typedef struct
{
	float yaw;
	float pitch;
	float roll;

} Direction;

typedef struct
{
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	
} Color;

//
// Fast approximation to sqrt(x*x + y*y)
//

static inline float qdist2(float x, float y)
{
	float ans;

	ASSERT(x >= 0.0F);
	ASSERT(y >= 0.0F);

	if (x > y)
	{
		ans = x + y * 0.5F;
	}
	else
	{
		ans = y + x * 0.5F;
	}

	return ans;
}


//
// Fast approximation to sqrt(x*x + y*y + z*z), x,y and z must
// be positive.
//

static inline float qdist3(float x, float y, float z)
{
	float ans;

	ASSERT(x >= 0.0F);
	ASSERT(y >= 0.0F);
	ASSERT(z >= 0.0F);

	if (x > y)
	{
		if (x > z)
		{
			//
			// x is the biggeset.
			//

			ans = x + (y + z) * 0.2941F;

			return ans;
		}
	}
	else
	{
		if (y > z)
		{
			//
			// y is the biggeset.
			//

			ans = y + (x + z) * 0.2941F;

			return ans;
		}
	}

	//
	// z is the biggeset.
	//

	ans = z + (x + y) * 0.2941F;

	return ans;
}


//
// A floating point number between 0 and 1.0F
// 

static inline float frand()
{
	std::int32_t irand = rand();
	float ans   = float(irand) * (1.0F / float(RAND_MAX));

	return ans;
}



#endif
