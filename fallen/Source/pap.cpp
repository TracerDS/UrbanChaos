//
// A lower memory map: 100k
//

#include <MFStdLib.h>
#include "pap.h"
#include "game.h"
#include "mav.h"
#ifndef PSX
#include "..\ddengine\headers\aeng.h"
#else
#include "c:\fallen\psxeng\headers\engine.h"

#endif
#include "inside2.h"
#include "ns.h"
#include "ware.h"

#include "memory.h"

//
// The maps.
//

MEM_PAP_Lo *PAP_lo; //[PAP_SIZE_LO][PAP_SIZE_LO];
MEM_PAP_Hi *PAP_hi; //[PAP_SIZE_HI][PAP_SIZE_HI];

void PAP_clear() {
    memset((std::uint8_t *) &PAP_lo[0][0], 0, sizeof(PAP_Lo) * PAP_SIZE_LO * PAP_SIZE_LO);
    memset((std::uint8_t *) &PAP_hi[0][0], 0, sizeof(PAP_Hi) * PAP_SIZE_HI * PAP_SIZE_HI);
}

#ifndef PSX

//
// A couple of debug functions.
//

std::int32_t PAP_on_map_lo(std::int32_t x, std::int32_t z) {
    if (WITHIN(x, 0, PAP_SIZE_LO - 1) &&
        WITHIN(z, 0, PAP_SIZE_LO - 1)) {
        return true;
    } else {
        return false;
    }
}

std::int32_t PAP_on_map_hi(std::int32_t x, std::int32_t z) {
    if (WITHIN(x, 0, PAP_SIZE_HI - 1) &&
        WITHIN(z, 0, PAP_SIZE_HI - 1)) {
        return true;
    } else {
        return false;
    }
}

void PAP_assert_if_off_map_lo(std::int32_t x, std::int32_t z) {
    ASSERT(PAP_on_map_lo(x, z));
}

void PAP_assert_if_off_map_hi(std::int32_t x, std::int32_t z) {
    ASSERT(PAP_on_map_hi(x, z));
}

#endif // PSX

std::int32_t PAP_calc_height_at_point(std::int32_t map_x, std::int32_t map_z) {
    if (!WITHIN(map_x, 0, PAP_SIZE_HI - 1) ||
        !WITHIN(map_z, 0, PAP_SIZE_HI - 1)) {
        return 0;
    } else {
        return PAP_2HI(map_x, map_z).Alt << PAP_ALT_SHIFT;
    }
}

std::int32_t PAP_calc_height_at(std::int32_t x, std::int32_t z) {
    std::int32_t h0;
    std::int32_t h1;
    std::int32_t h2;
    std::int32_t h3;

    std::int32_t xfrac;
    std::int32_t zfrac;

    std::int32_t answer;

    PAP_Hi *ph;

    if (GAME_FLAGS & GF_NO_FLOOR) {
        return -32767;
    }

    std::int32_t mx = x >> PAP_SHIFT_HI;
    std::int32_t mz = z >> PAP_SHIFT_HI;

    if (mx < 0 || mx > PAP_SIZE_HI - 2 ||
        mz < 0 || mz > PAP_SIZE_HI - 2) {
        return 0;
    }

    ph = &PAP_2HI(mx, mz);

    //	if(ph->Flags&PAP_FLAG_ROOF_EXISTS)
    //		return(MAVHEIGHT(mx,mz)<<6);

    h0 = ph[0].Alt;
    h1 = ph[1].Alt;
    h2 = ph[PAP_SIZE_HI + 0].Alt;
    h3 = ph[PAP_SIZE_HI + 1].Alt;

    if (h0 == h1 && h1 == h2 && h2 == h3) {
        //
        // No need to do any interpolation.
        //

        answer = h0 << PAP_ALT_SHIFT;
    } else {
        h0 <<= PAP_ALT_SHIFT;
        h1 <<= PAP_ALT_SHIFT;
        h2 <<= PAP_ALT_SHIFT;
        h3 <<= PAP_ALT_SHIFT;

        xfrac = x & 0xff;
        zfrac = z & 0xff;

        if (xfrac + zfrac < 0x100) {
            answer = h0;
            answer += (h2 - h0) * xfrac >> 8;
            answer += (h1 - h0) * zfrac >> 8;
        } else {
            answer = h3;
            answer += (h1 - h3) * (0x100 - xfrac) >> 8;
            answer += (h2 - h3) * (0x100 - zfrac) >> 8;
        }
    }

    //
    // Modifiers.
    //

    if (ph->Flags & PAP_FLAG_SINK_SQUARE) {
        answer -= KERB_HEIGHTI;
    }

    return answer;
}

