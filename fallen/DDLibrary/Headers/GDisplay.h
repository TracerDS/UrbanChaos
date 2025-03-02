// Display.h
// Guy Simmons, 13th November 1997.

#ifndef	DISPLAY_H
#define	DISPLAY_H

#include	"DDManager.h"
#include	"D3DTexture.h"

//---------------------------------------------------------------

#define	SHELL_ACTIVE			(LibShellActive())
#define	SHELL_CHANGED			(LibShellChanged())
#define	FLAGS_USE_3D			(1<<1)
#define	FLAGS_USE_WORKSCREEN	(1<<2)

//---------------------------------------------------------------

#define	DEFAULT_WIDTH			(640)
#define	DEFAULT_HEIGHT			(480)
#define	DEFAULT_DEPTH			(16)


enum enumDisplayType
{
	DT_PAL,
	DT_NTSC,
	DT_VGA,
};

extern enumDisplayType eDisplayType;


//---------------------------------------------------------------

extern	void	InitBackImage(CBYTE *name);
extern	void UseBackSurface(LPDIRECTDRAWSURFACE4 use);
extern	void	ResetBackImage();
// Set b3DInFrame to FALSE if there is no 3D going on, i.e. blits will work on the DC.
// Ignored for the PC.
extern	void	ShowBackImage(bool b3DInFrame = TRUE);

SLONG			OpenDisplay(ULONG width, ULONG height, ULONG depth, ULONG flags);
SLONG			CloseDisplay();
SLONG			SetDisplay(ULONG width,ULONG height,ULONG depth);
SLONG			ClearDisplay(UBYTE r,UBYTE g,UBYTE b);
void			ShellPaused();
void			ShellPauseOn();
void			ShellPauseOff();

//---------------------------------------------------------------

#define	DISPLAY_INIT		(1<<0)
#define	DISPLAY_PAUSE		(1<<1)
#define	DISPLAY_PAUSE_ACK	(1<<2)
#define DISPLAY_LOCKED		(1<<3)

#define	BK_COL_NONE			0
#define	BK_COL_BLACK		1
#define	BK_COL_WHITE		2
#define	BK_COL_USER			3



class	Display
{
	private:
	protected:
		enum
		{
			DWF_FULLSCREEN			=	(1<<0),
			DWF_VISIBLE				=	(1<<1),
			DWF_ZBUFFER				=	(1<<2),
			DWF_ACTIVE				=	(1<<3),
			DWF_USE_3D				=	(1<<4),
			DWF_USE_WORK			=	(1<<5),
			DWF_DISPLAY_CHANGED		=	(1<<6),
			DWF_TEXTURES_INVALID	=	(1<<7)
		}Attributes;

		enum
		{
			DWF_VALID_INTERFACE		=	(1<<0),
			DWF_VALID_FULLSCREEN	=	(1<<1),
			DWF_VALID_FRONT			=	(1<<2),
			DWF_VALID_BACK			=	(1<<3),
			DWF_VALID_WORK			=	(1<<4),
			DWF_VALID_VIEWPORT		=	(1<<6),

			DWF_VALID				=	DWF_VALID_INTERFACE		|
										DWF_VALID_FULLSCREEN	|
										DWF_VALID_FRONT			|
										DWF_VALID_BACK
		}Validates;

		volatile SLONG			AttribFlags,				// Attribute flags.
								ValidFlags,					// Validation flags.
								PauseCount;



		inline	void TurnValidInterfaceOn()				{	ValidFlags	|=	DWF_VALID_INTERFACE;		}
		inline	void TurnValidInterfaceOff()			{	ValidFlags	&=	~DWF_VALID_INTERFACE;		}

		inline	void TurnValidFullscreenOn()			{	ValidFlags	|=	DWF_VALID_FULLSCREEN;		}
		inline	void TurnValidFullscreenOff()			{	ValidFlags	&=	~DWF_VALID_FULLSCREEN;		}

		inline	void TurnValidFrontOn()					{	ValidFlags	|=	DWF_VALID_FRONT;			}
		inline	void TurnValidFrontOff()				{	ValidFlags	&=	~DWF_VALID_FRONT;			}

		inline	void TurnValidBackOn()					{	ValidFlags	|=	DWF_VALID_BACK;				}
		inline	void TurnValidBackOff()					{	ValidFlags	&=	~DWF_VALID_BACK;			}

		inline	void TurnValidWorkOn()					{	ValidFlags	|=	DWF_VALID_WORK;				}
		inline	void TurnValidWorkOff()					{	ValidFlags	&=	~DWF_VALID_WORK;			}

