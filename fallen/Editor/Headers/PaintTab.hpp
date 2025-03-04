// PaintTab.hpp
// Guy Simmons, 7th April 1997

#pragma once

#include	"undo.hpp"
#include	"ModeTab.hpp"
#include	"BuildTab.hpp"
#include	"Stealth.h"


#define	PALETTE_PAINT		0
#define	TEXTURE_PAINT		1
#define	PLANAR_PAINT		2
#define	ANIM_TMAP_PAINT		3
#define	FLOOR_PAINT			4
#define	STYLE_PAINT			5
#define	STYLE_DEFINE		6
#define	INSTYLE_DEFINE		7
#define	PSX_TEX_DEFINE		8


#define	FLOOR_CUT_BRUSH			1
#define	FLOOR_PASTE_BRUSH		2


#define	FLAGS_SHOW_TEXTURE	(1<<0)
#define	FLAGS_QUADS			(1<<1)
#define	FLAGS_FIXED			(1<<2)
extern char	texture_style_names[200][21];

extern std::uint16_t	floor_texture_sizes[];

class	PaintTab	:	public	ModeTab
{
	private:
		std::uint8_t				CurrentColour;
		std::int32_t				CurrentTexturePage;
		std::uint32_t				PaintMode,
							TextureFlags;
		std::int32_t				TextureWidth,
							TextureHeight,
							TextureX,
							TextureY,
							TextureZoom;
		EdRect				ClickRect[4],
							AnimRect,
							PaintRect;
		EdTexture			CurrentTexture;
		std::int32_t				CurrentAnimTmap;
		std::int32_t				ShowAnimTmap;


		void				do_undo_me_bloody_self_then(std::int32_t index);

	public:
							PaintTab(EditorModule *parent);
							~PaintTab();
		void				UpdateTabInfo();
		void				DrawTabContent();
		void				DrawTexture();
		void				DrawStyleTexture();
		void				UpdateTexture();
		void				UpdateTextureInfo();
		void				DrawPalette();
		void				DrawAnimTmapSelector();
		void				UpdatePalette();
		void				UpdatePaletteInfo();
		void				HandleTab(MFPoint *current_point);
		std::uint16_t				HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void				HandleControl(std::uint16_t control_id);
		void				HandlePaletteControl(std::uint16_t control_id);
		void				HandleTextureControl(std::uint16_t control_id);
		void				HandleStyleControl(std::uint16_t control_id);
		void				SelectColour(MFPoint *clicked_point);
		void				SelectTexture(MFPoint *clicked_point);
		void				SelectStyle(MFPoint *clicked_point);
		void				SelectAnimTexture(MFPoint *clicked_point);
		void				PlanarMapping(MFPoint *clicked_point);
		void				DoPlanarMap();
		void				DoPlanarMapF();
		void				SetEditAnimTexture(MFPoint *clicked_point);
//		std::uint16_t				ConvertFreeToFixedEle(struct TextureBits *t);
		std::uint16_t				ConvertFreeToFixedEle(struct TextureBits *t,std::int32_t *x,std::int32_t *y,std::int32_t *width,std::int32_t *height,std::int32_t *page);
		void				ConvertFixedToFree(struct TextureBits *t);
		void				ConvertMiniTex(struct	MiniTextureBits	*tex);
		std::uint16_t				ConvertTexToMiniTex();
		void				CutFloorBrush(BuildTab *BuildMode,MFPoint *current_point);

		inline std::int32_t		GetTexturePage()			{	return CurrentTexturePage;	}
		inline void			SetTexturePage(std::int32_t page)		{	CurrentTexturePage=page;	}
		inline EdTexture	*GetTexture()				{	return &CurrentTexture;		}
		inline std::uint32_t		GetTextureFlags()			{	return TextureFlags;		}
		inline void			SetTextureFlags(std::uint32_t flags)	{	TextureFlags=flags;			}
		inline std::uint32_t		GetPaintMode()				{	return PaintMode;			}
		inline void			SetPaintMode(std::uint32_t mode)		{	PaintMode=mode;				}
		inline std::uint16_t		GetCurrentColour()			{	return CurrentColour;		}
		inline void			SetCurrentColour(std::uint16_t col)			{	CurrentColour=col;		}
		inline std::uint32_t		GetAnimTmap()				{	return CurrentAnimTmap;		}
		inline void			SetAnimTmap(std::uint32_t v)			{	CurrentAnimTmap=v;			}

		Undo				MyUndo;
		bool				ApplyTexture(struct EditFace *edit_face);
		EditorModule		*Parent;
		ControlSet			PaletteSet,
							StyleSet,
							inStyleSet,
							TextureSet;
		std::int32_t				CurrentTextureRot;
		std::int32_t				CurrentStyleEdit;
		std::int32_t				CurrentStylePos;
		std::int32_t				CurrentInStyleEdit;
		std::int32_t				CurrentInStylePos;
		std::int32_t				SubMode,SubStatus;
		MapBlock			CutMapBlock;

};



