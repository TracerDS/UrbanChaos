#ifndef INLINE_H
#define INLINE_H

#if !defined(TARGET_DC)

#ifndef PSX

#ifdef __WINDOWS_386__
std::int32_t MUL64(std::int32_t, std::int32_t);
#pragma aux MUL64 =  \
    "	imul	ebx				"  \
    "	mov		ax,dx			" \
    "	rol		eax,16			" parm[eax][ebx] modify[eax ebx edx] value[eax]

#pragma aux DIV64 =               \
    "   mov     eax,edx         " \
    "   shl     eax,16          " \
    "   sar     edx,16          " \
    "   idiv    ebx             " parm[edx][ebx] modify[eax ebx edx] value[eax]

#else

inline std::int32_t DIV64(std::int32_t a, std::int32_t b) {
    /* 64bit divide */
    std::int32_t ret_v;
    __asm
    {                       
		 mov	edx,a
		 mov	ebx,b
		 mov	eax,edx
		 shl	eax,16			
		 sar	edx,16			
		 idiv	ebx
		 mov	ret_v,eax
    }
    return (ret_v);
}

inline std::int32_t MUL64(std::int32_t a, std::int32_t b) {
    /* 64bit multiply */
    std::int32_t ret_v;
    __asm
    {                       
		 mov	eax,a       
		 imul   b			
		 mov	ax,dx       
		 rol	eax,16
		 mov	ret_v,eax
    }
    return (ret_v);
}
#endif
#else

// psx bit

inline std::int32_t DIV64(std::int32_t a, std::int32_t b) {
    long long ina;
    std::int32_t out;
    ina = a;
    ina <<= 16;
    return ((std::int32_t) (ina / b));
}
/*
inline std::int32_t	DIV64(std::int32_t a,std::int32_t b)
{
        return(a/b);
}
*/

//
// was an inline but fucks up for some reason, so has been made into a function in collide.cpp
//
extern std::int32_t MUL64(std::int32_t i, std::int32_t j);

/*
inline std::int32_t	MUL64(std::int32_t a,std::int32_t b)
{
        return(a*b);
}
  */
#endif

#else // #if !defined (TARGET_DC)

inline std::int32_t MUL64(std::int32_t a, std::int32_t b);

// Dreamcast versions. Might be inlined later, just C for now.
// Also very approximate - DC compiler doesn't like long long, so trash the accuracy.
inline std::int32_t DIV64(std::int32_t a, std::int32_t b) {
    // return ( ( a << 16 )  / b );
    std::int32_t slResult;

    if (a < 0x8000) {
        // The most common case.
        slResult = ((a << 16) / b);
    } else if (a < 0x80000) {
        slResult = ((a << 12) / (b >> 4));
    } else if (a < 0x800000) {
        slResult = ((a << 8) / (b >> 8));
    } else {
        // Hasn't been used yet.
        ASSERT(false);
        slResult = (a / (b >> 16));
    }

#ifdef DEBUG
    std::int32_t slTemp = MUL64(slResult, b);
    slTemp -= a;
    ASSERT((slTemp > -200) && (slTemp < 200));
#endif

    return slResult;
}

inline std::int32_t MUL64(std::int32_t a, std::int32_t b) {
    // Typically used for multiplying two 16.16 values in the range 0.0-1.0 together.
    // return ( ( a * b ) >> 16 );
    if ((a <= 0x18000) && (a >= -0x18000) && (b <= 0x18000) && (b >= -0x18000)) {
        return (((a >> 1) * (b >> 1)) >> 14);
    }

    int iCount = 16;
    while ((a <= -0x8000) || (a >= 0x8000)) {
        a >>= 1;
        iCount--;
    }
    while ((b <= -0x8000) || (b >= 0x8000)) {
        b >>= 1;
        iCount--;
    }
    return ((a * b) >> iCount);
}

#endif // #else //#if !defined (TARGET_DC)

#endif