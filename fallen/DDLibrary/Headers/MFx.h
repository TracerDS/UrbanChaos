#ifndef _mfx_h_
#define _mfx_h_

#include "MFStdLib.h"
#include "structs.h"
#include "thing.h"

#define MFX_LOOPED (1)           // loop the wave infinitely
#define MFX_MOVING (2)           // update the source's coords automatically
#define MFX_REPLACE (4)          // play, replacing whatever's there and blatting the queue
#define MFX_OVERLAP (8)          // allocate a new channel if current is occupied
#define MFX_QUEUED (16)          // queue this up for later
#define MFX_CAMERA (32)          // attach to camera
#define MFX_HI_PRIORITY (64)     // these are for A3d not for us as they...
#define MFX_LO_PRIORITY (128)    // ...determine priority for funky HW channels
#define MFX_LOCKY (256)          // don't adjust Y coord when cam-ing or move-ing
#define MFX_SHORT_QUEUE (512)    // for sync-ing music -- queue for later, but replace the top of the queue
#define MFX_PAIRED_TRK1 (1024)   // when this chan starts playing, trigger the next chan in sync
#define MFX_PAIRED_TRK2 (2048)   // don't start this wave until the previous chan triggers it
#define MFX_EARLY_OUT (4096)     // mostly for music, 'breaks out' early from the wave, also works on loops
#define MFX_NEVER_OVERLAP (8192) // if anything at all playing on the channel, leave it

#define MFX_ENV_NONE (0)
#define MFX_ENV_ALLEY (1)
#define MFX_ENV_SEWER (2)
#define MFX_ENV_STADIUM (3)

#define MFX_CHANNEL_ALL (0x010000)
#define MFX_WAVE_ALL (0x010000)

//----- volume functions

void MFX_get_volumes(std::int32_t* fx, std::int32_t* amb, std::int32_t* mus); // all 0 to 127
void MFX_set_volumes(std::int32_t fx, std::int32_t amb, std::int32_t mus);

//----- playback functions -----

void MFX_play_xyz(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags, std::int32_t x, std::int32_t y, std::int32_t z);
void MFX_play_thing(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags, Entity* p);
void MFX_play_ambient(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags);
std::uint8_t MFX_play_stereo(std::uint16_t channel_id, std::uint32_t wave, std::uint32_t flags);

void MFX_stop(std::int32_t channel_id, std::uint32_t wave);
void MFX_stop_attached(Entity* p);

//----- audio processing functions -----

void MFX_set_pitch(std::uint16_t channel_id, std::uint32_t wave, std::int32_t pitchbend);
void MFX_set_gain(std::uint16_t channel_id, std::uint32_t wave, std::uint8_t gain);
void MFX_set_queue_gain(std::uint16_t channel_id, std::uint32_t wave, std::uint8_t gain);

//----- listener -----

void MFX_set_listener(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t heading, std::int32_t roll, std::int32_t pitch);

//----- sound library functions -----

void MFX_load_wave_list();
void MFX_free_wave_list();

//----- querying information back -----

std::uint16_t MFX_get_wave(std::uint16_t channel_id, std::uint8_t index = 0);

//----- general system stuff -----

void MFX_update();

//----- init stuff -----

void MFX_init();
void MFX_term();

// Mikes bodge stuff to get conversation in

std::int32_t MFX_QUICK_play(char* str, std::int32_t x = 0, std::int32_t y = 0, std::int32_t z = 0); // should be low-res (24.8) coordinates
void MFX_QUICK_wait();
void MFX_QUICK_stop();
std::int32_t MFX_QUICK_still_playing();

#endif