		inline	void TurnValidViewportOn()				{	ValidFlags	|=	DWF_VALID_VIEWPORT;			}
		inline	void TurnValidViewportOff()				{	ValidFlags	&=	~DWF_VALID_VIEWPORT;		}

		inline	BOOL			IsValidDefaults()		{	return	((CurrDriver && CurrMode && CurrDevice) ? TRUE : FALSE);	}
		inline	BOOL			IsValidInterface()		{	return	((ValidFlags&DWF_VALID_INTERFACE) ? TRUE : FALSE);			}
		inline	BOOL			IsValidFullscreen()		{	return	((ValidFlags&DWF_VALID_FULLSCREEN) ? TRUE : FALSE);			}
		inline	BOOL			IsValidFront()			{	return	((ValidFlags&DWF_VALID_FRONT) ? TRUE : FALSE);				}
		inline	BOOL			IsValidBack()			{	return	((ValidFlags&DWF_VALID_BACK) ? TRUE : FALSE);				}
		inline	BOOL			IsValidWork()			{	return	((ValidFlags&DWF_VALID_WORK) ? TRUE : FALSE);				}
		inline	BOOL			IsValidViewport()		{	return	((ValidFlags&DWF_VALID_VIEWPORT) ? TRUE : FALSE);			}

		UBYTE *background_image_mem;

#ifdef TARGET_DC
		bool					bInScene;
		LPDIRECTDRAWSURFACE4	psurfBackHandle;
#endif

	public:
		ULONG					BackColour;
		D3DDeviceInfo			*CurrDevice;				// Current Device
		D3DMATERIALHANDLE		black_handle,
								white_handle,
								user_handle;
		D3DRECT					ViewportRect;
		D3DTexture				*TextureList;
		DDDriverInfo			*CurrDriver;				// Current Driver
		DDModeInfo				*CurrMode;					// Current Mode
		GUID					*DDGuid;
		LPDIRECT3D3				lp_D3D;
		LPDIRECT3DDEVICE3		lp_D3D_Device;
		LPDIRECT3DMATERIAL3		lp_D3D_Black,
								lp_D3D_White,
								lp_D3D_User;
		LPDIRECT3DVIEWPORT3		lp_D3D_Viewport;
		LPDIRECTDRAW			lp_DD;
		LPDIRECTDRAW4			lp_DD4;
		LPDIRECTDRAWSURFACE4	lp_DD_FrontSurface,
								lp_DD_BackSurface,
#ifndef TARGET_DC
								lp_DD_WorkSurface,
#endif
								lp_DD_ZBuffer;

		LPDIRECTDRAWSURFACE4	lp_DD_Background,
								lp_DD_Background_use_instead;
#ifdef TARGET_DC
		LPDIRECT3DTEXTURE2		lp_DD_Background_use_instead_texture;
		LPDIRECT3DTEXTURE2		lp_DD_Background_use_instead_texture2;
#endif
		IDirectDrawGammaControl*	lp_DD_GammaControl;
	    RECT					DisplayRect;				// Current surface rectangle.

		//
		// Pixel format variables.
		//

		SLONG					mask_red;
		SLONG					mask_green;
		SLONG					mask_blue;
		SLONG					shift_red;
		SLONG					shift_green;
		SLONG					shift_blue;

		//
		// For writing directly to the back-buffer. These
		// are only valid when the screen is locked.
		//

		SLONG					screen_width;
		SLONG                   screen_height;
		SLONG                   screen_pitch;
		SLONG                   screen_bbp;
		UBYTE			       *screen;

		volatile ULONG			DisplayFlags;

								Display();
								~Display();

		// Methods.
		HRESULT					Init();
		HRESULT					Fini();

		HRESULT					GenerateDefaults();

		HRESULT					InitInterfaces();
		HRESULT					FiniInterfaces();

		HRESULT					InitWindow();
		HRESULT					FiniWindow();

		HRESULT					InitFullscreenMode();
		HRESULT					FiniFullscreenMode();

		HRESULT					InitFront();
		HRESULT					FiniFront();

		HRESULT					InitBack();
		HRESULT					FiniBack();

#ifdef TARGET_DC
		HRESULT					InitBackCache();
		HRESULT					FiniBackCache();
#endif

		HRESULT					InitViewport();
		HRESULT					FiniViewport();
		HRESULT					UpdateViewport();

		HRESULT					InitWork();
		HRESULT					FiniWork();

