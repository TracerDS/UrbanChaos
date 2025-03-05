// TextDef.c
// Guy Simmons, 7th October 1996.

// Hard coded font definition for quick text functions.

#include "Editor.hpp"

//---------------------------------------------------------------
// Byte1	-	Width
// Byte2	-	Height
// Byte3	-	Vertical offset.

//---------------------------------------------------------------

static std::uint8_t Null[] =
    {
        0,
        0,
        0,
};

static std::uint8_t Space[] =
    {
        4,
        0,
        0,
};

static std::uint8_t Excl[] =
    {
        1,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
};

static std::uint8_t Dollar[] =
    {
        5,
        9,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t Percent[] =
    {
        8,
        7,
        1,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
};

static std::uint8_t And[] =
    {
        7,
        8,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t Quotes[] =
    {
        3,
        2,
        1,
        1,
        0,
        1,
        1,
        0,
        1,
};

static std::uint8_t Astrisk[] =
    {
        5,
        5,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
};

static std::uint8_t BrackO[] =
    {
        3,
        9,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t BrackC[] =
    {
        3,
        9,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
};

static std::uint8_t Plus[] =
    {
        5,
        5,
        2,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t Comma[] =
    {
        2,
        4,
        6,
        1,
        1,
        1,
        1,
        0,
        1,
        1,
        0,
};

static std::uint8_t Minus[] =
    {
        5, 1, 4,
        1, 1, 1, 1, 1};

static std::uint8_t Stop[] =
    {
        2,
        2,
        6,
        1,
        1,
        1,
        1,
};

static std::uint8_t Zero[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t One[] =
    {
        2,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
};

static std::uint8_t Two[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
};

static std::uint8_t Three[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t Four[] =
    {
        5,
        7,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
};

static std::uint8_t Five[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t Six[] =
    {
        5,
        7,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t Seven[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t Eight[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t Nine[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
};

static std::uint8_t Colon[] =
    {
        2,
        3,
        4,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t QMark[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t At[] =
    {
        7,
        8,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
};

static std::uint8_t A[] =
    {
        5, 7, 1,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 1, 0, 1, 0,
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1};

static std::uint8_t B[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
};

static std::uint8_t C[] =
    {
        5, 7, 1,
        0, 1, 1, 1, 0,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 0,
        1, 0, 0, 0, 1,
        0, 1, 1, 1, 0};

static std::uint8_t D[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
};

static std::uint8_t E[] =
    {
        4,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
};

static std::uint8_t F[] =
    {
        4,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
};

static std::uint8_t G[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t H[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t I[] =
    {
        1,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
};

static std::uint8_t J[] =
    {
        5,
        7,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t K[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t L[] =
    {
        4,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
};

static std::uint8_t M[] =
    {
        7,
        7,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
};

static std::uint8_t N[] =
    {
        5,
        7,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t O[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t P[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
};

static std::uint8_t Q[] =
    {
        5,
        8,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
};

static std::uint8_t R[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t S[] =
    {
        5,
        7,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t T[] =
    {
        5,
        7,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t U[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t V[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t W[] =
    {
        7,
        7,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
};

static std::uint8_t X[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t Y[] =
    {
        5,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t Z[] =
    {
        4,
        7,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
};

static std::uint8_t Under[] =
    {
        6, 1, 7,
        1, 1, 1, 1, 1};

static std::uint8_t _a[] =
    {
        4,
        5,
        3,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
};

static std::uint8_t _b[] =
    {
        4,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        1,
        0,
};

static std::uint8_t _c[] =
    {
        4,
        5,
        3,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
};

static std::uint8_t _d[] =
    {
        4,
        7,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
};

static std::uint8_t _e[] =
    {
        4,
        5,
        3,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
};

static std::uint8_t _f[] =
    {
        4,
        7,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t _g[] =
    {
        4,
        7,
        3,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
};

static std::uint8_t _h[] =
    {
        4,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
};

static std::uint8_t _i[] =
    {
        2,
        7,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
};

static std::uint8_t _j[] =
    {
        3,
        9,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        1,
        0,
};

static std::uint8_t _k[] =
    {
        4,
        7,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
};

static std::uint8_t _l[] =
    {
        2,
        7,
        1,
        1,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
};

static std::uint8_t _m[] =
    {
        7,
        5,
        3,
        1,
        1,
        1,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
};

static std::uint8_t _n[] =
    {
        4,
        5,
        3,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
};

static std::uint8_t _o[] =
    {
        4,
        5,
        3,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
};

static std::uint8_t _p[] =
    {
        4,
        7,
        3,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
};

static std::uint8_t _q[] =
    {
        4,
        7,
        3,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t _r[] =
    {
        4,
        5,
        3,
        1,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
};

static std::uint8_t _s[] =
    {
        4,
        5,
        3,
        0,
        1,
        1,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        0,
};

static std::uint8_t _t[] =
    {
        3,
        7,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t _u[] =
    {
        4,
        5,
        3,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
};

static std::uint8_t _v[] =
    {
        5,
        5,
        3,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
};

static std::uint8_t _w[] =
    {
        7,
        5,
        3,
        1,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        0,
};

static std::uint8_t _x[] =
    {
        5,
        5,
        3,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        0,
        1,
        0,
        0,
        0,
        1,
};

static std::uint8_t _y[] =
    {
        4,
        7,
        3,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        1,
        0,
        1,
        1,
        1,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
};

static std::uint8_t _z[] =
    {
        4,
        5,
        3,
        1,
        1,
        1,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        1,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
};

//---------------------------------------------------------------

std::uint8_t *CharTable[] =
    {
        Null, Null, Null, Null, Null, Null, Null, Null,
        Null, Null, Null, Null, Null, Null, Null, Null,
        Null, Null, Null, Null, Null, Null, Null, Null,
        Null, Null, Null, Null, nullptr, Null, Null, Null,
        Space, Excl, Quotes, Null, Dollar, Percent, And, Null,
        BrackO, BrackC, Astrisk, Plus, Comma, Minus, Stop, Null,
        Zero, One, Two, Three, Four, Five, Six, Seven,
        Eight, Nine, Colon, Null, Null, Null, Null, QMark,
        At, A, B, C, D, E, F, G,
        H, I, J, K, L, M, N, O,
        P, Q, R, S, T, U, V, W,
        X, Y, Z, Null, Null, Null, Null, Under,
        Null, _a, _b, _c, _d, _e, _f, _g,
        _h, _i, _j, _k, _l, _m, _n, _o,
        _p, _q, _r, _s, _t, _u, _v, _w,
        _x, _y, _z, Null, Null, Null, Null, Null};

//---------------------------------------------------------------
