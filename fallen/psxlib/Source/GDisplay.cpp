// GDisplay.cpp
// Mike Diskett 19th April 1998

// Hardware Level Display stuff for Playstation
//
//.

#include	<MFStdLib.h>
//#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include <libapi.h>
#include <libcd.h>
#include <libpad.h>
#include <libmcrd.h>

#include	"c:\fallen\psxlib\headers\GDisplay.h"
#include	"c:\fallen\headers\night.h"
#include	"c:\fallen\psxeng\headers\psxeng.h"
#include	"c:\fallen\headers\game.h"

std::int32_t DisplayWidth,
					DisplayHeight,
					DisplayBPP;
Display the_display;

#if 1
char *GDisp_Bucket;
#else
char GDisp_Bucket[BUCKET_MEM];
#endif

#include "ctrller.h"

ControllerPacket PAD_Input1,PAD_Input2;

std::uint32_t GDisp_OTag[OTSIZE];
std::uint32_t GDisp_OTag2[OTSIZE];

extern void	PCReadFile(char* name,std::uint8_t *addr,std::uint32_t len);
extern std::uint8_t Wadmenu_Backdrop[];

extern void MFX_init();
extern void AENG_flip2(std::uint32_t *back_image);

extern int screen_x;
extern int screen_y;

//---------------------------------------------------------------
std::int32_t InitHardware()
{
	std::uint8_t act[2];
#ifndef FS_ISO9660
	PCinit();
#endif

	ResetCallback();

	CdInit();
	MFX_init();

#ifndef VERSION_DEMO
	MemCardInit(1);
	MemCardStart();
#endif
	
	PadInitDirect((char*)&PAD_Input1,(char*)&PAD_Input2);
//	act[0]=act[1]=0;
//	PadSetAct(0,act,2);
	PadStartCom();

	PAD_Input1.data.pad=0xffff;
	PAD_Input2.data.pad=0xffff;

	InitGeom();			/* initialize geometry subsystem */

}

void InitVideo()
{
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

#ifdef VERSION_NTSC
	SetVideoMode(MODE_NTSC);
#else
	SetVideoMode(MODE_PAL);
#endif
	SetGeomOffset(DisplayWidth>>1, DisplayHeight>>1);	/* set geometry origin as (160, 120) */

	SetGeomScreen(420);		/* distance to viewing-screen */
	SetFogNearFar(810,20000,420);

	SetDispMask(1);

	return(1);
}

void ReleaseHardware()
{
	StopCallback();
	PadStopCom();
	ResetGraph(3);
}


std::uint16_t psx_tpages[22];
//std::uint16_t	psx_tpages_clut[16];

#define	MAX_PATH	128
#define	FILE	std::int32_t

#define	MFFileHandle	std::int32_t
#define	FILE_OPEN_ERROR	(-1)
#define	SEEK_MODE_CURRENT	(1)

extern std::int32_t	SpecialOpen(char* name);
extern std::int32_t	SpecialRead(std::int32_t handle,std::uint8_t *ptr,std::int32_t s1);
extern std::int32_t	SpecialSeek(std::int32_t handle,std::int32_t mode,std::int32_t size);
extern std::int32_t	SpecialSize(std::int32_t handle);

#define	FileOpen(x)		SpecialOpen(x)
#define	FileClose(x)	SpecialClose(x)
#define	FileCreate(x,y)	ASSERT(0)
#define	FileRead(h,a,s) SpecialRead(h,(std::uint8_t*)a,s)
#define	FileWrite(h,a,s) ASSERT(0)
#define	FileSeek(h,m,o) SpecialSeek(h,m,o)
#define FileSize(h)		SpecialSize(h)
	
#define	fopen(x,y)		SpecialOpen(x)
#define	fclose(x)		SpecialClose(x)
#define	fread(a,s1,s2,h) SpecialRead(h,a,s1*s2)

