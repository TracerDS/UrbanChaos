#ifndef ANIM_H
#define ANIM_H 1

#include "Prim.h"

// define s
#define MAX_CREATURE_TYPES 20
#define MAX_ANIMS_PER_CREATURE 20
#define MAX_MATRIX_POOL 10000

#define MAX_PEOPLE_TYPES 20
#define MAX_BODY_BITS 18
#define MAX_BODY_VARIETY 10
#define PEOPLE_NAME_SIZE 15

#define ANIM_PERSON 1
#define ANIM_RHINO 2
#define ANIM_APE 3

// #define	ANIM_TYPE_WALK						1
// #define	ANIM_TYPE_RUN						2
// #define	ANIM_TYPE_JUMP						3

#define TWEEN_OFFSET_SHIFT 0

#define ANIM_FLAG_LAST_FRAME (1 << 0)

// structures

struct AnimItem {
    std::uint16_t Start;
    std::uint16_t End;
};

struct PrimMultiAnim {
    struct Matrix33 Mat;
    std::int32_t DX;
    std::int32_t DY;
    std::int32_t DZ;
    std::uint16_t Next;
};

struct BodyDef {
    std::uint8_t BodyPart[20]; // 1..14 is a normal person
};

// data
extern struct AnimItem anim_item[MAX_CREATURE_TYPES][MAX_ANIMS_PER_CREATURE];
extern struct PrimMultiAnim prim_multi_anims[]; // 500K

extern std::uint16_t next_prim_multi_anim;

// FUNCTIONS

extern std::int8_t read_a_multi_vue(std::int32_t m_object);
extern void animate_and_draw_chap();
extern void setup_people_anims();
extern void setup_extra_anims();
extern void setup_global_anim_array();

extern std::int32_t next_game_chunk;
extern std::int32_t next_anim_chunk;

// Guys stuff.

#ifdef PSX

#define MAX_NUMBER_OF_CHUNKS 5
#define MAX_NUMBER_OF_FRAMES 900
#define MAX_NUMBER_OF_ELEMENTS 13000

#else

#define MAX_NUMBER_OF_CHUNKS 5
#define MAX_NUMBER_OF_FRAMES 5000
#define MAX_NUMBER_OF_ELEMENTS 80000

#endif

//************************************
//  Game Stuff
//************************************

//
// This contains the prim objects compressed matrix and offset for each object of each keyframe
//
struct KeyFrameElement {
    struct CMatrix33 CMatrix;
    struct Matrix33 Matrix;
    std::int32_t OffsetX;
    std::int32_t OffsetY;
    std::int32_t OffsetZ;
    std::uint16_t Next;
    std::uint16_t Parent;
};

//************************************************************************************************
//************************************************************************************************
#ifdef PSX
#define ULTRA_COMPRESSED_ANIMATIONS
#endif

//************************************************************************************************

struct GameKeyFrameElementCompOld {
    std::int8_t m00, m01, m10, m11;
    //	std::int8_t				dm02, dm12, dm20, dm21, dm22;
    //	std::uint8_t				pad1,pad2,pad3;
    std::int16_t OffsetX;
    std::int16_t OffsetY;
    std::int16_t OffsetZ;
    std::uint16_t Pad;
};
struct GameKeyFrameElementComp {
    std::int8_t m00, m01, m10, m11;
    //	std::int8_t				dm02, dm12, dm20, dm21, dm22;
    //	std::uint8_t				pad1,pad2,pad3;
    std::int8_t OffsetX;
    std::int8_t OffsetY;
    std::int8_t OffsetZ;
    std::uint8_t Pad;
};

struct GameKeyFrameElementBig {
    struct CMatrix33 CMatrix;
    std::int16_t OffsetX;
    std::int16_t OffsetY;
    std::int16_t OffsetZ;
    std::uint16_t Pad;
};

#ifndef ULTRA_COMPRESSED_ANIMATIONS
struct GameKeyFrameElement {
    struct CMatrix33 CMatrix;
    std::int16_t OffsetX;
    std::int16_t OffsetY;
    std::int16_t OffsetZ;
    std::uint16_t Pad;
};

