//
// playcuts.h
// matthew rosenfeld 30 june 99
//
// plays back cutscenes made with cutscene.cpp in gedit
//

#ifndef _PLAYCUTS_H_
#define _PLAYCUTS_H_

#include "MFStdLib.h"
#include "Game.h"

//----------------------------------------------------------------------------
// DEFINES
//

#ifdef TARGET_DC
#define MAX_CUTSCENES (1)
#define MAX_CUTSCENE_TRACKS (1)
#define MAX_CUTSCENE_PACKETS (1)
#define MAX_CUTSCENE_TEXT (1)
#else
#define MAX_CUTSCENES (20)
#define MAX_CUTSCENE_TRACKS (20 * 15)
#define MAX_CUTSCENE_PACKETS (20 * 15 * 128)
#define MAX_CUTSCENE_TEXT (4096)
#endif

//----------------------------------------------------------------------------
// STRUCTS
//

struct CPData;
struct CPChannel;
struct CPPacket;

struct CPData {
    std::uint8_t version;
    std::uint8_t channelcount;
    std::uint8_t pad1, pad2;
    CPChannel *channels;
};

struct CPChannel {
    std::uint8_t type;  // 0=unused, 1=character, 2=camera, 3=spot sound, 4=vfx
    std::uint8_t flags; // come up with some later... :P
    std::uint8_t pad1, pad2;
    std::uint16_t index;       // of the sound/character or the type of fx
    std::uint16_t packetcount; //
    CPPacket *packets;         //
};

struct CPPacket {
    std::uint8_t type;          // 0=unused, 1=animation, 2=action, 3=sound, 4=camerarec		0
    std::uint8_t flags;         // come up with some later... :P
    std::uint16_t index;        // of animation, sound etc										4
    std::uint16_t start;        // time of packet start
    std::uint16_t length;       // natural packet length										8
    GameCoord pos;              // location														20
    std::uint16_t angle, pitch; // no roll :P
};

//----------------------------------------------------------------------------
// EXTERNS (for the benefit of others)
//

extern std::uint16_t PLAYCUTS_cutscene_ctr;
extern std::uint16_t PLAYCUTS_track_ctr;
extern std::uint16_t PLAYCUTS_packet_ctr;
extern std::uint16_t PLAYCUTS_text_ctr;
#ifndef PSX
extern CPData PLAYCUTS_cutscenes[MAX_CUTSCENES];
extern CPPacket PLAYCUTS_packets[MAX_CUTSCENE_PACKETS];
extern CPChannel PLAYCUTS_tracks[MAX_CUTSCENE_TRACKS];
extern char PLAYCUTS_text_data[MAX_CUTSCENE_TEXT];
#else
extern CPData *PLAYCUTS_cutscenes;
extern CPPacket *PLAYCUTS_packets;
extern CPChannel *PLAYCUTS_tracks;
extern char *PLAYCUTS_text_data;
#endif

//----------------------------------------------------------------------------
// FUNCTION PROTOYPES
//

CPData *PLAYCUTS_Read(MFFileHandle handle);
void PLAYCUTS_Free(CPData *cutscene);
void PLAYCUTS_Play(CPData *cutscene);
void PLAYCUTS_Reset();

#endif