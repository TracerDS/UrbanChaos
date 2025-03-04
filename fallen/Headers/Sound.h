//	Sound.h
//	Guy Simmons, 8th May 1998.

#pragma once

#include "Structs.h"
#include "MFX.h"

#define USE_A3D

// because the versions in MFStdLib.h are misspelt...
#define	WAVE_PLAY_INTERRUPT		0
#define	WAVE_PLAY_NO_INTERRUPT	1
#define	WAVE_PLAY_INTERRUPT_LOOPS 4

#define	WORLD_TYPE_CITY_POP		1
#define	WORLD_TYPE_CITY_UNPOP	2
#define	WORLD_TYPE_FOREST		3
#define	WORLD_TYPE_SNOW			4
#define	WORLD_TYPE_DESERT		5

extern std::int16_t	world_type;

//---------------------------------------------------------------
/*
void play_quicker_wave(std::int32_t sample);
void play_quick_wave(Thing *p_thing,std::int32_t sample,std::int32_t mode);
std::int32_t play_quick_wave_xyz(std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t sample,std::int32_t id,std::int32_t mode);
std::int32_t play_quick_wave_old(WaveParams *wave,std::int32_t sample,std::int32_t id,std::int32_t mode);
std::int32_t play_ambient_wave(std::int32_t sample,std::int32_t id,std::int32_t mode,std::int32_t range=256, std::uint8_t flags=0);
std::int32_t play_object_wave(std::int32_t channel, Thing *p_thing,std::int32_t sample,std::int32_t mode);
void wave_move(std::int32_t channel, std::int32_t x, std::int32_t y, std::int32_t z);
std::int32_t SOUND_query_current_wave(std::int32_t id);
bool SOUND_query_looped(std::int32_t id);*/
void play_glue_wave(std::uint16_t type, std::uint16_t id, std::int32_t x=0, std::int32_t y=0, std::int32_t z=0);
void process_ambient_effects();
void process_weather();

//void	NewLoadWaveList(char* names[]=0);

void SOUND_reset();
void SOUND_SewerPrecalc();
void SOUND_InitFXGroups(char* fn);

void PainSound(Thing *p_thing);
void EffortSound(Thing *p_thing);
void MinorEffortSound(Thing *p_thing);
void ScreamFallSound(Thing *p_thing);
void StopScreamFallSound(Thing *p_thing);
void SOUND_Curious(Thing *p_thing);
std::uint8_t SOUND_Gender(Thing *p_thing);


//std::int32_t	SOUND_Range(std::int32_t start, std::int32_t end);

#ifndef PSX

#ifdef DODGYPSXIFY
extern bool dodgy_psx_mode;
#endif

inline std::int32_t SOUND_Range(std::int32_t start, std::int32_t end) {
#ifdef DODGYPSXIFY
	if (dodgy_psx_mode) return start;
#endif
	std::int32_t diff=(end-start)+1;
	return start+(rand()%diff);
}

#else
#define SOUND_Range(start,end) (start)
#endif


#ifndef PSX

typedef std::uint16_t	SOUNDFXG[2];

extern std::uint8_t *SOUND_FXMapping;//[1024]; // blahblah
extern SOUNDFXG *SOUND_FXGroups;//[128][2]; // blahblah
#else
extern std::uint8_t SOUND_FXMapping[512]; // blahblah
extern std::uint16_t SOUND_FXGroups[8][2]; // blahblah
#endif

//---------------------------------------------------------------
// pull in the automatically-generated sound header
#include "sound_id.h"

//---------------------------------------------------------------

#define	WIND_REF			MAX_THINGS+100
#define	WEATHER_REF			(WIND_REF+1)
#define	THUNDER_REF			(WIND_REF+2)
#define	SIREN_REF			(WIND_REF+3)
#define	AMBIENT_EFFECT_REF	(WIND_REF+4)
#define MUSIC_REF			(WIND_REF+5)

//---------------------------------------------------------------

#ifdef USE_A3D

//extern void	A3DLoadWaveList(char* path,char* file);
//extern void	A3DFreeWaveList();
//extern void A3DRender();
extern void A3D_Check_Init();

#endif

