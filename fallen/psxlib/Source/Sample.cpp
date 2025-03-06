// SampleManager.cpp
// Guy Simmons, 24th February 1998.

// Windows multimedia stuff, used for WAV's.

//---------------------------------------------------------------
//
//	User functions.
//
//---------------------------------------------------------------
#include <MFStdLib.h>

void LoadSampleList(char* sample_file) {
}

//---------------------------------------------------------------

void PlaySample(std::int32_t ref, std::int16_t sample_no, std::int32_t vol, std::int32_t pan, std::int32_t freq, std::int32_t pri) {
}

//---------------------------------------------------------------

void StopSample(std::int32_t user_ref) {
}

void PlayWave(std::int32_t ref, std::int32_t wave_id, std::int32_t play_type, WaveParams* the_params) {
    //	the_qs_sound_manager.PlayWave(ref,wave_id,play_type,the_params);
}

//---------------------------------------------------------------

void StopWave(std::int32_t ref, std::int32_t wave_id) {
    //	the_qs_sound_manager.StopWave(ref,wave_id);
}

void LoadWaveList(char* wave_path, char* wave_list) {
}

//---------------------------------------------------------------

void LoadWave(char* wave_name) {
}

//---------------------------------------------------------------

void FreeWaveList() {
}

void SetListenerPosition(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t scale) {
}

void SetListenerOrientation(std::int32_t angle, std::int32_t roll, std::int32_t tilt) {
}
