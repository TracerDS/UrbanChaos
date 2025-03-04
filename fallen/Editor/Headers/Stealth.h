// Stealth.h
// Guy Simmons, 21st February 1997.

#ifndef	_STEALTH_H_
#define	_STEALTH_H_

#define	TEXTURE_WIDTH		256
#define	TEXTURE_HEIGHT		256
#define	TEXTURE_PAGE_SIZE	(TEXTURE_WIDTH*TEXTURE_HEIGHT*2)


typedef	struct
{
	std::uint16_t		*TexturePtr;
	std::uint8_t		*PalPtr;

}GameTexture;

#define NUM_GAME_TEXTURES 50

extern GameTexture		game_textures[NUM_GAME_TEXTURES];

typedef	struct
{
	
}EdTriangle;

typedef	struct
{
	
}EdQuad;

typedef	struct
{
	std::int32_t	U[4],
			V[4];
}EdTexture;


#endif
