/************************************************************
 *
 *   flamengine.h
 *   2D flame (and other fx) engine
 *
 */

#ifndef _FLAMENGINE_
#define _FLAMENGINE_

#include <MFStdLib.h>

struct FlameXY2 {
    std::uint8_t x, y;
};

union FlameXY {
    FlameXY2 loc;
    std::uint16_t ofs;
};

#ifdef TARGET_DC
// Properly aligned!
struct FlameParticle {
    FlameXY pos;
    std::uint8_t jx, jy;  // Jitter range
    std::uint8_t ex, ey;  // Emitter position
    std::uint8_t life;    // could be ttl but is just on/off flag
    std::uint8_t prate;   // speed of pulsing
    std::uint8_t pmode;   // kind of pulse (ramp/cycle)
    std::uint8_t wmode;   // walk mode
    std::int16_t pulse;   // particles pulsing in and out
    std::uint32_t pstart; // start of pulse range;
    std::uint32_t pend;   // end of pulse range;
};
#else
struct FlameParticle {
    FlameXY pos;
    std::uint8_t jx, jy;  // Jitter range
    std::uint8_t ex, ey;  // Emitter position
    std::uint8_t life;    // could be ttl but is just on/off flag
    std::int16_t pulse;   // particles pulsing in and out
    std::uint8_t prate;   // speed of pulsing
    std::uint8_t pmode;   // kind of pulse (ramp/cycle)
    std::uint8_t wmode;   // walk mode
    std::uint32_t pstart; // start of pulse range;
    std::uint32_t pend;   // end of pulse range;
};
#endif

struct FlameParams {
    std::uint8_t blur, dark, convec;
    std::uint8_t palette[768];
    std::uint16_t free, posn;
    std::uint8_t randomize; // extra random stuff for flames
    FlameParticle particles[2000];
};

class Flamengine {
   private:
    std::uint8_t data[256 * 256 * 2];
    std::uint8_t work[256 * 256 * 2];
    void AddParticles();    //   Sparkly bits
    void AddParticles2();   //   Sparkly bits
    void Feedback();        //   Funky feedback; must be used BEFORE any other screen draw
    void ConvectionBlur();  //   Straightforward blur box filter but offset one line
    void ConvectionBlur2(); //   H@X0R'd version
    void Darkening();       //   Moves and/or darkens the image
    void UpdateTexture();   //   Locks, pokes, unlocks and updates a texture with the fire data
    void ReadHeader(MFFileHandle handle);
    void ReadParts(MFFileHandle handle);
    void BlitOffset(); //   Blit the texture, skewed for feedback, onto the backbuffer
   public:
    FlameParams params;
    Flamengine(char *name);
    ~Flamengine();
    void Run();               //	 Update the animation one frame
    void Blit();              //   Blit the texture, suitably alpha'd, onto the backbuffer
    void BlitHalf(char side); //   Same, but half only (for seperate left + right)
};

#endif