//
// Things sometimes like to think the map is at a strange height
//
std::int32_t PAP_calc_height_at_thing(Thing *p_thing, std::int32_t x, std::int32_t z) {
    switch (p_thing->Class) {
    case CLASS_PERSON:

        if (p_thing->Genus.Person->Flags & FLAG_PERSON_WAREHOUSE) {
            return WARE_calc_height_at(
                p_thing->Genus.Person->Ware,
                p_thing->WorldPos.X >> 8,
                p_thing->WorldPos.Z >> 8);
        }
#if 0
			else
			if(p_thing->Genus.Person->InsideIndex)
			{
				if(find_inside_flags(p_thing->Genus.Person->InsideIndex,x>>8,z>>8)& FLAG_INSIDE_STAIR)
				{
					std::int32_t	res,y1;
					std::uint16_t	new_floor;

					res=find_stair_y(p_thing,&y1,x,p_thing->WorldPos.Y>>8,z,&new_floor);
					if(res)
					{
						MSG_add(" inside on stairs at y %d \n",y1);
						return(y1);
					}
					else
						MSG_add(" inside at y %d \n",y1);


				}

				return(get_inside_alt(p_thing->Genus.Person->InsideIndex));
			}
			else
			if (p_thing->Flags & FLAGS_IN_SEWERS)
			{
				return(NS_calc_height_at(x,z)); //p_thing->WorldPos.X>>8,p_thing->WorldPos.Z>>8));
			}
#endif
        break;
    }
    return (PAP_calc_map_height_at(x, z));
}

std::int32_t PAP_calc_map_height_at(std::int32_t x, std::int32_t z) {
    std::int32_t h0;
    std::int32_t h1;
    std::int32_t h2;
    std::int32_t h3;

    std::int32_t xfrac;
    std::int32_t zfrac;

    std::int32_t answer;

    PAP_Hi *ph;

    std::int32_t mx = x >> PAP_SHIFT_HI;
    std::int32_t mz = z >> PAP_SHIFT_HI;

    if (mx < 0 || mx > PAP_SIZE_HI - 2 ||
        mz < 0 || mz > PAP_SIZE_HI - 2) {
        if (GAME_FLAGS & GF_NO_FLOOR) {
            return -32767;
        } else {
            return 0;
        }
    }

    ph = &PAP_2HI(mx, mz);

    if (ph->Flags & PAP_FLAG_HIDDEN) {
        return MAVHEIGHT(mx, mz) << 6;
    }

    if (GAME_FLAGS & GF_NO_FLOOR) {
        return -32767;
    }

    h0 = ph[0].Alt;
    h1 = ph[1].Alt;
    h2 = ph[PAP_SIZE_HI + 0].Alt;
    h3 = ph[PAP_SIZE_HI + 1].Alt;

    if (h0 == h1 && h1 == h2 && h2 == h3) {
        //
        // No need to do any interpolation.
        //

        answer = h0 << PAP_ALT_SHIFT;
    } else {
        h0 <<= PAP_ALT_SHIFT;
        h1 <<= PAP_ALT_SHIFT;
        h2 <<= PAP_ALT_SHIFT;
        h3 <<= PAP_ALT_SHIFT;

        xfrac = x & 0xff;
        zfrac = z & 0xff;

        if (xfrac + zfrac < 0x100) {
            answer = h0;
            answer += (h2 - h0) * xfrac >> 8;
            answer += (h1 - h0) * zfrac >> 8;
        } else {
            answer = h3;
            answer += (h1 - h3) * (0x100 - xfrac) >> 8;
            answer += (h2 - h3) * (0x100 - zfrac) >> 8;
        }
    }

    //
    // Modifiers.
    //

    if (ph->Flags & PAP_FLAG_SINK_SQUARE) {
        answer -= KERB_HEIGHTI;
    }

    return answer;
}

#ifndef PSX
std::int32_t PAP_is_flattish(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2) {
    std::int32_t i;

    std::int32_t max = -INFINITY;
    std::int32_t min = +INFINITY;

    std::int32_t alongx;
    std::int32_t alongz;

    std::int32_t dx = x2 - x1;
    std::int32_t dz = z2 - z1;

    std::int32_t x;
    std::int32_t z;

    std::int32_t height;

#define PAP_FLATTISH_SAMPLES 8

    for (i = 0; i < PAP_FLATTISH_SAMPLES; i++) {
        alongx = Random() & 0xff;
        alongz = Random() & 0xff;

        x = x1 + (dx * alongx >> 8);
        z = z1 + (dz * alongz >> 8);

        height = PAP_calc_height_at(x, z);

        if (height > max) {
            max = height;
        }
        if (height < min) {
            min = height;
        }

        if (abs(max - min) > 0x10) {
            return false;
        }
    }

    return true;
}

