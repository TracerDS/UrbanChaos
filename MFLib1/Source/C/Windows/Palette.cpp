// Palette.cpp
// Guy Simmons, 11th February 1997.


#include	<MFHeader.h>

std::uint8_t CurrentPalette[256*3];
LPDIRECTDRAWPALETTE lp_DD_Palette	=	nullptr;
PALETTEENTRY ThePalette[256];

//---------------------------------------------------------------


void InitPalettes()
{
#if defined(_DEBUG) && !defined(TARGET_DC)
	std::uint32_t			c0;


	// First set up the Windows static entries.
	for(c0=0;c0<10;c0++)
	{
	    // The first 10 static entries:
	    ThePalette[c0].peFlags	=	PC_EXPLICIT;
	    ThePalette[c0].peRed	=	(std::uint8_t)c0;
	    ThePalette[c0].peGreen	=	0;
	    ThePalette[c0].peBlue	=	0;

	    // The last 10 static entries:
	    ThePalette[c0+246].peFlags	=	PC_EXPLICIT;
	    ThePalette[c0+246].peRed	=	(std::uint8_t)(c0+246);
	    ThePalette[c0+246].peGreen	=	0;
	    ThePalette[c0+246].peBlue	=	0;
	}

	// Set up private entries.  Initialise to black.
	for(c0=10;c0<246;c0++)
	{
	    ThePalette[c0].peFlags	=	PC_NOCOLLAPSE|PC_RESERVED;
	    ThePalette[c0].peRed	=	0;
	    ThePalette[c0].peGreen	=	0;
	    ThePalette[c0].peBlue	=	0;
	}
#else
	// Initialise palette to black.
	memset(ThePalette,0,sizeof(ThePalette));
#endif
}


//---------------------------------------------------------------

std::int32_t CreatePalettes()
{
	HRESULT			dd_result;


	if(lp_DD_Palette==nullptr )
	{
#ifdef	_DEBUG
		dd_result	=	lp_DD->CreatePalette(
												DDPCAPS_8BIT,
												ThePalette,
												&lp_DD_Palette,
												nullptr
											);
#else
		dd_result	=	lp_DD->CreatePalette(
												DDPCAPS_8BIT|DDPCAPS_ALLOW256,
												ThePalette,
												&lp_DD_Palette,
												nullptr
											);
#endif
		if(dd_result!=DD_OK)
		{
			return	-1;
		}

		dd_result	=	lp_DD_FrontSurface->SetPalette(lp_DD_Palette);
		if(dd_result!=DD_OK)
		{
			LogText("  Unable to attach palette to lp_DD_FrontSurface\n");
			return	-1;
		}
/*
		if(dd_result!=DD_OK)		dd_result	=	lp_DD_BackSurface->SetPalette(lp_DD_Palette);

		{
			LogText("  Unable to attach palette to lp_DD_BackSurface\n");
			return	-1;
		}
*/
	}

	return	NoError;
}

//---------------------------------------------------------------

void DestroyPalettes()
{
	HRESULT		dd_result;


	if(lp_DD_Palette)
	{
		dd_result	=	lp_DD_Palette->Release();
		if(dd_result==DD_OK)
		{
			lp_DD_Palette	=	nullptr;
		}
		else
		{
//	char		text[256];
//			sprintf(text,"Unable to Release Palette. Error: %ld",dd_result&0xffff);
//			ERROR_MSG(0,text)
		}
	}
}

//---------------------------------------------------------------

void RestorePalettes()
{
	if(lp_DD_Palette && lp_DD_FrontSurface)
	{
		lp_DD_FrontSurface->SetPalette(lp_DD_Palette);
	}
}

//---------------------------------------------------------------

void SetPalette(std::uint8_t *the_palette)
{
#ifndef TARGET_DC
	std::uint32_t			c0;
	HRESULT			dd_result;


#ifdef _DEBUG
	// First set up the Windows static entries.
	for(c0=0;c0<10;c0++)
	{
	    // The first 10 static entries:
	    ThePalette[c0].peFlags	=	PC_EXPLICIT;
	    ThePalette[c0].peRed	=	(std::uint8_t)c0;
	    ThePalette[c0].peGreen	=	0;
	    ThePalette[c0].peBlue	=	0;

	    // The last 10 static entries:
	    ThePalette[c0+246].peFlags	=	PC_EXPLICIT;
	    ThePalette[c0+246].peRed	=	(std::uint8_t)(c0+246);
	    ThePalette[c0+246].peGreen	=	0;
	    ThePalette[c0+246].peBlue	=	0;
	}

	// Set up private entries.
	for(c0=10;c0<246;c0++)
	{
	    ThePalette[c0].peFlags	=	PC_NOCOLLAPSE|PC_RESERVED;
	    ThePalette[c0].peRed	=	(std::uint8_t)(the_palette[(c0*3)+0]);
	    ThePalette[c0].peGreen	=	(std::uint8_t)(the_palette[(c0*3)+1]);
	    ThePalette[c0].peBlue	=	(std::uint8_t)(the_palette[(c0*3)+2]);
	}
#else
	// Set up all entries.
	for(c0=0;c0<256;c0++)
	{
		ThePalette[c0].peRed	=	(std::uint8_t)(the_palette[(c0*3)+0]);
		ThePalette[c0].peGreen	=	(std::uint8_t)(the_palette[(c0*3)+1]);
		ThePalette[c0].peBlue	=	(std::uint8_t)(the_palette[(c0*3)+2]);
	}
#endif
	for(c0=0;c0<256;c0++)
	{
		CurrentPalette[c0*3+0]=ThePalette[c0].peRed;
		CurrentPalette[c0*3+1]=ThePalette[c0].peGreen;
		CurrentPalette[c0*3+2]=ThePalette[c0].peBlue;
	}
	if(lp_DD_Palette)
		dd_result	=	lp_DD_Palette->SetEntries(0,0,256,ThePalette);
#endif
}

//---------------------------------------------------------------

std::int32_t FindColour(std::uint8_t *the_palette,std::int32_t r,std::int32_t g,std::int32_t b)
{
	std::int32_t	found	=	-1;

	if(r>255)
		r=255;
	if(g>255)
		g=255;
	if(b>255)
		b=255;

	switch(WorkScreenDepth)
	{
		case 1:
		{
			
			std::int32_t	dist	=	0x7fffffff,
					c0,
					dist2,
					tr,
					tg,
					tb;


			for(c0=0;c0<256;c0++)
			{
				tr	=	*the_palette++;
				tg	=	*the_palette++;
				tb	=	*the_palette++;

				tr	-=	r;
				tg	-=	g;
				tb	-=	b;

				dist2=	abs(tr*tr)+abs(tg*tg)+abs(tb*tb);
				if(dist2<dist)
				{
					found	=	c0;
					dist	=	dist2;
					if(dist<8)
						return(c0);
				}
			}
			break;
		}
		case 2:

			 found=(((r>>3)<<11)|((g>>2)<<5)|(b>>3));
			break;
		case 4:
			 found=((r<<16)|(g<<8)|(b));
			break;

	}
	return(found);
}

//---------------------------------------------------------------
