#pragma once

//
// Turn off the warning about [] arrays at the end of structures.
//

#pragma warning(disable : 4200)

//
// Standard libraries most often used.
//

#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdint>

//
// Constants.
//

#define PI (3.14159265F)
#undef INFINITY
#define INFINITY (0x7fffffff)

//
// Useful defines.
//

#define MAX3(mnx, mny, mnz) std::max({mnx, mny, mnz})
#define MIN3(mnx, mny, mnz) std::min({mnx, mny, mnz})
#define MIN4(a, b, c, d) std::min({a, b, c, d})
#define MAX4(a, b, c, d) std::max({a, b, c, d})
#define SWAP(x, y)               \
    {                            \
        std::int32_t SWAP_spare; \
        SWAP_spare = x;          \
        x = y;                   \
        y = SWAP_spare;          \
    }
#define SWAP_UB(x, y)            \
    {                            \
        std::uint8_t SWAP_spare; \
        SWAP_spare = x;          \
        x = y;                   \
        y = SWAP_spare;          \
    }
#define SWAP_UW(x, y)             \
    {                             \
        std::uint16_t SWAP_spare; \
        SWAP_spare = x;           \
        x = y;                    \
        y = SWAP_spare;           \
    }
#define SWAP_FL(x, y)     \
    {                     \
        float SWAP_spare; \
        SWAP_spare = x;   \
        x = y;            \
        y = SWAP_spare;   \
    }
#define SWAP_DB(x, y)      \
    {                      \
        double SWAP_spare; \
        SWAP_spare = x;    \
        x = y;             \
        y = SWAP_spare;    \
    }
#define SWAP_HF(x, y)       \
    {                       \
        Pointhf SWAP_spare; \
        SWAP_spare = x;     \
        x = y;              \
        y = SWAP_spare;     \
    }
#define SWAP_P3(x, y)       \
    {                       \
        Point3d SWAP_spare; \
        SWAP_spare = x;     \
        x = y;              \
        y = SWAP_spare;     \
    }
#define SWAP_VD(x, y)     \
    {                     \
        void *SWAP_spare; \
        SWAP_spare = x;   \
        x = y;            \
        y = SWAP_spare;   \
    }
#define SWAP_UWP(x, y)             \
    {                              \
        std::uint16_t *SWAP_spare; \
        SWAP_spare = x;            \
        x = y;                     \
        y = SWAP_spare;            \
    }
#define SWAP_UBP(x, y)            \
    {                             \
        std::uint8_t *SWAP_spare; \
        SWAP_spare = x;           \
        x = y;                    \
        y = SWAP_spare;           \
    }
#define WITHIN(x, a, b) ((x) >= (a) && (x) <= (b))
#define SATURATE(x, a, b)         \
    {                             \
        if ((x) < (a)) (x) = (a); \
        if ((x) > (b)) (x) = (b); \
    }
#define SIGN(x) (((x)) ? (((x) > 0) ? 1 : -1) : 0)
#define QLEN(x, y) (((x) > (y)) ? (x) + ((y) >> 1) : (y) + ((x) >> 1))
#define XOR(a, b) ((!(a) && (b)) || ((a) && !(b)))
#define SQRT16(x) ((sqrt((double) (x)) * 256))
// #define SHOW(a)			{TRACE(#a" = %d\n", (a));}
// #define SHOW_X(a)			{TRACE(#a" = 0x%x\n", (a));}
// #define SHOW_FL(a)		{TRACE(#a" = %f\n", (a));}

//
// Error checking macros
//

#ifndef NDEBUG

void OS_string(const char *fmt, ...);

#define ASSERT(x)  \
    {              \
        assert(x); \
    }
#define TRACE OS_string
#define VERIFY(x) ASSERT(x)
#else
#define ASSERT(x)    \
    {                \
        __assume(x); \
    }
#define TRACE
#define VERIFY(x) x

//
// In release build, get rid of the x == 1; warning message otherwise
// VERIFY generates it all the time!
//

#pragma warning(disable : 4553)
#endif

static inline int ftol(float f) {
    return (int) f;
}

// Globally useful structures.
//

struct Point3d {
    float x, y, z;
};
struct Point2d {
    float x, y;
};
struct Pointhf {
    float x, z;
};
struct Pointuv {
    float u, v;
};

struct Direction {
    float yaw;
    float pitch;
    float roll;
};

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

//
// Fast approximation to sqrt(x*x + y*y)
//

static inline float qdist2(float x, float y) {
    float ans;

    ASSERT(x >= 0.0F);
    ASSERT(y >= 0.0F);

    if (x > y) {
        ans = x + y * 0.5F;
    } else {
        ans = y + x * 0.5F;
    }

    return ans;
}

//
// Fast approximation to sqrt(x*x + y*y + z*z), x,y and z must
// be positive.
//

static inline float qdist3(float x, float y, float z) {
    float ans;

    ASSERT(x >= 0.0F);
    ASSERT(y >= 0.0F);
    ASSERT(z >= 0.0F);

    if (x > y) {
        if (x > z) {
            //
            // x is the biggeset.
            //

            ans = x + (y + z) * 0.2941F;

            return ans;
        }
    } else {
        if (y > z) {
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

static inline float frand() {
    std::int32_t irand = rand();
    float ans = float(irand) * (1.0F / float(RAND_MAX));

    return ans;
}