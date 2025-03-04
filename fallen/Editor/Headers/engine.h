#pragma once
#include	"poly.h"

//*********************************************************
// 				DEFINES
//*********************************************************
/*
//#define		FLOAT_ENGINE			1

#ifdef		FLOAT_ENGINE
typedef	float					FSLONG;
#else
typedef	long				FSLONG;
#endif
*/
#define		MAX_BLOCKS				15000
#define		MAX_BUCKETS				40960
#define		MAX_BUCKET_POOL			3000000   /* In BytES */
#define		MAX_BUCKET_POINTS		MAX_BUCKET_POOL

#define		BT_QUAD					1
#define		BT_TRI					2
#define		BT_VECT					3
#define		BT_RECT					4
#define		BT_DX_QUAD				5
#define		BT_DX_TRI				6

// Game Editor bits.
#define		BT_MAP_THING			7
#define		BT_WAYPOINT				8
#define		BT_LINE					9
#define		BT_SPHERE_AREA			10
#define		BT_RECT_AREA			11

#define	EF_OFF_LEFT		(1<<0)
#define	EF_OFF_RIGHT	(1<<1)
#define	EF_OFF_TOP		(1<<2)
#define	EF_OFF_BOTTOM	(1<<3)
#define	EF_FAR_OUT		(1<<4)
#define	EF_BEHIND_YOU	(1<<5)
#define	EF_TRANSLATED	(1<<6)
#define	EF_TOO_BIG		(1<<7)

#define	EF_CLIPFLAGS 	(EF_OFF_LEFT+EF_OFF_RIGHT+EF_OFF_TOP+EF_OFF_BOTTOM)


#define	ENGINE_CLIPZ_FLAG	(1<<0)
#define	ENGINE_CLIPX_FLAG	(1<<1)
#define	ENGINE_CLIPY_FLAG	(1<<2)

/****************************************************************************/
/* STRUCTS  	-	Just for the engine.									*/
/****************************************************************************/


struct	EngineStuff
{
	std::int32_t	CosY,SinY,CosX,SinX,CosZ,SinZ;
	std::int32_t	CosDY,SinDY;
	std::int32_t	X,Y,Z;
	std::int32_t	Scale;
	std::int32_t	VW;
	std::int32_t	VH;
	std::int32_t	VW2;
	std::int32_t	VH2;
	std::int32_t	AngleX;
	std::int32_t	AngleY;
	std::int32_t	AngleZ;
	std::int32_t	AngleDY;
	std::uint32_t	ShowDebug;
	std::uint32_t	ExtraInfo;
	std::uint32_t	BucketSize;
	std::int32_t	MousePosX;
	std::int32_t	MousePosY;
	std::int32_t	MousePosZ;
	std::int32_t	DX;
	std::int32_t	DY;
	std::int32_t	DZ;
	std::int32_t	ClipZ;
	std::int32_t	ClipMaxY;
	std::int32_t	ClipMinY;
	std::int32_t	ClipMaxX;
	std::int32_t	ClipMinX;
	std::int32_t	ClipFlag;
	std::int32_t	TrueY;
};



//---------------------------------------------------------------
//	A funny fanny thing for the game editor.

#define	ED_ITEM_NONE		0
#define	ED_ITEM_THING		1
#define	ED_ITEM_MAP_BLOCK	2
#define	ED_ITEM_BUILDING	3
#define	ED_ITEM_WAYPOINT	4
#define	ED_ITEM_SIZE_HOOK	5

struct	EdItem
{
	std::uint8_t		ItemType;
	std::int32_t		ItemRef;
};

//---------------------------------------------------------------

struct	BucketHead
{
	void	*BucketPtr;
};

struct	BucketGeneric
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
};


struct	BucketQuad
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::uint8_t	TextPage;
	std::uint8_t	Col;
	struct	MfEnginePoint	P[4];
	std::int32_t	DebugInfo;
	std::int32_t	DebugFlags;
	EdItem	EditRef;
};

struct	BucketTri
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::uint8_t	TextPage;
	std::uint8_t	Col;
	struct	MfEnginePoint	P[3];
	std::int32_t	DebugInfo;
	std::int32_t	DebugFlags;
	EdItem	EditRef;
};

