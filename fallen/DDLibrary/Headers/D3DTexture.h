// D3DTexture.h
// Guy Simmons, 29th November 1997.

#ifndef	D3DTEXTURE_H
#define	D3DTEXTURE_H


// Call after doing lots of loading.
void NotGoingToLoadTexturesForAWhileNowSoYouCanCleanUpABit ( void );

// handle = file opened with FileOpen
// dwSize = number of bytes to load.
// return = where the file was loaded.
// You _must_ copy this or do something with it before loading another file.
void* FastLoadFileSomewhere ( MFFileHandle handle, DWORD dwSize );

#include	"tga.h"
#include	"FileClump.h"

struct	Char
{
	std::int32_t		X,
				Y,
				Height,
				Width;
};

struct	Font
{
	std::int32_t			StartLine;
	Char			CharSet[96];
	Font			*NextFont;
};

#define	D3D_TEXTURE_FONT		(1<<0)
#define D3D_TEXTURE_FONT2		(1<<1)	// texture is a font with red borders

#define D3DTEXTURE_TYPE_UNUSED	0
#define D3DTEXTURE_TYPE_TGA		1
#define D3DTEXTURE_TYPE_USER	2		// The user updates the contents- it doesn't have a file.


// First number is size of subtextures, second is number of subtextures in page.
// 3x3 means the page is 4x as big in each direction, with lots of pixel padding.
#define D3DPAGE_NONE			0
#define D3DPAGE_64_3X3			1
#define D3DPAGE_64_4X4			2
#define D3DPAGE_32_3X3			3
#define D3DPAGE_32_4X4			4

class	D3DTexture
{
	public:

		char					texture_name[256];
		std::uint32_t					ID;		// texture ID for FileClump
		// Allow the texture to be shrunk or replaced with junk for faster loading.
		bool					bCanShrink;
		std::int32_t					TextureFlags;
		Font					*FontList;
		LPDIRECT3DTEXTURE2		lp_Texture;
		LPDIRECTDRAWSURFACE4	lp_Surface;
		HRESULT					Reload_TGA ();
		HRESULT					Reload_user();
		bool					GreyScale;
		bool					UserWantsAlpha;	// The user page needs an alpha-channel.
#ifdef TEX_EMBED
		//char *name;			// Texture file name.
		std::uint8_t bPagePos;			// Position in page.
		std::uint8_t bPageType;		// One of D3DPAGE_xxx
		WORD wPageNum;			// The D3Dpage this is in.
#endif

		D3DTexture				*NextTexture;


		D3DTexture()							{
													TextureFlags	=	0;
													FontList		=	nullptr;
													NextTexture=nullptr;
													#ifndef TARGET_DC
													#endif
													lp_Surface=nullptr;
													lp_Texture=nullptr;
													Type = D3DTEXTURE_TYPE_UNUSED;
#ifdef TEX_EMBED
													wPageNum = -1;	// None.
													bPagePos = 0;
													bPageType = D3DPAGE_NONE;
#endif
													// IT WOULD FUCKING HELP IF SOME OF THESE ACTUALLY GOT SET UP WITH DEFAULTS
													GreyScale = false;
													UserWantsAlpha = false;
													ID = -1;
													bCanShrink = false;
												}

		//
		// The format used.
		// 

		std::int32_t mask_red=0;
		std::int32_t mask_green=0;
		std::int32_t mask_blue = 0;
		std::int32_t mask_alpha = 0;

		std::int32_t shift_red = 0;
		std::int32_t shift_green = 0;
		std::int32_t shift_blue = 0;
		std::int32_t shift_alpha = 0;

		std::int32_t		Type;
		std::int32_t       size;			// The size in pixels of the texture page.
		std::int32_t       ContainsAlpha;

		HRESULT		LoadTextureTGA(char* tga_file,std::uint32_t texid,bool bCanShrink=true);

		HRESULT		ChangeTextureTGA(char* tga_file);

		HRESULT		CreateUserPage(std::int32_t size, bool i_want_an_alpha_channel);	// Power of two between 32 and 256 inclusive
		HRESULT     LockUser      (std::uint16_t **bitmap, std::int32_t *pitch);				// Returns the texture page on success. The pitch is in bytes!
		void        UnlockUser	  ();

		HRESULT		Reload();
		HRESULT		Destroy();

		HRESULT		CreateFonts(TGA_Info *tga_info,TGA_Pixel *tga_data);
		Font		*GetFont(std::int32_t id);

		// resets texture page for loading
		static void	BeginLoading();

		//
		// Makes the texture grey-scale. If it is already loaded, it
		// is re-loaded.
		//

		void set_greyscale(bool is_greyscale);

		LPDIRECT3DTEXTURE2		GetD3DTexture()			{ return lp_Texture; }
		LPDIRECTDRAWSURFACE4	GetSurface()		{ return lp_Surface; }
#ifdef TEX_EMBED
		void					GetTexOffsetAndScale ( float *pfUScale, float *pfUOffset, float *pfVScale, float *pfVOffset );
#endif

		HRESULT					SetColorKey(std::int32_t flags,LPDDCOLORKEY key)	{
																					if (lp_Surface)
																					{
																						return	lp_Surface->SetColorKey(flags,key);
																					}
																					else
																					{
																						return DDERR_GENERIC;
																					}
																				}

		inline	bool		IsFont()				{	return	TextureFlags&D3D_TEXTURE_FONT;	}
		inline	void		FontOn()				{	TextureFlags	|=	D3D_TEXTURE_FONT;	}
		inline	void		FontOff()				{	TextureFlags	&=	~D3D_TEXTURE_FONT;	}

		inline	bool		IsFont2()				{	return	TextureFlags&D3D_TEXTURE_FONT2;	}
		inline	void		Font2On()				{	TextureFlags	|=	D3D_TEXTURE_FONT2;	}
		inline	void		Font2Off()				{	TextureFlags	&=	~D3D_TEXTURE_FONT2;	}
};



class D3DPage
{
public:
	std::uint8_t		bPageType;			// One of D3DPAGE_xxx
	std::uint8_t		bNumTextures;		// Number of textures in page.
	char		*pcDirectory;		// The page's directory. THIS IS STATIC - don't free it.
	char		*pcFilename;		// The page's filename. THIS IS STATIC - don't free it.
	char		**ppcTextureList;	// A pointer to an array of pointers to strings of the texture names :-)
	D3DTexture	*pTex;				// The texture page texture itself.
	//D3DTexture	*pTextures[16];		// The textures in this page, in order.

	D3DPage ( void )
	{
		bPageType = 0;
		bNumTextures = 0;
		pcDirectory = nullptr;
		pcFilename = nullptr;
		pTex = nullptr;
		ppcTextureList = nullptr;
	}

	// Call this when linking a standard D3DTexture to the page - it will demand-load the page's texture.
	void D3DPage::EnsureLoaded ( void );
	// Call this when unloading everything.
	void D3DPage::Unload ( void );

};

#endif


