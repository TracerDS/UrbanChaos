// KFrameEd.hpp
// Guy Simmons, 12th March 1997.

#pragma once

#include	"EditMod.hpp"
#include	"Anim.h"
#include	"Prim.h"
#include	"LevelEd.hpp"


#define	GOT_ANIM_COPY				(1<<0)

extern void	test_draw(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t tween,
						  struct KeyFrameElement *anim_info,struct KeyFrameElement *anim_info_next,
						  struct Matrix33 *rot_mat,
						  struct Matrix31 *parent_pos,	struct Matrix33 *parent_mat,
						  KeyFrameElement *parent_element,
						  struct Matrix31 *end_pos,		struct Matrix33 *end_mat
						  );

class	KeyFrameEditor	:	public	EditorModule
{
	private:
		std::int32_t			AnimAngleX[2],
						AnimAngleY[2],
						AnimAngleZ[2],
						AnimOffsetX[2],
						AnimOffsetY[2],
						AnimOffsetZ[2],
						AnimScale,
						AnimCount[2],
						AnimTween[2],
						AnimGlobalAngleX,
						AnimGlobalAngleY,
						AnimGlobalOffsetX,
						AnimGlobalOffsetY,
//						CurrentElement,
						Flags;
		std::uint8_t			SpeedFlag;
		std::int32_t			QuaternionFlag;
		std::int32_t			Flip1, Flip2;
		Anim			*AnimList[2],
						*CurrentAnim[2],
						*PlayingAnim[2];
		Character		*CurrentCharacter,
						TestCharacter;
		ControlSet		AnimControls;
		EdRect			AllAnimsRect,
						AnimFrameRect,
						CharactersRect,
						BodyPartRect,
						KeyFrameRect;
		KeyFrame		*CurrentFrame[2],
						*SelectedFrame;
		struct FightCol FightingCol;
		struct FightCol *FightingColPtr;
		std::uint32_t			FightColBank;
		std::uint8_t			VideoMode;
		std::uint8_t			PersonID,Bank;
		std::uint8_t			PersonBits[MAX_BODY_BITS];
		std::int32_t			DontDrawBoth;
		std::int32_t			MoveSeparately;
				
//		KeyFrameChunk	*TestChunk;
//		KeyFrameElement	*TheElements;

	public:

						~KeyFrameEditor();
		void			SetupModule();
		void			CreateKeyFrameTabs();
		void			DestroyKeyFrameTabs();
		void			DrawContent();
		void			DrawControls();
		void			HandleContentClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleControlClick(std::uint8_t flags,MFPoint *clicked_point);
		std::int32_t			HandleModuleKeys();
		void			HandleModule();
		void			AddKeyFrameChunk();
		void			HandleControl(std::uint32_t control_id);
		void			HandleAnimControl(std::uint32_t control_id);
		void			DrawCombatEditor();
		void			DrawPeopleTypes();
		void			DoCurrentAnim();
		// JCL - recursive anim stuff
		void			DoAnimRecurse(std::int32_t part_number, struct Matrix33 *mat, std::int32_t start_object,
									  struct Matrix31 *parent_pos, struct Matrix33 *parent_mat,
									  KeyFrameElement *parent_element);
		void			DoHierarchicalAnim();

		void			DrawKeyFrame(std::uint16_t multi_object,EdRect *bounds_rect,struct KeyFrame *the_frame,struct Matrix33 *r_matrix);
//		void			LoadMultiVUE(struct	KeyFrameChunk *the_chunk);
//		void			SortMultiObject(struct KeyFrameChunk *the_chunk);
//		void			test_draw(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t tween,struct KeyFrameElement *anim_info,struct KeyFrameElement *anim_info_next,struct Matrix33 *rot_mat);
		void			DrawKeyFrames();
		void			SetPersonBits();
		void			SetBodyType(std::int32_t part);
		std::int32_t			GetPartID(std::uint16_t current);
		void			DrawAnimFrames(Anim *the_anim,bool hilite);
		Anim			*DrawAllAnimsBox();
		void			ClearAll();
		void			AppendAnim();
		void			InsertAnim(Anim *insert_here);
		void			DestroyAnim(Anim *the_anim);
		void			LoadAllAnims(KeyFrameChunk *the_chunk);
		void			SaveAllAnims(KeyFrameChunk *the_chunk,std::int32_t save_all);
		void			LoadAnim(MFFileHandle file_handle,Anim *the_anim);
		void			SaveAnim(MFFileHandle file_handle,Anim *the_anim);
		void			SaveBodyPartInfo(MFFileHandle file_handle,std::int32_t version,KeyFrameChunk *the_chunk);
		void			LoadBodyPartInfo(MFFileHandle file_handle,std::int32_t version,KeyFrameChunk *the_chunk);

		void			LoadKeyFrameChunks();
		void			SaveChunkTextureInfo(KeyFrameChunk *the_chunk);
		void			LoadChunkTextureInfo(KeyFrameChunk *the_chunk);
		void			SetAnimBank(std::int32_t bank);

		std::int32_t			DragAndDropFrame(KeyFrame *selected_frame,std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h,MFPoint *clicked_point,std::uint32_t del_flag);

		LevelEditor		*LinkLevelEditor;
		inline std::int32_t	GetAnimAngleX()		{ return AnimAngleX[Bank]; }
		inline std::int32_t	GetAnimAngleY()		{ return AnimAngleY[Bank]; }
		inline std::int32_t	GetAnimAngleZ()		{ return AnimAngleZ[Bank]; }

		inline Anim		*GetCurrentAnim()		{	return CurrentAnim[Bank];		}
		inline std::uint8_t	GetPersonID()		{	return PersonID;		}
		inline void		SetSelectedFrame(KeyFrame *kframe)		{	SelectedFrame=kframe;		}

		std::int32_t			GetQuaternionFlag() {return (!(QuaternionFlag));};
};