struct	BucketVect
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	struct	MfEnginePoint	P[2];
	std::uint16_t	Col;
	std::int32_t	DebugInfo;
};

struct	BucketRect
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	struct	MfEnginePoint	P[1];
	std::int32_t	Width;
	std::int32_t	Height;
	std::uint8_t	Col;
};

// Game Editor stuff.
struct	BucketMapThing
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::int32_t	BaseX,BaseY,
			X,Y;
	EdItem	EditRef;
};

struct	BucketWaypoint
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::int32_t	BaseX,BaseY,
			X,Y;
	EdItem	EditRef;
};

struct	BucketLine
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::int32_t	X1,Y1,
			X2,Y2;
};

struct	BucketSphereArea
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::int32_t	X,Y,Radius;
	bool	ShowSizeHook;
	EdItem	EditRef;
};

struct	BucketRectArea
{
	void	*BucketPtr;
	std::uint16_t	DrawFlags;
	std::uint8_t	BucketType;
	std::int32_t	X,Y,Z;
	bool	ShowSizeHook;
	EdItem	EditRef;
};

// Added Direct3D bucket types, Guy - 2/10/97

struct	BucketDXQuad
{
	void				*BucketPtr;
	std::uint16_t				DrawFlags;
	std::uint8_t				BucketType;
	MFDXEnginePoint		P[4];
	std::uint8_t				TexturePage;
};
typedef	BucketDXQuad		BucketDXQuad;

struct	BucketDXTri
{
	void				*BucketPtr;
	std::uint16_t				DrawFlags;
	std::uint8_t				BucketType;
	MFDXEnginePoint		P[3];
	std::uint8_t				TexturePage;
};
typedef	BucketDXTri			BucketDXTri;


//*********************************************************
// 				EXTERNS
//*********************************************************

//DATA
extern std::int32_t	poly_count;

extern std::uint8_t	bucket_pool[MAX_BUCKET_POOL];
extern std::uint16_t	select_colour;
extern struct	BucketHead	bucket_heads[MAX_BUCKETS];
extern std::uint8_t	*current_bucket_pool;
extern std::uint8_t	*end_bucket_pool;

//FUNCTIONS
extern struct	EngineStuff	engine;

extern void	set_camera_to_base();
extern void	set_camera_to_mid();
extern void	set_camera();
extern void	set_camera_plan();
extern void	set_camera_front();
extern void	set_camera_side();
extern void	set_camera_angledy(std::int16_t angle);
extern void	clear_camera_angledy();

extern void	init_camera();
extern void	render_view(std::uint8_t highlight);
extern void	init_engine();
extern void	add_bucket(void* p_bucket,std::int32_t z);
extern std::uint32_t (*rotate_point_gte)(struct	SVector *v,struct SVector *r);
extern std::uint32_t	rotate_point_gte_perspective(struct	SVector *v,struct SVector *r);
extern std::uint32_t	rotate_point_gte_normal(struct	SVector *v,struct SVector *r);
extern void	calc_world_pos_plan(std::int32_t x,std::int32_t y);
extern void	calc_world_pos_front(std::int32_t x,std::int32_t y);

extern void	animate_texture_maps();


//*********************************************************
// 				MACROS
//*********************************************************

#define	setZ4(p,z1,z2,z3,z4)   					(p)->P[0].Z3d  =(std::int16_t)(z1);(p)->P[1].Z3d  =(std::int16_t)(z2);(p)->P[2].Z3d   =(std::int16_t)(z3);(p)->P[3].Z3d  =(std::int16_t)(z4)
#define	setXY4(p,x1,y1,x2,y2,x3,y3,x4,y4)   	(p)->P[0].X    =(std::int16_t)(x1);(p)->P[1].X    =(std::int16_t)(x2);(p)->P[2].X     =(std::int16_t)(x3);(p)->P[3].X    =(std::int16_t)(x4);(p)->P[0].Y =(std::int16_t)(y1);(p)->P[1].Y =(std::int16_t)(y2);(p)->P[2].Y =(std::int16_t)(y3);(p)->P[3].Y =(std::int16_t)(y4)
#define	setUV4NA(p,x1,y1,x2,y2,x3,y3,x4,y4,page)                                \
													(p)->P[0].TX   =(std::int16_t)(x1);\
													(p)->P[1].TX   =(std::int16_t)(x2);\
													(p)->P[2].TX    =(std::int16_t)(x3);\
													(p)->P[3].TX   =(std::int16_t)(x4);\
													(p)->P[0].TY=(std::int16_t)(y1);\
													(p)->P[1].TY=(std::int16_t)(y2);\
													(p)->P[2].TY=(std::int16_t)(y3);\
													(p)->P[3].TY=(std::int16_t)(y4);\
													(p)->TextPage=(std::int16_t)(page);