void setup_textures(int world)
{
	std::uint8_t	*addr;
	char	name[32];
	TIM_IMAGE	tim;
	std::int32_t	c0;
	std::int32_t	id;

#if 0
	addr=(std::uint8_t*)&GDisp_Bucket[BUCKET_MEM-(34*1024)];

	for(c0=1;c0<19;c0++)
	{
		sprintf(name,"data\\textures\\world%d\\tex%02d.tim",world,c0);
//		printf("%s\n",name);

		PCReadFile(name,addr,32788);
		if(OpenTIM((std::uint32_t*)addr)==0)
		{
			ReadTIM(&tim);
			{
				DrawSync(0);
		
				LoadImage(tim.prect,tim.paddr);
				if (c0<17)
					psx_tpages[c0-1]=getTPage(0,1,tim.prect->x,tim.prect->y);

//				psx_tpages_clut[c0-1]=getClut(tim.crect->x,tim.crect->y);
			}
		}
	}
#else
	RECT r;
	r.x=512;
	r.y=0;
	r.w=512;
	r.h=512;

	addr=(std::uint8_t*)MemAlloc(524288+196608);
#ifndef VERSION_DEMO
	if (world==0)
	{
		sprintf(name,"levels0\\level00\\texture%d.tex",Wadmenu_Backdrop[wad_level]);
	}
	else
		sprintf(name,"levels%d\\level%02d\\textures.tex",world/10,world);
#else
	if (world==0)
		sprintf(name,"urban\\level00\\texture0.tex");
	else
		sprintf(name,"urban\\level%02d\\textures.tex",world);
#endif

	PCReadFile(name,addr,524288+196608);
	LoadImage2(&r,(std::uint32_t*)addr);
	DrawSync(0);
	if (world!=0)
	{
		r.x=320;
		r.w=192;
		LoadImage(&r,(std::uint32_t*)&addr[524288]);
		DrawSync(0);
	}
	MemFree((void*)addr);

	for(c0=0;c0<16;c0++)
		psx_tpages[c0]=getTPage(0,1,512+((c0&7)<<6),(c0&8)<<5);
	psx_tpages[16]=getTPage(0,1,320,0);
	psx_tpages[17]=getTPage(0,1,320,256);
	psx_tpages[18]=getTPage(0,1,384,0);
	psx_tpages[19]=getTPage(0,1,384,256);
	psx_tpages[20]=getTPage(0,1,448,0);
	psx_tpages[21]=getTPage(0,1,448,256);

#endif
}

void TEXTURE_choose_set(std::int32_t number)
{
//extern void	load_texture_styles(std::uint8_t editor, std::uint8_t world);
//	number=13;
//	load_texture_styles(false, number);
	setup_textures(number);
}

/*
void LoadTitle()
{
	std::uint8_t *addr;
	TIM_IMAGE	tim;

	addr=MemAlloc(262168);
	if (addr==0)
	{
		printf("Fuckup, cannot allocate memory for Titlepage.\n");
		exit(1);
	}
	PCReadFile("data\\gamelogo.tim",addr,262168);

	if(OpenTIM((std::uint32_t*)addr)==0)
	{
		ReadTIM(&tim);
		LoadImage(tim.prect,tim.paddr);
		tim.prect->y=0;
		DrawSync(0);
		LoadImage(tim.prect,tim.paddr);
		DrawSync(0);
	}
	MemFree(addr);
}
*/

#if 1
void GDisp_SetupBucketMem(char *addr,std::int32_t size)
{
	// Clear the ordering tables (we dont want stray polygons in an area of memory that
	// is no longer valid.
	ClearOTag(the_display.DisplayBuffers[0].ot,OTSIZE);
	ClearOTag(the_display.DisplayBuffers[1].ot,OTSIZE);

	// Next we point GDisp_Bucket to the appropriate location
	GDisp_Bucket=addr;

	// We'll make the prim buffers point to the address of our new display bucket
	the_display.DisplayBuffers[0].PrimMem=the_display.DisplayBuffers[1].PrimMem=GDisp_Bucket;
	the_display.CurrentPrim=the_display.CurrentDisplayBuffer->PrimMem;

	// Finally we set the amount of memory to size
	BUCKET_MEM=size;
}   
#endif
		  
