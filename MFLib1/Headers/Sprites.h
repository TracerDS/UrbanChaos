// Sprites.h
// Guy Simmons, 13th February 1997.

#pragma once

#ifndef	_MF_TYPES_H_
	#include	<MFTypes.h>
#endif

#define	END_LINE				0x00
#define	COPY_PIXELS				0x01
#define	SKIP_PIXELS				0x02
#define	DUPLICATE_PIXELS		0x03
#define	FINISHED				0x04


typedef struct
{
	std::uint8_t		*SpriteData;
	std::uint16_t		SpriteHeight;
	std::uint16_t		SpriteWidth;
}BSprite;


extern void	(*DrawBSprite)(std::int32_t x,std::int32_t y,BSprite *the_sprite);
extern void	(*DrawBSpriteC)(std::int32_t x,std::int32_t y,BSprite *the_sprite);

extern void	DrawBSpritePal16(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint8_t *pal);
extern void	DrawBSpritePal32(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint8_t *pal);

extern void	(*DrawMonoBSprite)(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint32_t colour);
extern void	(*DrawMonoBSpriteC)(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint32_t colour);


extern void	DrawBSpritePalC16(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint8_t *pal);
extern void	DrawBSpritePalC32(std::int32_t x,std::int32_t y,BSprite *the_sprite,std::uint8_t *pal);

extern void	SetupBSprites(BSprite *sprite_ref,std::uint8_t *sprite_data);

