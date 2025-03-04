#ifndef	DRAWTYPE_H
#define	DRAWTYPE_H	
//
// Draw Types
//

#include "cache.h"

#define RMAX_DRAW_TWEENS		RMAX_PEOPLE+30
#define RMAX_DRAW_MESHES		320 //500

#define MAX_DRAW_TWEENS		(save_table[SAVE_TABLE_DTWEEN].Maximum)
#define MAX_DRAW_MESHES		(save_table[SAVE_TABLE_DMESH].Maximum)

#define	DT_NONE				0
#define	DT_BUILDING			1
#define	DT_PRIM				2
#define	DT_MULTI_PRIM		3
#define	DT_ROT_MULTI		4
#define	DT_EFFECT			5
#define DT_MESH				6
#define DT_TWEEN			7
#define	DT_SPRITE			8
#define DT_VEHICLE			9
#define DT_ANIM_PRIM		10
#define DT_CHOPPER			11
#define DT_PYRO				12
#define DT_ANIMAL_PRIM		13
#define	DT_TRACK			14
#define DT_BIKE				15

#define	DT_FLAG_UNUSED		(1<<7)
#define	DT_FLAG_GUNFLASH	(1<<6)


//
// structs
//

typedef struct
{
	std::uint8_t				TweakSpeed;
	std::int8_t				Locked;     //which frame if any is locked in place
	std::uint8_t				FrameIndex;
	std::uint8_t				QueuedFrameIndex;
	
	std::int16_t				Angle,AngleTo,
						Roll,DRoll,
						Tilt,TiltTo;

	std::int32_t				CurrentAnim,
						AnimTween,
						TweenStage;
	struct	GameKeyFrame			*CurrentFrame,
						*NextFrame,
						*InterruptFrame,
						*QueuedFrame;
	struct	GameKeyFrameChunk		*TheChunk;

	std::uint8_t				Flags;
	std::uint8_t				Drawn;  //game turn last drawn
	std::uint8_t				MeshID;
	std::uint8_t				PersonID;
}DrawTween;

typedef struct
{
	std::uint16_t				Angle;
	std::uint16_t				Roll;
	std::uint16_t				Tilt;
	std::uint16_t				ObjectId;
	CACHE_Index			Cache;  //ubyte
	std::uint8_t				Hm;		// 255 => nullptr

}DrawMesh;




//
// Functions
//

void init_draw_tweens();
DrawTween *alloc_draw_tween(std::int32_t type);
void free_draw_tween(DrawTween *draw_tween);


//
// DrawMesh functions.
//

void init_draw_meshes();
DrawMesh *alloc_draw_mesh ();
void free_draw_mesh  (DrawMesh *drawmesh);






#endif