std::int32_t OpenDisplay(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint32_t flags)
{
	/* initialize environment for double buffer 
	 *	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,256)-(320,480) (320x240)
	 */

#ifdef VERSION_LORES
	DisplayWidth=320;
#else
	DisplayWidth=512;
#endif

#ifdef VERSION_NTSC
	DisplayHeight=240;//256;
#else
	DisplayHeight=256;
#endif

	SetDefDrawEnv(&the_display.DisplayBuffers[0].Draw, 0,   0, DisplayWidth, DisplayHeight);	
	SetDefDrawEnv(&the_display.DisplayBuffers[1].Draw, 0, 256, DisplayWidth, DisplayHeight);
	SetDefDispEnv(&the_display.DisplayBuffers[0].Disp, 0, 256, DisplayWidth, DisplayHeight);
	SetDefDispEnv(&the_display.DisplayBuffers[1].Disp, 0,   0, DisplayWidth, DisplayHeight);

	the_display.DisplayBuffers[0].Disp.screen.x=screen_x;
	the_display.DisplayBuffers[1].Disp.screen.x=screen_x;
	the_display.DisplayBuffers[0].Disp.screen.y=screen_y;
	the_display.DisplayBuffers[1].Disp.screen.y=screen_y;

//	the_display.DisplayBuffers[0].PrimMem=the_display.DisplayBuffers[1].PrimMem=GDisp_Bucket;

	the_display.DisplayBuffers[0].ot=GDisp_OTag;
	the_display.DisplayBuffers[1].ot=GDisp_OTag2;
//	the_display.DisplayBuffers[0].ot=the_display.DisplayBuffers[1].ot=GDisp_OTag;
						 
	the_display.CurrentDisplayBuffer=&the_display.DisplayBuffers[0];
	the_display.CurrentPrim=the_display.CurrentDisplayBuffer->PrimMem;

	ClearOTag(the_display.CurrentDisplayBuffer->ot, OTSIZE);
	
	the_display.DisplayBuffers[0].Draw.isbg=1;
	the_display.DisplayBuffers[0].Draw.r0=0;
	the_display.DisplayBuffers[0].Draw.g0=0;
	the_display.DisplayBuffers[0].Draw.b0=0;

	the_display.DisplayBuffers[1].Draw.isbg=1;
	the_display.DisplayBuffers[1].Draw.r0=0;
	the_display.DisplayBuffers[1].Draw.g0=0;
	the_display.DisplayBuffers[1].Draw.b0=0;

	PutDispEnv(&the_display.CurrentDisplayBuffer->Disp); 
	PutDrawEnv(&the_display.CurrentDisplayBuffer->Draw); 

	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	// Commented out whilst menu is in, this was only temporary only NEway
//	LoadTitle();
//	setup_textures(0);
extern void AENG_flip_init();
	AENG_flip_init();

	return(0);
}

void SetDisplayClear(std::int32_t clear)
{
	the_display.DisplayBuffers[0].Draw.isbg=clear;
	the_display.DisplayBuffers[1].Draw.isbg=clear;
}

void SetDisplayFade()
{
//	int b0;
//	if (NIGHT_flag & NIGHT_FLAG_DAYTIME)
//		b0=255;
//	else
//		b0=0;

	the_display.DisplayBuffers[0].Draw.r0=0;//NIGHT_sky_colour.red;
	the_display.DisplayBuffers[0].Draw.g0=0;//NIGHT_sky_colour.green;
	the_display.DisplayBuffers[0].Draw.b0=0;//NIGHT_sky_colour.blue;
	the_display.DisplayBuffers[1].Draw.r0=0;//NIGHT_sky_colour.red;
	the_display.DisplayBuffers[1].Draw.g0=0;//NIGHT_sky_colour.green;
	the_display.DisplayBuffers[1].Draw.b0=0;//NIGHT_sky_colour.blue;
}


//---------------------------------------------------------------

std::int32_t CloseDisplay()
{
}

//---------------------------------------------------------------

std::int32_t SetDisplay(std::uint32_t width,std::uint32_t height,std::uint32_t depth)
{
}


void ShowBackImage()
{
}
void LoadBackImage(char* name)
{
}
void InitBackImage(char* name)
{
}
void ResetBackImage()
{
}

std::uint8_t psx_motor[2];
extern int vibra_mode;
extern int psx_send;

void PSX_SetShock(std::uint8_t fast,std::uint8_t slow)
{

	if (vibra_mode && !(GAME_STATE &(GS_LEVEL_LOST|GS_LEVEL_WON)))
	{
		if (fast>psx_motor[0])
			psx_motor[0]=fast;
		if (slow>psx_motor[1])
			psx_motor[1]=slow;
		psx_send=1;
	} 
	else
	{
		psx_motor[0]=0;
		psx_motor[1]=0;
	}

}




