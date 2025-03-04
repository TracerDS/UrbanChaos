#include "psxeng.h"

#ifndef _KANJI_H
#define _KANJI_H
#ifdef VERSION_KANJI

extern void Kanji_Init(std::uint16_t x,std::uint16_t y);

extern void Kanji_string(std::int32_t x,std::int32_t y,std::uint16_t *str,std::int32_t col,std::int32_t scale);

extern void Kanji_Debug();

#endif
#endif