// KFrameEd.hpp
// Guy Simmons, 12th March 1997.

#ifndef	_KFRAMEED_HPP_
#define	_KFRAMEED_HPP_

#include	"EditMod.hpp"
#include	"Anim.h"
#include	"Prim.h"
#include	"LevelEd.hpp"


#define	GOT_ANIM_COPY				(1<<0)

extern	void	test_draw(UWORD	prim,SLONG x,SLONG y,SLONG z,SLONG tween,
						  struct KeyFrameElement *anim_info,struct KeyFrameElement *anim_info_next,
						  struct Matrix33 *rot_mat,
						  struct Matrix31 *parent_pos,	struct Matrix33 *parent_mat,
						  KeyFrameElement *parent_element,
						  struct Matrix31 *end_pos,		struct Matrix33 *end_mat
						  );

class	KeyFrameEditor	:	public	EditorModule
{
	private:
		SLONG			AnimAngleX[2],
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
		UBYTE			SpeedFlag;
		SLONG			QuaternionFlag;
		SLONG			Flip1, Flip2;
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
		ULONG			FightColBank;
		UBYTE			VideoMode;
		UBYTE			PersonID,Bank;
		UBYTE			PersonBits[MAX_BODY_BITS];
		SLONG			DontDrawBoth;
		SLONG			MoveSeparately;
				
//		KeyFrameChunk	*TestChunk;
//		KeyFrameElement	*TheElements;

	public:

						~KeyFrameEditor();
		void			SetupModule();
		void			CreateKeyFrameTabs();
		void			DestroyKeyFrameTabs();
		void			DrawContent();
		void			DrawControls();
		void			HandleContentClick(UBYTE flags,MFPoint *clicked_point);
		void			HandleControlClick(UBYTE flags,MFPoint *clicked_point);
		SLONG			HandleModuleKeys();
		void			HandleModule();
		void			AddKeyFrameChunk();
		void			HandleControl(ULONG control_id);
		void			HandleAnimControl(ULONG control_id);
		void			DrawCombatEditor();
		void			DrawPeopleTypes();
		void			DoCurrentAnim();
		// JCL - recursive anim stuff
		void			DoAnimRecurse(SLONG part_number, struct Matrix33 *mat, SLONG start_object,
									  struct Matrix31 *parent_pos, struct Matrix33 *parent_mat,
									  KeyFrameElement *parent_element);
		void			DoHierarchicalAnim();

		void			DrawKeyFrame(UWORD multi_object,EdRect *bounds_rect,struct KeyFrame *the_frame,struct Matrix33 *r_matrix);
//		void			LoadMultiVUE(struct	KeyFrameChunk *the_chunk);
//		void			SortMultiObject(struct KeyFrameChunk *the_chunk);
//		void			test_draw(UWORD	prim,SLONG x,SLONG y,SLONG z,SLONG tween,struct KeyFrameElement *anim_info,struct KeyFrameElement *anim_info_next,struct Matrix33 *rot_mat);
		void			DrawKeyFrames();
		void			SetPersonBits();
		void			SetBodyType(SLONG part);
		SLONG			GetPartID(UWORD current);
		void			DrawAnimFrames(Anim *the_anim,bool hilite);
		Anim			*DrawAllAnimsBox();
		void			ClearAll();
		void			AppendAnim();
		void			InsertAnim(Anim *insert_here);
		void			DestroyAnim(Anim *the_anim);
		void			LoadAllAnims(KeyFrameChunk *the_chunk);
		void			SaveAllAnims(KeyFrameChunk *the_chunk,SLONG save_all);
		void			LoadAnim(MFFileHandle file_handle,Anim *the_anim);
		void			SaveAnim(MFFileHandle file_handle,Anim *the_anim);
		void			SaveBodyPartInfo(MFFileHandle file_handle,SLONG version,KeyFrameChunk *the_chunk);
		void			LoadBodyPartInfo(MFFileHandle file_handle,SLONG version,KeyFrameChunk *the_chunk);

		void			LoadKeyFrameChunks();
		void			SaveChunkTextureInfo(KeyFrameChunk *the_chunk);
		void			LoadChunkTextureInfo(KeyFrameChunk *the_chunk);
		void			SetAnimBank(SLONG bank);

		SLONG			DragAndDropFrame(KeyFrame *selected_frame,SLONG x,SLONG y,SLONG w,SLONG h,MFPoint *clicked_point,ULONG del_flag);

		LevelEditor		*LinkLevelEditor;
		inline SLONG	GetAnimAngleX()		{ return AnimAngleX[Bank]; }
		inline SLONG	GetAnimAngleY()		{ return AnimAngleY[Bank]; }
		inline SLONG	GetAnimAngleZ()		{ return AnimAngleZ[Bank]; }

		inline Anim		*GetCurrentAnim()		{	return CurrentAnim[Bank];		}
		inline UBYTE	GetPersonID()		{	return PersonID;		}
		inline void		SetSelectedFrame(KeyFrame *kframe)		{	SelectedFrame=kframe;		}

		SLONG			GetQuaternionFlag() {return (!(QuaternionFlag));};
};


#endif