std::int32_t PAP_calc_height_noroads(std::int32_t x, std::int32_t z) {
    std::int32_t h0;
    std::int32_t h1;
    std::int32_t h2;
    std::int32_t h3;

    std::int32_t xfrac;
    std::int32_t zfrac;

    std::int32_t answer;

    PAP_Hi *ph;

    std::int32_t mx = x >> PAP_SHIFT_HI;
    std::int32_t mz = z >> PAP_SHIFT_HI;

    if (mx < 0 || mx > PAP_SIZE_HI - 2 ||
        mz < 0 || mz > PAP_SIZE_HI - 2) {
        return 0;
    }

    ph = &PAP_2HI(mx, mz);

    h0 = ph[0].Alt;
    h1 = ph[1].Alt;
    h2 = ph[PAP_SIZE_HI + 0].Alt;
    h3 = ph[PAP_SIZE_HI + 1].Alt;

    if (h0 == h1 && h1 == h2 && h2 == h3) {
        //
        // No need to do any interpolation.
        //

        answer = h0 << PAP_ALT_SHIFT;
    } else {
        h0 <<= PAP_ALT_SHIFT;
        h1 <<= PAP_ALT_SHIFT;
        h2 <<= PAP_ALT_SHIFT;
        h3 <<= PAP_ALT_SHIFT;

        xfrac = x & 0xff;
        zfrac = z & 0xff;

        if (xfrac + zfrac < 0x100) {
            answer = h0;
            answer += (h2 - h0) * xfrac >> 8;
            answer += (h1 - h0) * zfrac >> 8;
        } else {
            answer = h3;
            answer += (h1 - h3) * (0x100 - xfrac) >> 8;
            answer += (h2 - h3) * (0x100 - zfrac) >> 8;
        }
    }

    //
    // Modifiers.
    //

    return answer;
}

std::int32_t PAP_calc_map_height_near(std::int32_t x, std::int32_t z) {
    std::int32_t i;

    std::int32_t dx;
    std::int32_t dz;

    std::int32_t height;
    std::int32_t max = -INFINITY;

    for (i = 0; i < 4; i++) {
        dx = (i & 1) ? -8 : +8;
        dz = (i & 2) ? -8 : +8;

        height = PAP_calc_map_height_at(x + dx, z + dz);

        if (height > max) {
            max = height;
        }
    }

    return max;
}
#endif

std::int32_t PAP_on_slope(std::int32_t x, std::int32_t z, std::int32_t *angle) {
    std::int32_t h0;
    std::int32_t h1;
    std::int32_t h2;
    std::int32_t h3;

    std::int32_t xfrac;
    std::int32_t zfrac;

    std::int32_t answer;

    PAP_Hi *ph;

    std::int32_t mx = x >> PAP_SHIFT_HI;
    std::int32_t mz = z >> PAP_SHIFT_HI;

    if (mx < 0 || mx > PAP_SIZE_HI - 2 ||
        mz < 0 || mz > PAP_SIZE_HI - 2) {
        return 0;
    }

    ph = &PAP_2HI(mx, mz);

    if (ph->Flags & PAP_FLAG_HIDDEN) {
        //
        // We should be calling RFACE_on_slope here really!
        //

        // ASSERT(0);

        return 0;
    }

    h0 = ph[0].Alt;
    h1 = ph[1].Alt;
    h2 = ph[PAP_SIZE_HI + 0].Alt;
    h3 = ph[PAP_SIZE_HI + 1].Alt;

    if (h0 == h1 && h1 == h2 && h2 == h3) {
        //
        // No need to do any interpolation.
        //
        return (0);

    } else {
        //  h0   h2
        //
        //	h1   h3

        h0 <<= PAP_ALT_SHIFT;
        h1 <<= PAP_ALT_SHIFT;
        h2 <<= PAP_ALT_SHIFT;
        h3 <<= PAP_ALT_SHIFT;

        xfrac = x & 0xff;
        zfrac = z & 0xff;

        if (xfrac + zfrac < 0x100) {
            std::int32_t vx, vy, vz;
            std::int32_t wx, wy, wz;
            std::int32_t rx, ry, rz;
            std::int32_t len;

            vx = 256;
            vy = h2 - h0;
            vz = 0;

            wx = 0;
            wy = h1 - h0;
            wz = -256;

            rx = (vy * wz); //-vz*wy;
            ry = 65536;     // vz*wx-vx*wz; dont care about this
            rz = (vx * wy); //-vy*wx;

            if (rx == 0 && rz == 0)
                return (0);

            *angle = (Arctan(-rx, -rz)) & 2047;

            rx = abs(rx);
            rz = abs(rz);
            len = QDIST3(rx, ry, rz);

            ry = (ry << 8) / (len);

            return (abs(256 - (len >> 8)));

        } else {
            std::int32_t vx, vy, vz;
            std::int32_t wx, wy, wz;
            std::int32_t rx, ry, rz;
            std::int32_t len;

            vx = -256;
            vy = h1 - h3;
            vz = 0;

            wx = 0;
            wy = h2 - h3;
            wz = -256;

            rx = (vy * wz); //-vz*wy;
            ry = 65536;     // vz*wx-vx*wz; dont care about this
            rz = (vx * wy); //-vy*wx;

            if (rx == 0 && rz == 0)
                return (0);

            *angle = (Arctan(rx, -rz)) & 2047;

            rx = abs(rx);
            rz = abs(rz);
            len = QDIST3(rx, ry, rz);

            ry = (ry << 8) / (len);

            return (abs(256 - (len >> 8)));
        }
    }
}