// inline	void	GetCMatrix(GameKeyFrameElement *e, CMatrix33 *c)
inline void GetCMatrix(GameKeyFrameElement* e, CMatrix33* c) {
    *c = e->CMatrix;
}

inline void SetCMatrix(GameKeyFrameElement* e, CMatrix33* c) {
    e->CMatrix = *c;
};

//************************************************************************************************
#else

// JCL - wow - 4 bytes for the matrix instead of 36 (well, 12..)

struct GameKeyFrameElement {
    std::int8_t m00, m01, m10, m11;
    //	std::int8_t				dm02, dm12, dm20, dm21, dm22;
    //	std::uint8_t				pad1,pad2,pad3;
    std::int8_t OffsetX;
    std::int8_t OffsetY;
    std::int8_t OffsetZ;
    std::uint8_t Pad;
};

void GetCMatrix(GameKeyFrameElement* e, CMatrix33* c);
void SetCMatrix(GameKeyFrameElement* e, CMatrix33* c);

#endif
//************************************************************************************************
//************************************************************************************************

struct GameFightCol {
    std::uint16_t Dist1;
    std::uint16_t Dist2;

    std::uint8_t Angle;
    std::uint8_t Priority;
    std::uint8_t Damage;
    std::uint8_t Tween;

    std::uint8_t AngleHitFrom;
    std::uint8_t Height;
    std::uint8_t Width;
    std::uint8_t Dummy;

    struct GameFightCol* Next;
};

//
// This Contains
//
struct GameKeyFrame {
    std::uint8_t XYZIndex;
    std::uint8_t TweenStep;
    std::int16_t Flags;
    GameKeyFrameElement* FirstElement;
    GameKeyFrame *PrevFrame, *NextFrame;
    GameFightCol* Fight;
};

/*
struct	GameKeyFrameChunk
{
        std::uint16_t						MultiObject[10];
        std::int32_t						ElementCount;
        struct	BodyDef				PeopleTypes[MAX_PEOPLE_TYPES];
        struct GameKeyFrame			AnimKeyFrames[20000];
        struct GameKeyFrame			*AnimList[200];
        struct GameKeyFrameElement	TheElements[MAX_NUMBER_OF_ELEMENTS];
        std::int32_t	MaxKeyFrames;
        std::int32_t	MaxElements;
};
*/

struct FightCol {
    std::uint16_t Dist1;
    std::uint16_t Dist2;

    std::uint8_t Angle;
    std::uint8_t Priority;
    std::uint8_t Damage;
    std::uint8_t Tween;

    std::uint8_t AngleHitFrom;
    std::uint8_t Height;
    std::uint8_t Width;
    std::uint8_t Dummy;

    std::uint32_t Dummy2;

    struct FightCol* Next;
};

struct GameKeyFrameChunk {
    std::uint16_t MultiObject[10];
    std::int32_t ElementCount;
    struct BodyDef* PeopleTypes;             //[MAX_PEOPLE_TYPES];
    struct GameKeyFrame* AnimKeyFrames;      //[MAX_NUMBER_OF_FRAMES];
    struct GameKeyFrame** AnimList;          //[200];
    struct GameKeyFrameElement* TheElements; //[MAX_NUMBER_OF_ELEMENTS];
    struct GameFightCol* FightCols;          //[200];

    std::int16_t MaxPeopleTypes;
    std::int16_t MaxKeyFrames;
    std::int16_t MaxAnimFrames;
    std::int16_t MaxFightCols;
    std::int32_t MaxElements;
};

struct KeyFrame {
    std::int16_t ChunkID;
    std::int16_t Flags;
    std::int32_t FrameID,
        TweenStep,
        ElementCount;
    KeyFrameElement* FirstElement;
    KeyFrame *PrevFrame,
        *NextFrame;
    std::int16_t Dx, Dy, Dz;
    std::int16_t Fixed;
    struct FightCol* Fight;
};