		void					RunFMV();
		void					RunCutscene(int which, int language=0, bool bAllowButtonsToExit=TRUE );

		HRESULT					ChangeMode(SLONG w,SLONG h,SLONG bpp,SLONG refresh);

		bool					IsGammaAvailable();
		void					SetGamma(int black = 0, int white = 256);
		void					GetGamma(int* black, int* white);

		HRESULT					Restore();

		HRESULT					AddLoadedTexture(D3DTexture *the_texture);	// Remember this texture
		void					RemoveAllLoadedTextures();				// Forget all textures
		HRESULT					FreeLoadedTextures();					// Destroy each texture you know
		HRESULT					ReloadTextures();

		HRESULT					toGDI();
		HRESULT					fromGDI();

		void                   *screen_lock  ();
		void                    screen_unlock();

#ifndef TARGET_DC
		HRESULT					ShowWorkScreen();
#endif

		void					blit_back_buffer();
	
#ifdef TARGET_DC
		// Sod the user colour background stuff - it's junk.
		// Something always gets blitted over it anyway.
		void					SetUserColour(UBYTE red, UBYTE green, UBYTE blue) {}

		inline	HRESULT			SetBlackBackground()	{	return	DD_OK;	}
		inline	HRESULT			SetWhiteBackground()	{	return	DD_OK;	}
		inline	HRESULT			SetUserBackground()		{	return	DD_OK;	}
		inline	HRESULT			BeginScene()			{
																//TRACE("BeginScene ");
																if ( bInScene )
																{
																	return ( DD_OK );
																}
																else
																{
																	bInScene = TRUE;
																	return	lp_D3D_Device->BeginScene();
																}
															}
		inline	HRESULT			EndScene()				{	/*TRACE("EndScene ");*/ bInScene = FALSE; return	lp_D3D_Device->EndScene();	}
		inline	HRESULT			ClearViewport()			{
																HRESULT hres = lp_D3D_Viewport->Clear	(
																									1,
																									&ViewportRect,
																									D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
																								);
																return hres;
															}
#else //#ifdef TARGET_DC

		void					SetUserColour(UBYTE red, UBYTE green, UBYTE blue);

		inline	HRESULT			SetBlackBackground()	{	BackColour=BK_COL_BLACK;	return	lp_D3D_Viewport->SetBackground(black_handle);	}
		inline	HRESULT			SetWhiteBackground()	{	BackColour=BK_COL_WHITE;	return	lp_D3D_Viewport->SetBackground(white_handle);	}
		inline	HRESULT			SetUserBackground()		{	BackColour=BK_COL_USER;		return	lp_D3D_Viewport->SetBackground(user_handle);	}
		inline	HRESULT			BeginScene()			{	return	lp_D3D_Device->BeginScene();					}
		inline	HRESULT			EndScene()				{	return	lp_D3D_Device->EndScene();						}
		inline	HRESULT			ClearViewport()			{
																return	lp_D3D_Viewport->Clear	(
																									1,
																									&ViewportRect,
																									D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
																								);
															}
#endif //#else //#ifdef TARGET_DC

