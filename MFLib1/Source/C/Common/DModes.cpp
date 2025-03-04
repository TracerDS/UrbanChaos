// DModes.cpp
// Guy Simmons, 8th February 1996.

#include	<MFHeader.h>

//---------------------------------------------------------------

DisplayModeInfo	DisplayModes[]	=
{
	{	false,	0,0,0,			DISPLAY_MODE_NONE			},

	{	false,	320,200,8,		DISPLAY_MODE_320_200_8		},			//	320x200
	{	false,	320,200,16,		DISPLAY_MODE_320_200_16		},
	{	false,	320,200,24,		DISPLAY_MODE_320_200_24		},
	{	false,	320,200,32,		DISPLAY_MODE_320_200_32		},
	{	false,	320,240,8,		DISPLAY_MODE_320_240_8		},			//	320x240
	{	false,	320,240,16,		DISPLAY_MODE_320_240_16		},
	{	false,	320,240,24,		DISPLAY_MODE_320_240_24		},
	{	false,	320,240,32,		DISPLAY_MODE_320_240_32		},
	{	false,	320,400,8,		DISPLAY_MODE_320_400_8		},			//	320x400
	{	false,	320,400,16,		DISPLAY_MODE_320_400_16		},
	{	false,	320,400,24,		DISPLAY_MODE_320_400_24		},
	{	false,	320,400,32,		DISPLAY_MODE_320_400_32		},
	{	false,	512,384,8,		DISPLAY_MODE_512_384_8		},			//	512x384
	{	false,	512,384,16,		DISPLAY_MODE_512_384_16		},
	{	false,	512,384,24,		DISPLAY_MODE_512_384_24		},
	{	false,	512,384,32,		DISPLAY_MODE_512_384_32		},
	{	false,	640,400,8,		DISPLAY_MODE_640_400_8		},			//	640x400
	{	false,	640,400,16,		DISPLAY_MODE_640_400_16		},
	{	false,	640,400,24,		DISPLAY_MODE_640_400_24		},
	{	false,	640,400,32,		DISPLAY_MODE_640_400_32		},
	{	false,	640,480,8,		DISPLAY_MODE_640_480_8		},			//	640x480
	{	false,	640,480,16,		DISPLAY_MODE_640_480_16		},
	{	false,	640,480,24,		DISPLAY_MODE_640_480_24		},
	{	false,	640,480,32,		DISPLAY_MODE_640_480_32		},
	{	false,	800,600,8,		DISPLAY_MODE_800_600_8		},			//	800x600		
	{	false,	800,600,16,		DISPLAY_MODE_800_600_16		},
	{	false,	800,600,24,		DISPLAY_MODE_800_600_24		},
	{	false,	800,600,32,		DISPLAY_MODE_800_600_32		},
	{	false,	1024,768,8,		DISPLAY_MODE_1024_768_8		},			//	1024x768
	{	false,	1024,768,16,	DISPLAY_MODE_1024_768_16	},
	{	false,	1024,768,24,	DISPLAY_MODE_1024_768_24	},
	{	false,	1024,768,32,	DISPLAY_MODE_1024_768_32	},
	{	false,	1152,864,8,		DISPLAY_MODE_1152_864_8		},			//	1152x864
	{	false,	1152,864,16,	DISPLAY_MODE_1152_864_16	},
	{	false,	1152,864,24,	DISPLAY_MODE_1152_864_24	},
	{	false,	1152,864,32,	DISPLAY_MODE_1152_864_32	},
	{	false,	1280,1024,8,	DISPLAY_MODE_1280_1024_8	},			//	1280x1024		
	{	false,	1280,1024,16,	DISPLAY_MODE_1280_1024_16	},
	{	false,	1280,1024,24,	DISPLAY_MODE_1280_1024_24	},
	{	false,	1280,1024,32,	DISPLAY_MODE_1280_1024_32	}
};

//---------------------------------------------------------------

extern bool		EmulateLoRes;

std::int32_t	DisplayModeAvailable(std::uint32_t width, std::uint32_t height, std::uint32_t depth)
{
#ifdef	_DEBUG
	// Shut the compiler up.
	width	=	width;
	height	=	height;
	depth	=	depth;
	return	true;
#else
	std::uint32_t		c0;
	std::int32_t		result	=	DISPLAY_MODE_NONE;


	EmulateLoRes	=	false;
	for(c0=1;c0<DISPLAY_MODE_COUNT;c0++)
	{
		if	(
				DisplayModes[c0].Width==width		&&
				DisplayModes[c0].Height==height		&&
				DisplayModes[c0].Depth==depth
			)
		{
			if(DisplayModes[c0].Availability)
			{
				result	=	DisplayModes[c0].DisplayMode;
				break;
			}
		}	
	}

	if(result==DISPLAY_MODE_NONE && width==320 && depth==8 && (height==200||height==240))
	{
		EmulateLoRes	=	true;
		if(height==200)
			result			=	DISPLAY_MODE_320_200_8;
		else
			result			=	DISPLAY_MODE_320_240_8;
	}

	return	result;
#endif
}

//---------------------------------------------------------------
