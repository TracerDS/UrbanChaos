//	QSManager.h
//	Guy Simmons, 6th May 1998.

#ifndef QSMANAGER_H
#define QSMANAGER_H

//---------------------------------------------------------------

// #include "qmixer.h"
#include "qmdx.h"

#if defined(QMDX_H)
#define QS(name) QMDX_##name
#ifndef TARGET_DC
#pragma comment(lib, "qmdx.lib")
#endif
#else
#define QS(name) QSWaveMix##name
#ifndef TARGET_DC
#pragma comment(lib, "Qmixer.lib")
#endif
#endif

//---------------------------------------------------------------

#define QS_WAVE_VALID (1 << 0)
#define QS_WAVE_LOADED (1 << 1)
#define QS_WAVE_STREAMED (1 << 2)

//---------------------------------------------------------------

class Wave {
   private:
    std::uint32_t WaveFlags;
    LPMIXWAVE MixWave;

   public:
    Wave *Next,
        *Prev;

    Wave();
    ~Wave();

    //	Methods.
    HRESULT Init(char *file_name, HQMIXER the_mixer);
    HRESULT Fini(HQMIXER the_mixer);
    HRESULT Load();
    HRESULT Free();

    inline bool IsValid() { return WaveFlags & QS_WAVE_VALID; }
    inline void ValidOn() { WaveFlags |= QS_WAVE_VALID; }
    inline void ValidOff() { WaveFlags &= ~QS_WAVE_VALID; }

    inline bool IsLoaded() { return WaveFlags & QS_WAVE_LOADED; }
    inline void LoadedOn() { WaveFlags |= QS_WAVE_LOADED; }
    inline void LoadedOff() { WaveFlags &= ~QS_WAVE_LOADED; }

    inline bool IsStreamed() { return WaveFlags & QS_WAVE_STREAMED; }
    inline void StreamedOn() { WaveFlags |= QS_WAVE_STREAMED; }
    inline void StreamedOff() { WaveFlags &= ~QS_WAVE_STREAMED; }

    inline LPMIXWAVE GetMixWave() { return MixWave; }
};

//---------------------------------------------------------------

#define QS_CHANNEL_OPEN (1 << 0)

//---------------------------------------------------------------

class Channel {
   private:
    int IChannel;
    std::uint32_t ChannelFlags,
        ChannelPriority;
    std::int32_t ChannelUserRef,
        ChannelWaveID;

   public:
    Channel();
    ~Channel();

    //	Methods.
    void Open();
    void Close();

    inline std::int32_t GetUserRef() { return ChannelUserRef; }
    inline void SetUserRef(std::int32_t ref) { ChannelUserRef = ref; }
    inline std::int32_t GetPriority() { return ChannelPriority; }
    inline void SetPriority(std::int32_t pri) { ChannelPriority = pri; }
    inline std::int32_t GetWaveID() { return ChannelWaveID; }
    inline void SetWaveID(std::int32_t id) { ChannelWaveID = id; }
};

//---------------------------------------------------------------

#define QS_MANAGER_INIT (1 << 0)
#define QS_MANAGER_ACTIVE (1 << 1)

#define MAX_CHANNELS 16

//---------------------------------------------------------------

class QSManager {
   private:
    std::uint32_t ManagerFlags,
        WaveCount;
    Channel Channels[MAX_CHANNELS];
    HQMIXER HQMixer;
    QMIXCONFIG Config;
    Wave *WaveList,
        *WaveListEnd;

   public:
    QSManager();
    ~QSManager();

    // Methods.
    HRESULT Init();
    HRESULT Fini();
    void ActivateSound();
    void DeactivateSound();

    HRESULT LoadWaves(char *wave_path, char *script_name);
    HRESULT LoadWave(char *wave_name);
    HRESULT FreeWaves();
    HRESULT AddWave(Wave *the_wave);
    HRESULT DeleteWave(Wave *the_wave);

    HRESULT PlayWave(std::int32_t wave_ref, std::int32_t wave_id, std::int32_t play_type, WaveParams *the_params);
    HRESULT StopWave(std::int32_t wave_ref, std::int32_t wave_id);

    inline bool IsInitialised() { return ManagerFlags & QS_MANAGER_INIT; }
    inline void InitOn() { ManagerFlags |= QS_MANAGER_INIT; }
    inline void InitOff() { ManagerFlags &= ~QS_MANAGER_INIT; }

    inline bool IsActive() { return ManagerFlags & QS_MANAGER_ACTIVE; }
    inline void ActiveOn() { ManagerFlags |= QS_MANAGER_ACTIVE; }
    inline void ActiveOff() { ManagerFlags &= ~QS_MANAGER_ACTIVE; }

    inline HQMIXER GetHQMixer() { return HQMixer; }
};

//---------------------------------------------------------------

extern QSManager the_qs_sound_manager;

//---------------------------------------------------------------

#endif