#define	setUV4(p,x1,y1,x2,y2,x3,y3,x4,y4,page)	if(page>=0)\
												{\
													(p)->P[0].TX   =(std::int16_t)(x1);\
													(p)->P[1].TX   =(std::int16_t)(x2);\
													(p)->P[2].TX    =(std::int16_t)(x3);\
													(p)->P[3].TX   =(std::int16_t)(x4);\
													(p)->P[0].TY=(std::int16_t)(y1);\
													(p)->P[1].TY=(std::int16_t)(y2);\
													(p)->P[2].TY=(std::int16_t)(y3);\
													(p)->P[3].TY=(std::int16_t)(y4);\
													(p)->TextPage=(std::int16_t)(page);\
												}\
												else\
												{\
													std::int32_t	temp_unique;\
													temp_unique=anim_tmaps[-(page)].Current;\
													(p)->P[0].TX   =anim_tmaps[-(page)].UV[temp_unique][0][0];\
													(p)->P[1].TX   =anim_tmaps[-(page)].UV[temp_unique][1][0];\
													(p)->P[2].TX   =anim_tmaps[-(page)].UV[temp_unique][2][0];\
													(p)->P[3].TX   =anim_tmaps[-(page)].UV[temp_unique][3][0];\
													(p)->P[0].TY   =anim_tmaps[-(page)].UV[temp_unique][0][1];\
													(p)->P[1].TY   =anim_tmaps[-(page)].UV[temp_unique][1][1];\
													(p)->P[2].TY   =anim_tmaps[-(page)].UV[temp_unique][2][1];\
													(p)->P[3].TY   =anim_tmaps[-(page)].UV[temp_unique][3][1];\
													(p)->TextPage  =anim_tmaps[-(page)].Page[temp_unique];\
												}\

#define	setShade4(p,s1,s2,s3,s4)				(p)->P[0].Shade=(std::int16_t)(s1);(p)->P[1].Shade=(std::int16_t)(s2);(p)->P[2].Shade=(std::int16_t)(s3);(p)->P[3].Shade=(std::int16_t)(s4)

#define	setZ3(p,z1,z2,z3)   					(p)->P[0].Z3d  =(std::int16_t)(z1);(p)->P[1].Z3d  =(std::int16_t)(z2);(p)->P[2].Z3d   =(std::int16_t)(z3)
#define	setXY3(p,x1,y1,x2,y2,x3,y3)				(p)->P[0].X    =(std::int16_t)(x1);(p)->P[1].X    =(std::int16_t)(x2);(p)->P[2].X     =(std::int16_t)(x3);(p)->P[0].Y =(std::int16_t)(y1);(p)->P[1].Y =(std::int16_t)(y2);(p)->P[2].Y =(std::int16_t)(y3)
#define	setUV3(p,x1,y1,x2,y2,x3,y3,page)		(p)->P[0].TX   =(std::int16_t)(x1);(p)->P[1].TX   =(std::int16_t)(x2);(p)->P[2].TX    =(std::int16_t)(x3);(p)->P[0].TY=(std::int16_t)(y1);(p)->P[1].TY=(std::int16_t)(y2);(p)->P[2].TY=(std::int16_t)(y3);(p)->TextPage=(std::int16_t)(page)
#define	setShade3(p,s1,s2,s3)					(p)->P[0].Shade=(std::int16_t)(s1);(p)->P[1].Shade=(std::int16_t)(s2);(p)->P[2].Shade=(std::int16_t)(s3)

