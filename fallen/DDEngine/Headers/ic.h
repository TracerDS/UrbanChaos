//
// Image compression.
//

#ifndef _IC_
#define _IC_

#include "tga.h"


//
// Compresses the given 4x4 block of a TGA using the mad S3 texture compression
// system.  Ignores the alpha channel of course!
//

typedef struct
{
	std::uint16_t colour1;	// 5:6:5
	std::uint16_t colour2;	// 5:6:5
	std::uint32_t bit;

} IC_Packet;

IC_Packet IC_pack(
			TGA_Pixel *tga, 
			std::int32_t      tga_width,
			std::int32_t      tga_height,
			std::int32_t      px,
			std::int32_t      py);


//
// Fills in a 4x4 block of the given tga with the colour data
// from a compressed IC packet.
//

void IC_unpack(
			IC_Packet  ip,
			TGA_Pixel *tga, 
			std::int32_t      tga_width,
			std::int32_t      tga_height,
			std::int32_t      px,
			std::int32_t      py);



//
// Debug test...	
//

void IC_test();


#endif
