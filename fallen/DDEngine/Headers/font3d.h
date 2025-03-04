
// THIS IS PANTS!

#if 0

/************************************************************
 *
 *   font3d.h
 *   3D text writer
 *
 */

#pragma once

#include <MFStdLib.h>

struct FontVec {
	float x,y,z;       // vertex pos
	float tx,ty,tz;    // translated vertex pos
	float nx,ny,nz;    // normal pos
	float tnx,tny,tnz; // translated normal pos
};

struct FontFace {
	FontVec *a, *b, *c, norm;
};

struct FontData {
	std::uint32_t     numpts, numfaces;
    FontVec  *pts;
	FontFace *faces;
	std::uint16_t     width;
};

class Font3D {
  private:
    FontData data[100];
	std::uint32_t    nextchar;
	std::uint32_t	 LetterWidth(char chr);
	float	 fontscale;
  public:
    void ClearLetters();
	void AddLetter(char* fn);
	void DrawLetter(char chr, std::uint32_t x, std::uint32_t y, std::uint32_t rgb=0xffffff, float yaw=0, float roll=0, float pitch=0, float scale=3.5);
	void DrawString(char* str, std::uint32_t x, std::uint32_t y, std::uint32_t rgb=0xffffff, float scale=3.5, char wibble=0, std::uint16_t zoom=0);
	Font3D(char *path, float scale=1.0);
	~Font3D();
};

#endif