struct KeyFrameChunk {
    char ANMName[64],
        ASCName[64],
        VUEName[64];
    std::uint16_t MultiObject;
    std::uint16_t MultiObjectStart;
    std::uint16_t MultiObjectEnd;
    std::int32_t ElementCount;
    struct BodyDef PeopleTypes[MAX_PEOPLE_TYPES];
    char PeopleNames[MAX_PEOPLE_TYPES][PEOPLE_NAME_SIZE];
//	std::uint8_t				BodyBits[MAX_BODY_PARTS][MAX_BODY_VARIETY]; //10 types of each body part
#ifdef PSX
    KeyFrame KeyFrames[1];
#else
    KeyFrame KeyFrames[MAX_NUMBER_OF_FRAMES];
#endif
    KeyFrameElement* FirstElement;
    KeyFrameElement* LastElement;
    std::int32_t KeyFrameCount;
};

struct KeyFrameEdDefaults {
    std::int32_t Left,
        Top;
    std::int32_t Height,
        Width;
    //	KeyFrameChunk		KeyFrameChunks[MAX_NUMBER_OF_CHUNKS];
};

#define ANIM_NAME_SIZE 64
#define ANIM_LOOP 1

class Anim {
   private:
    char AnimName[ANIM_NAME_SIZE];
    std::uint32_t AnimFlags;
    std::int32_t FrameCount;
    Anim *LastAnim,
        *NextAnim;
    KeyFrame *CurrentFrame,
        *FrameListEnd,
        *FrameListStart;
    std::uint8_t TweakSpeed;

   public:
    Anim();
    ~Anim();
    void AddKeyFrame(KeyFrame* the_frame);
    void RemoveKeyFrame(KeyFrame* the_frame);
    inline void SetAnimName(char* string) { strcpy(AnimName, string); }
    inline char* GetAnimName() { return AnimName; }
    inline void SetCurrentFrame(KeyFrame* the_frame) { CurrentFrame = the_frame; }
    inline KeyFrame* GetFrameList() { return FrameListStart; }
    inline void SetFrameList(KeyFrame* frame_list) { FrameListStart = frame_list; }
    inline KeyFrame* GetFrameListEnd() { return FrameListEnd; }
    inline KeyFrame* GetFrameListStart() { return FrameListStart; }
    inline void SetFrameListEnd(KeyFrame* frame_list_end) { FrameListEnd = frame_list_end; }
    inline std::int32_t GetFrameCount() { return FrameCount; }
    inline void SetFrameCount(std::int32_t count) { FrameCount = count; }
    inline std::uint32_t GetAnimFlags() { return AnimFlags; }
    inline void SetAnimFlags(std::uint32_t flags) { AnimFlags = flags; }
    void SetAllTweens(std::int32_t tween);
    void StartLooping();
    void StopLooping();
    inline void SetNextAnim(Anim* next) { NextAnim = next; }
    inline Anim* GetNextAnim() { return NextAnim; }
    inline void SetLastAnim(Anim* last) { LastAnim = last; }
    inline Anim* GetLastAnim() { return LastAnim; }
    inline std::uint8_t GetTweakSpeed() { return TweakSpeed; }
    inline void SetTweakSpeed(std::uint8_t speed) { TweakSpeed = speed; }
};

class Character {
   private:
    char CharName[32];
    std::uint16_t MultiObject;
#ifdef TARGET_DC
    // OI! This needs to be aligned on the DC, but it's probably a
    // Good Entity to align it on the PC as well.
    std::uint16_t wJunk;
#endif
    Anim AnimList[50];

   public:
    void AddAnim(Anim* the_anim);
    void RemoveAnim(Anim* the_anim);
    inline char* GetCharName() {
        return CharName;
    }
    inline void SetCharName(char* string) {
        strcpy(CharName, string);
    }
    inline std::uint16_t GetMultiObject() {
        return MultiObject;
    }
    inline void SetMultiObject(std::uint16_t multi) {
        MultiObject = multi;
    }
};

//
// Anim initailisation stuff.
//

void ANIM_init();
void ANIM_fini();

#endif
