//	Text.h
//	Guy Simmons, 17th May 1998.

#ifndef	TEXT_H
#define	TEXT_H

//---------------------------------------------------------------
extern bool text_fudge;
extern std::uint32_t text_colour;


std::int32_t	text_width(char* message,std::int32_t font_id,std::int32_t *char_count);
std::int32_t	text_height(char* message,std::int32_t font_id,std::int32_t *char_count);
void	draw_text_at(float x,float y,char* message,std::int32_t font_id);
void	draw_centre_text_at(float x,float y,char* message,std::int32_t font_id,std::int32_t flag);
void	show_text();

//---------------------------------------------------------------

#endif