#define	setXY2(p,x1,y1,x2,y2)					(p)->P[0].X    =(std::int16_t)(x1);(p)->P[1].X    =(std::int16_t)(x2);(p)->P[0].Y =(std::int16_t)(y1);(p)->P[1].Y =(std::int16_t)(y2)
#define	setUV2(p,x1,y1,x2,y2)					(p)->P[0].TX   =(std::int16_t)(x1);(p)->P[1].TX   =(std::int16_t)(x2);(p)->P[0].TY=(std::int16_t)(y1);(p)->P[1].TY=(std::int16_t)(y2)
#define	setShade2(p,s1,s2)						(p)->P[0].Shade=(std::int16_t)(s1);(p)->P[1].Shade=(std::int16_t)(s2)
											
#define	setXY1(p,x1,y1)							(p)->P[0].X    =(std::int16_t)(x1);(p)->P[0].Y =(std::int16_t)(y1)

#define	setCol4(p,col)							(p)->Col=(std::int16_t)(col)
#define	setCol3(p,col)							(p)->Col=(std::int16_t)(col)
#define	setCol2(p,col)							(p)->Col=(std::int16_t)(col)

#define	setPoly50MGT4(p)			((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPoly50MGT3(p)			((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPoly50GT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPoly50GT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPoly50T4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_TEXTURED)
#define	setPoly50T3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_TEXTURED)
#define	setPoly50G4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD)
#define	setPoly50G3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD)
#define	setPoly50F4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_SEMI_TRANS)
#define	setPoly50F3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_SEMI_TRANS)

#define	setPolyAMT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	setPolyAMT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	setPolyAG4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_GOURAD)
#define	setPolyAG3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_GOURAD)
#define	setPolyAT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_TEXTURED)
#define	setPolyAT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_ALPHA|POLY_FLAG_TEXTURED)

#define	setPolyTGT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_TILED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPolyTGT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_TILED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)


#define	setPolyMGT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPolyMGT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPolyGT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPolyGT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	setPolyMT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	setPolyMT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	setPolyT4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_TEXTURED)
#define	setPolyT3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_TEXTURED)
#define	setPolyG4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(POLY_FLAG_GOURAD)
#define	setPolyG3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(POLY_FLAG_GOURAD)
#define	setPolyF4(p)				((struct BucketQuad*)p)->BucketType=BT_QUAD;((struct BucketQuad*)p)->DrawFlags=(0)
#define	setPolyF3(p)				((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(0)
#define	setPolyGVect(p)				((struct BucketVect*)p)->BucketType=BT_VECT;((struct BucketVect*)p)->DrawFlags=0

#define	setPolyType3(p,type)		((struct BucketTri*) p)->BucketType=BT_TRI ;((struct BucketTri*) p)->DrawFlags=(type)
#define	setPolyType4(p,type)		((struct BucketQuad*) p)->BucketType=BT_QUAD ;((struct BucketQuad*) p)->DrawFlags=(type)

#define	setRect(p)					((struct BucketQuad*)p)->BucketType=BT_RECT;((struct BucketQuad*)p)->DrawFlags=0


#define	POLY_50MGT					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	POLY_50MT					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_MASKED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	POLY_MGT					(POLY_FLAG_GOURAD|POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	POLY_MT						(POLY_FLAG_MASKED|POLY_FLAG_TEXTURED)
#define	POLY_NULL					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_MASKED)

#define	POLY_50F					(POLY_FLAG_SEMI_TRANS)
#define	POLY_50GT					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	POLY_50T					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_TEXTURED)
#define	POLY_50G					(POLY_FLAG_SEMI_TRANS|POLY_FLAG_GOURAD)
#define	POLY_TGT					(POLY_FLAG_TILED|POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	POLY_GT						(POLY_FLAG_GOURAD|POLY_FLAG_TEXTURED)
#define	POLY_TT						(POLY_FLAG_TILED|POLY_FLAG_TEXTURED)
#define	POLY_T						(POLY_FLAG_TEXTURED)
#define	POLY_G						(POLY_FLAG_GOURAD)
#define	POLY_F						(0)

