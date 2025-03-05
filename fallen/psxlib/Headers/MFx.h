//
// MFx.h
//
// Muckyfoot sound fx api for A3D / PSX
//
// (same header, different cpp)
//
#ifndef _mfx_h_
#define _mfx_h_

#include "MFStdLib.h"
#include "c:\fallen\headers\structs.h"
#include "c:\fallen\headers\thing.h"

#define MFX_LOOPED (1)        // loop the wave infinitely
#define MFX_MOVING (2)        // update the source's coords automatically
#define MFX_REPLACE (4)       // play, replacing whatever's there and blatting the queue
#define MFX_OVERLAP (8)       // allocate a new channel if current is occupied
#define MFX_QUEUED (16)       // queue this up for later
#define MFX_CAMERA (32)       // attach to camera
#define MFX_HI_PRIORITY (64)  // these are for A3d not for us as they...
#define MFX_LO_PRIORITY (128) // ...determine priority for funky HW channels
#define MFX_LOCKY (256)       // don't adjust Y coord when cam-ing or move-ing

#define MFX_SHOT (512)
#define MFX_EXPLODE (1024)
#define MFX_PLAYER (2048)
#define MFX_EARLY_OUT (4096)
#define MFX_SHORT_QUEUE (8192)
#define MFX_NEVER_OVERLAP (16384)

#define MFX_ENV_NONE (0)
#define MFX_ENV_ALLEY (1)
#define MFX_ENV_SEWER (2)
#define MFX_ENV_STADIUM (3)

#define MFX_CHANNEL_ALL (0x010000)
#define MFX_WAVE_ALL (0x010000)

#define MFX_PAIRED_TRK1 (1)
#define MFX_PAIRED_TRK2 (2)

//----- transport functions -----

void MFX_play_xyz(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags, std::int32_t x, std::int32_t y, std::int32_t z);
void MFX_play_pos(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags, GameCoord* position);
void MFX_play_thing(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags, Thing* p);
void MFX_play_ambient(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags);
void MFX_play_stereo(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags);

void MFX_stop(std::int32_t channel_id, std::uint32_t wave);
void MFX_stop_attached(Thing* p);

//----- audio processing functions -----

void MFX_set_pitch(std::uint16_t channel_id, std::uint32_t wave, std::int32_t pitchbend);
void MFX_set_wave(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t new_wave);
void MFX_set_xyz(std::uint16_t channel_id, std::uint32_t wave, std::int32_t x, std::int32_t y, std::int32_t z);
void MFX_set_pos(std::uint16_t channel_id, std::uint32_t wave, GameCoord* position);
void MFX_set_gain(std::uint16_t channel_id, std::uint32_t wave, std::uint8_t gain);

//----- listener & environment -----

void MFX_set_listener(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t heading, std::int32_t roll, std::int32_t pitch);

void MFX_set_environment(std::int32_t env_type);

//----- sound library functions -----

void MFX_load_wave_list(char* path, char* script_file);
void MFX_load_wave_list(char* names[] = 0); // load list from array
void MFX_load_wave_file(char* wave_file);
void MFX_free_wave_list();

//----- general system stuff -----

void MFX_render();

//----- Here this must go, because nowhere else would be right for it.

void MUSIC_init_level(std::int32_t world);

void MFX_Conv_wait();
std::int32_t MFX_Conv_play(std::int32_t waypoint, std::int32_t conv, std::int32_t conv_off);
void MFX_Init_Speech(std::int32_t level);

#endif
