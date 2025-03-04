// LevelEdit.cpp
// Guy Simmons, 19th February 1997.

#ifndef	_LEVELED_HPP_
#define	_LEVELED_HPP_

#include	"Edit.h"
#include	"EditMod.hpp"
#include	"PaintTab.hpp"
#include	"PrPiTab.hpp"
#include	"LightTab.hpp"
#include	"ColTab.hpp"
#include	"MapTab.hpp"
#include	"MapEdTab.hpp"
#include	"BuildTab.hpp"
#include	"HmTab.hpp"
#include	"SewerTab.hpp"

#define	TAB_NONE			0
#define	TAB_PAINT			1
#define	TAB_PRIMPICK		2
#define	TAB_LIGHT			3
#define	TAB_MAP				4
#define	TAB_MAPED			5
#define	TAB_BUILD			6
#define TAB_HM				7
#define TAB_SEWER			8

#define	UNDO_NONE			0
#define	UNDO_TEXTURE		1


class	LevelEditor		:	public	EditorModule
{
	private:
		std::uint8_t			UndoType,
						LastU[4],
						LastV[4];
		ControlSet		PrimSet;
		std::uint16_t			CurrentAnimTmap;

	public:
						~LevelEditor();
		void			SetupModule();
		void			CreateLevelTabs();
		void			DestroyLevelTabs();
		void			DrawContent();
		void			DrawAnimTmapContent(std::int32_t current_anim_tmap);
		void			DrawTexStyleContent();
		void			DrawPSXTexContent();
		std::int32_t			HandlePSXTexClick(std::uint8_t flags,MFPoint *clicked_point);
		std::int32_t			HandleTexStyleClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleAnimTmapClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleContentClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleControlClick(std::uint8_t flags,MFPoint *clicked_point);
		void			DragEngine(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleStyleControl(std::uint32_t  control_id);
		void			HandlePSXControl(std::uint32_t  control_id);
		std::uint8_t			DoStylePopup(MFPoint *clicked_point,std::uint8_t flags);
		void			SetWallTextureInfo(std::int32_t	wall,std::uint8_t page,EdTexture	*current_texture);
		void			TextureFace(std::int16_t face,PaintTab *texture_mode);

		std::uint8_t			DoInStylePopup(MFPoint *clicked_point,std::uint8_t flags);
		void			DrawInTexStyleContent();
		void			HandleInStyleControl(std::uint32_t  control_id);
		void			DrawTexInStyleContent();
		std::int32_t			HandleTexInStyleClick(std::uint8_t flags,MFPoint *clicked_point);


		void			HandleModule();
		void			DoFacePopup(MFPoint *clicked_point);
		bool			ApplyTexture(struct EditFace *edit_face);
		inline void		SetAnimTexture(std::int32_t tmap)			{	CurrentAnimTmap=(std::uint16_t)tmap;	}
		ModeTab			*TestMode;
		PaintTab		*PaintMode;
		PrimPickTab		*PrimMode;
		LightTab		*LightMode;
		ColTab			*ColMode;
		MapTab			*MapMode;
		MapEdTab		*MapEdMode;
		BuildTab		*BuildMode;
		HmTab			*HmMode;
		SewerTab		*SewerMode;
		ControlSet		PSXControls;
		ControlSet		StyleControls;
		ControlSet		InStyleControls;
};

struct	LevelEdDefaults
{
	std::int32_t		Left,
				Top;
	std::int32_t		Height,
				Width;
};

#endif