		inline	HRESULT			SetRenderState(D3DRENDERSTATETYPE type,SLONG state)
															{
																return	lp_D3D_Device->SetRenderState(type,state);
															}
		inline	HRESULT			SetTexture(LPDIRECT3DTEXTURE2 tex)
															{
																ASSERT ( lp_D3D_Device != NULL );
																return lp_D3D_Device->SetTexture(0, tex);
															}
		inline	HRESULT			SetTextureState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD dw)
															{
																return lp_D3D_Device->SetTextureStageState(stage, type, dw);
															}
		
		HRESULT			Flip(LPDIRECTDRAWSURFACE4 alt,SLONG flags);

		inline	HRESULT			DrawPrimitive(D3DPRIMITIVETYPE p_type,DWORD dwVertexDesc,LPVOID v,DWORD v_count,DWORD flags)
															{
																return	lp_D3D_Device->DrawPrimitive(p_type,dwVertexDesc,v,v_count,flags);
															}
		inline	HRESULT			DrawIndexedPrimitive(D3DPRIMITIVETYPE p_type,DWORD dwVertexDesc,LPVOID v,DWORD v_count,LPWORD i,DWORD i_count,DWORD flags)
															{
																return	lp_D3D_Device->DrawIndexedPrimitive(p_type,dwVertexDesc,v,v_count,i,i_count,flags);
															}


		inline	BOOL			IsValid()				{	return	(((ValidFlags&DWF_VALID)==DWF_VALID) ? TRUE : FALSE);	}

		inline	BOOL			IsDisplayChanged()		{	return	(((AttribFlags&DWF_DISPLAY_CHANGED)==DWF_DISPLAY_CHANGED) ? TRUE : FALSE);	}
		inline	void			DisplayChangedOn()		{	AttribFlags		|=	DWF_DISPLAY_CHANGED;	}
		inline	void			DisplayChangedOff()		{	AttribFlags		&=	~DWF_DISPLAY_CHANGED;	}

		inline	BOOL			IsTexturesInvalid()		{	return	(((AttribFlags&DWF_TEXTURES_INVALID)==DWF_TEXTURES_INVALID) ? TRUE : FALSE);}
		inline	void			TexturesInvalidOn()		{	AttribFlags		|=	DWF_TEXTURES_INVALID;	}
		inline	void			TexturesInvalidOff()	{	AttribFlags		&=	~DWF_TEXTURES_INVALID;	}

		inline	BOOL			IsFullScreen()			{	return	(AttribFlags&DWF_FULLSCREEN);		}
		inline	void			FullScreenOn()			{	AttribFlags		|=	DWF_FULLSCREEN;			}
		inline	void			FullScreenOff()			{	AttribFlags		&=	~DWF_FULLSCREEN;		}

		inline	BOOL			IsCreateZBuffer()		{	return	(AttribFlags&DWF_ZBUFFER);			}
		inline	void			CreateZBufferOn()		{	AttribFlags		|=	DWF_ZBUFFER;			}
		inline	void			CreateZBufferOff()		{	AttribFlags		&=	~DWF_ZBUFFER;			}

		inline	BOOL			IsUse3D()				{	return	(AttribFlags&DWF_USE_3D);			}
		inline	void			Use3DOn()				{	AttribFlags		|=	DWF_USE_3D;				}
		inline	void			Use3DOff()				{	AttribFlags		&=	~DWF_USE_3D;			}

#ifndef TARGET_DC
		inline	BOOL			IsUseWork()				{	return	(AttribFlags&DWF_USE_WORK);			}
		inline	void			UseWorkOn()				{	AttribFlags		|=	DWF_USE_WORK;			}
		inline	void			UseWorkOff()			{	AttribFlags		&=	~DWF_USE_WORK;			}
#endif
		
		inline	BOOL			IsInitialised()			{	return	DisplayFlags&DISPLAY_INIT;			}
		inline	void			InitOn()				{	DisplayFlags	|=	DISPLAY_INIT;			}
		inline	void			InitOff()				{	DisplayFlags	&=	~DISPLAY_INIT;			}

		inline DDDriverInfo		*GetDriverInfo()		{	return	CurrDriver;							}
		inline DDModeInfo		*GetModeInfo()			{	return	CurrMode;							}
		inline D3DDeviceInfo	*GetDeviceInfo()		{	return	CurrDevice;							}

		inline ULONG			GetFormattedPixel(UBYTE r, UBYTE g, UBYTE b) {return ((r >> mask_red) << shift_red) | ((g >> mask_green) << shift_green) | ((b >> mask_blue) << shift_blue);}

		//
		// The screen must be locked for these functions to work!
		//

		void					PlotPixel         (SLONG x, SLONG y, UBYTE red, UBYTE green, UBYTE blue);
		void					PlotFormattedPixel(SLONG x, SLONG y, ULONG colour);
		void                    GetPixel          (SLONG x, SLONG y, UBYTE *red, UBYTE *green, UBYTE *blue);

		//
		// A background picture... The image must be the same dimensions as the
		// back buffer and the image data must remain valid while the surface
		// is alive.
		//

		void create_background_surface(UBYTE *image_data);	// Must be same dimensions as back buffer!
		void use_this_background_surface(LPDIRECTDRAWSURFACE4 this_one);
		void blit_background_surface(bool b3DInFrame = TRUE);
		void destroy_background_surface();
};

//---------------------------------------------------------------

// they're macros now!
#define	DisplayWidth	640
#define DisplayHeight	480

extern SLONG			RealDisplayWidth;
extern SLONG			RealDisplayHeight;
extern SLONG			DisplayBPP;
extern Display			the_display;
extern volatile SLONG	hDDLibStyle,
						hDDLibStyleEx;
extern volatile	HMENU	hDDLibMenu;
extern volatile HWND	hDDLibWindow;

//---------------------------------------------------------------

#endif

