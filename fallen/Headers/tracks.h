//
// tracks.h
// tyre tracks, footprints...
// 22 sept 98
//

#ifndef _TRACKS_H_
#define _TRACKS_H_

#include "MFStdLib.h"
#include "Structs.h"
#include "game.h"

// this is how many track slots are available on the given system
// using BUILD_PSX means that it'll be 50 both on the PSX, *AND* when Mike builds PSX
// nads on his PC.

#ifndef BUILD_PSX
#define TRACK_BUFFER_LENGTH 300
#else
#define TRACK_BUFFER_LENGTH 50
#endif

#define TRACK_SURFACE_NONE 0
#define TRACK_SURFACE_MUDDY 1
#define TRACK_SURFACE_WATER 2
#define TRACK_SURFACE_ONSNOW 3

#define TRACK_TYPE_TYRE 1
#define TRACK_TYPE_TYRE_SKID 2
#define TRACK_TYPE_LEFT_PRINT 3
#define TRACK_TYPE_RIGHT_PRINT 4

#define TRACK_FLAGS_FLIPABLE 1
#define TRACK_FLAGS_SPLUTTING 2
#define TRACK_FLAGS_INVALPHA 4

struct Track {
    //	std::int32_t	x,y,z; // not required -- here to debug...

    std::int32_t dx, dy, dz;
    std::int32_t page, colour;
    THING_INDEX thing; // Entity*	thing; //miked did this
    std::int16_t sx, sz;
    std::uint16_t padtolong;
    std::uint8_t flip;
    std::uint8_t flags;
    std::uint8_t splut;
    std::uint8_t splutmax;
};

typedef Track *TrackPtr;

extern Track *tracks; //[TRACK_BUFFER_LENGTH];
extern std::uint16_t track_head, track_tail, track_eob;

#define TO_TRACK(x) (&tracks[x])
#define TRACK_NUMBER(x) (std::uint16_t) (x - TO_TRACK(0))

void TRACKS_InitOnce(std::int16_t size = TRACK_BUFFER_LENGTH);
void TRACKS_Reset(std::int16_t size = TRACK_BUFFER_LENGTH);
void TRACKS_Draw();
void TRACKS_DrawTrack(Entity *p_thing);

// Figure out the offsets given the width
void TRACKS_CalcDiffs(Track &track, std::uint8_t width);

// Add a track unit supplying exact parameters one by one
void TRACKS_AddQuad(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz, std::int32_t page, std::int32_t colour, std::uint8_t width, std::uint8_t flip, std::uint8_t flags);

// Add a track unit supplying a completed track entry
void TRACKS_AddTrack(Track &track);

// Add a track unit "intelligently" supplying coordinates and a type of track
std::uint16_t TRACKS_Add(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz, std::uint8_t type, std::uint16_t last);

// What grounds is at XZ?
std::int32_t TRACKS_GroundAtXZ(std::int32_t X, std::int32_t Z);

// Make something bleed
void TRACKS_Bleed(Entity *bleeder);
void TRACKS_Bloodpool(Entity *bleeder);

#endif