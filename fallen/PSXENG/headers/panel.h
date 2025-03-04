/*
** File: Panel.h
**
** Author: James Watson
**
** Copyright: 1999 Mucky Foot Productions Ltd
*/

#pragma once

#include "MFStdLib.h"
#include "psxeng.h"

void PANEL_draw_quad(std::int32_t left,std::int32_t top,std::int32_t width,std::int32_t height,std::int32_t page,std::uint32_t colour,std::uint8_t u1,std::uint8_t v1,std::uint8_t u2,std::uint8_t v2);
void PANEL_funky_quad(std::int32_t which,std::int32_t x,std::int32_t y,std::uint32_t colour);
void PANEL_new_funky();
void PANEL_inventory(Thing *darci) ;
void PANEL_new_text(Thing *who, std::int32_t delay, char* fmt, ...);
void PANEL_new_text_process();
void PANEL_new_help_message(char* fmt, ...);
void PANEL_new_text_draw();
void PANEL_draw_beacons();
void PANEL_new_widescreen();
void PANEL_render_thugs();
void PANEL_DrawSign(std::int32_t x,std::int32_t y,std::int32_t type);
void PANEL_flash_sign(std::int32_t sign, std::int32_t flip);
void PANEL_new_info_message(char* fmt, ...);
void PANEL_new_face(Thing *who,std::int32_t x,std::int32_t y);
void PANEL_draw_search(std::int32_t timer);
void PANEL_draw_eog(std::int32_t win);
void PANEL_new_text_init();
void PANEL_new_widescreen_init();
void PANEL_draw_local_health(std::int32_t mx,std::int32_t my,std::int32_t mz,std::int32_t percentage,std::int32_t radius = 60);


#define PANEL_SIGN_WHICH_UTURN                0
#define PANEL_SIGN_WHICH_TURN_RIGHT           1
#define PANEL_SIGN_WHICH_DONT_TAKE_RIGHT_TURN 2
#define PANEL_SIGN_WHICH_STOP                 3

#define PANEL_SIGN_FLIP_LEFT_AND_RIGHT (1 << 0)
#define PANEL_SIGN_FLIP_TOP_AND_BOTTOM (1 << 1)

