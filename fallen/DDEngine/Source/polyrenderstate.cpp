//
// Drawing polygons with D3D
//

#include <MFStdLib.h>
#include <DDLib.h>
#include <math.h>
#include "Game.h" //	Guy	-	4 DEMO
#include "matrix.h"
#include "poly.h"
#include "texture.h"
#include "message.h"
#include "night.h"
#ifndef TARGET_DC
#include "../../MuckyBasic/clip.h"
#endif
#include "vertexbuffer.h"
#include "polypoint.h"
#include "renderstate.h"
#include "polypage.h"
#include "env.h"

extern RenderState DefRenderState;
extern PolyPage POLY_Page[POLY_NUM_PAGES];

#ifdef TEX_EMBED
std::int32_t PageOrder[POLY_NUM_PAGES];
static bool PageOrdered[POLY_NUM_PAGES];
#endif

// static flag for render state setup

static bool RenderStates_OK = false;

// #define interface for setting state in a PolyPage instead
// of to the card

#undef SET_TEXTURE
#undef SET_NO_TEXTURE
#undef SET_RENDER_STATE

#ifdef TEX_EMBED
#define SET_TEXTURE(PAGE)                               \
    {                                                   \
        pa->RS.SetTexture(TEXTURE_get_handle(PAGE));    \
        pa->SetTexOffset(TEXTURE_get_D3DTexture(PAGE)); \
    }
#else
#define SET_TEXTURE(PAGE) pa->RS.SetTexture(TEXTURE_get_handle(PAGE))
#endif
#define SET_NO_TEXTURE pa->RS.SetTexture(nullptr)
#define SET_RENDER_STATE(I, V) pa->RS.SetRenderState(I, V)
#define SET_EFFECT(FX) pa->RS.SetEffect(FX)

// POLY_reset_render_states
//
// reset render states

void POLY_reset_render_states() {
    RenderStates_OK = false;
}

// POLY_init_texture_flags
//
// clear everything

void POLY_init_texture_flags() {
    POLY_reset_render_states();
    memset(POLY_page_flag, 0, sizeof(POLY_page_flag));
}

// POLY_load_texture_flags
//
// load flags from file

void POLY_load_texture_flags(char* fname, std::int32_t offset) {
    FILE* handle = MF_Fopen(fname, "rb");

    if (handle) {
        char line[256];
        std::int32_t match;
        std::int32_t page = 0;

        while (fgets(line, 256, handle)) {
            match = sscanf(line, "Page %d:", &page);

            page += offset;

            if (match == 1 && WITHIN(page, 0, POLY_NUM_PAGES - 1)) {
                char* c;

                for (c = line; *c; c++); // Zoom to the end of the line.

                while (1) {
                    c--;

                    if (*c == 'T' || *c == 't') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_TRANSPARENT;
                    } else if (*c == 'W' || *c == 'w') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_WRAP;
                    } else if (*c == 'A' || *c == 'a') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_ADD_ALPHA;
                    } else if (*c == 'I' || *c == 'i') {
                        if (WITHIN(page, 0, POLY_NUM_PAGES - 2)) {
                            POLY_page_flag[page + 0] |= POLY_PAGE_FLAG_2PASS;
                            POLY_page_flag[page + 1] |= POLY_PAGE_FLAG_TRANSPARENT;
                            POLY_page_flag[page + 1] |= POLY_PAGE_FLAG_SELF_ILLUM;
                        }
                    } else if (*c == 'S' || *c == 's') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_SELF_ILLUM;
                    } else if (*c == 'F' || *c == 'f') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_NO_FOG;
                    } else if (*c == 'D' || *c == 'd') {
                        if (WITHIN(page, 0, POLY_NUM_PAGES - 2)) {
                            POLY_page_flag[page + 0] |= POLY_PAGE_FLAG_2PASS | POLY_PAGE_FLAG_WINDOW;
                            POLY_page_flag[page + 1] |= POLY_PAGE_FLAG_WINDOW_2ND;
                        }
                    } else if (*c == 'M' || *c == 'm') {
                        POLY_page_flag[page] |= POLY_PAGE_FLAG_ALPHA;
                    } else if (*c == ':') {
                        break;
                    }
                }
            }
        }

        MF_Fclose(handle);
    }
}

// POLY_init_render_states
//
// initialize all the render states for each page

#ifdef TARGET_DC
int iPolyRenderStateFrameNum = 0;
#endif

#define HOW_MANY_FRAMES_TO_WAIT 0

void POLY_init_render_states() {
#if 0
//#ifdef TARGET_DC
	iPolyRenderStateFrameNum++;

	if ( !RenderStates_OK )
	{
		iPolyRenderStateFrameNum = 0;
	}

	//if (RenderStates_OK) return;
	if ( iPolyRenderStateFrameNum > HOW_MANY_FRAMES_TO_WAIT ) return;
#else
    if (RenderStates_OK) return;
#endif

    // create default
    extern int AENG_detail_filter;

    if (!AENG_detail_filter) {
        DefRenderState = RenderState(D3DFILTER_NEAREST, D3DFILTER_NEAREST);
    } else {
        DefRenderState = RenderState(D3DFILTER_LINEAR, D3DFILTER_LINEAR);
    }

    // TRACE("Phase %i\n", iPhase);

    // set each page to the default
    for (int ii = 0; ii < POLY_NUM_PAGES; ii++) {
        POLY_Page[ii].RS = DefRenderState;
    }

    // set defaults for each page
    PolyPage* pa;

    for (int ii = 0; ii < POLY_NUM_PAGES; ii++) {
        pa = &POLY_Page[ii];

#if 0
//#ifdef TARGET_DC
		if ( iPolyRenderStateFrameNum < HOW_MANY_FRAMES_TO_WAIT )
		{
			// Don't do anything - leave them all as default.
			// This is a horrible kludge, and I don't know why I need to
			// wait but I do. Grrrr...
		}
		else
#endif // #ifdef TARGET_DC
        {
            // Default is on!
            SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);

            // set using the old interface
            switch (ii) {
            case POLY_PAGE_LADSHAD:
                SET_TEXTURE((TEXTURE_page_ladshad));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                break;

            case POLY_PAGE_SIGN:
                SET_TEXTURE((TEXTURE_page_sign));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                break;

            case POLY_PAGE_LASTPANEL_ALPHA:
                SET_TEXTURE((TEXTURE_page_lastpanel));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);

                break;

            case POLY_PAGE_LASTPANEL_ADDALPHA:
                SET_TEXTURE((TEXTURE_page_lastpanel));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_LASTPANEL_ADD:
                SET_TEXTURE((TEXTURE_page_lastpanel));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

                break;

            case POLY_PAGE_LASTPANEL_SUB:
                SET_TEXTURE((TEXTURE_page_lastpanel));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_LASTPANEL2_ADD:
                SET_TEXTURE((TEXTURE_page_lastpanel2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

                break;

            case POLY_PAGE_LASTPANEL2_ADDALPHA:
                SET_TEXTURE((TEXTURE_page_lastpanel2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_LASTPANEL2_SUB:
                SET_TEXTURE((TEXTURE_page_lastpanel2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_LASTPANEL2_ALPHA:
                SET_TEXTURE((TEXTURE_page_lastpanel2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;

            case POLY_PAGE_LITE_BOLT:
                SET_TEXTURE((TEXTURE_page_litebolt));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                //				SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                //				SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_SHADOW_OVAL:
                SET_TEXTURE((TEXTURE_page_shadowoval));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);

                break;

            case POLY_PAGE_SHADOW_SQUARE:
                SET_TEXTURE((TEXTURE_page_shadowsquare));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_FADECAT:
                SET_TEXTURE((TEXTURE_page_fadecat));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_FADE_MF:
                SET_TEXTURE((TEXTURE_page_fade_MF));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_LADDER:
                SET_TEXTURE((TEXTURE_page_ladder));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);

                break;

            case POLY_PAGE_FINALGLOW:
                SET_TEXTURE((TEXTURE_page_finalglow));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                break;

            case POLY_PAGE_IC2_NORMAL:
                SET_TEXTURE((TEXTURE_page_ic2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                break;

            case POLY_PAGE_IC_NORMAL:
                SET_TEXTURE((TEXTURE_page_ic));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                break;

            case POLY_PAGE_IC2_ALPHA:
            case POLY_PAGE_IC2_ALPHA_END:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_TEXTURE((TEXTURE_page_ic2));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                break;

            case POLY_PAGE_IC_ALPHA:
            case POLY_PAGE_IC_ALPHA_END:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_TEXTURE((TEXTURE_page_ic));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                break;

            case POLY_PAGE_IC2_ADDITIVE:
                SET_TEXTURE((TEXTURE_page_ic2));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                break;

            case POLY_PAGE_IC_ADDITIVE:
                SET_TEXTURE((TEXTURE_page_ic));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                break;

            case POLY_PAGE_PRESS1:
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#ifndef TARGET_DC
                TEXTURE_set_colour_key(TEXTURE_page_press1);
                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
#endif
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_press1));
                break;

            case POLY_PAGE_PRESS2:
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#ifndef TARGET_DC
                TEXTURE_set_colour_key(TEXTURE_page_press2);
                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
#endif
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_press2));
                break;

            case POLY_PAGE_TARGET:
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#ifndef TARGET_DC
                TEXTURE_set_colour_key(TEXTURE_page_target);
                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
#endif
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_target));
                break;

            case POLY_PAGE_DEVIL:
                SET_TEXTURE((TEXTURE_page_devil));
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);
                break;

            case POLY_PAGE_ANGEL:
                SET_TEXTURE((TEXTURE_page_angel));
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);
                break;

            case POLY_PAGE_LEAF:
                SET_TEXTURE((TEXTURE_page_leaf));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, true);

                break;

            case POLY_PAGE_RUBBISH:
                SET_TEXTURE((TEXTURE_page_rubbish));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, true);

                break;

            case POLY_PAGE_WINMAP:

                //
                // Draw the environment map additive alpha.
                //

                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_winmap));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);

                break;

            case POLY_PAGE_ENVMAP:

                //
                // Draw the environment map additive alpha.
                //

                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_envmap));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                break;

            case POLY_PAGE_SEWATER:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_TEXTURE((TEXTURE_page_water));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                break;

            case POLY_PAGE_SKY:
                SET_TEXTURE((TEXTURE_page_sky));
#if USE_TOMS_ENGINE_PLEASE_BOB
                // This now needs to be Z-buffered, because it is no longer the first thing drawn any more.
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, true);
#else
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
#endif
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_SHADOW:
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);

#ifdef TARGET_DC
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
#else
                if (the_display.GetDeviceInfo()->DestInvSourceColourSupported()) {
                    // use a density greyscale shadowmap
                    SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                    SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                    SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
                } else {
                    // use a density alpha (+black) shadowmap
                    SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                    SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                    SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                }
#endif

                SET_TEXTURE((TEXTURE_page_shadow));
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZBIAS, 8);
                break;

            case POLY_PAGE_TEST_SHADOWMAP:
                if (the_display.GetDeviceInfo()->DestInvSourceColourSupported()) {
                    SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                    SET_TEXTURE(TEXTURE_page_shadow);
                    SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                }
                break;

            case POLY_PAGE_PUDDLE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_puddle));
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_MOON:
                //				SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE,false);
                SET_TEXTURE((TEXTURE_page_moon));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                //				TEXTURE_set_colour_key(TEXTURE_page_moon);
                //				SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE,true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                break;

            case POLY_PAGE_MANONMOON:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_TEXTURE((571));
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                break;

            case POLY_PAGE_CLOUDS:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_clouds));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_ALPHA:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_NO_TEXTURE;
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_ALPHA_OVERLAY:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_NO_TEXTURE;
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                break;

            case POLY_PAGE_ADDITIVE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_NO_TEXTURE;
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_ADDITIVEALPHA:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_NO_TEXTURE;
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_MASKED:
                SET_TEXTURE(-1);
#ifndef TARGET_DC
                TEXTURE_set_colour_key(0);
                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
#endif
                break;

            case POLY_PAGE_FACE1:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
                SET_TEXTURE((TEXTURE_page_face1));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);

                break;

            case POLY_PAGE_FACE2:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
                SET_TEXTURE((TEXTURE_page_face2));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);

                break;

            case POLY_PAGE_FACE3:
                SET_TEXTURE((TEXTURE_page_face3));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_FACE4:
                SET_TEXTURE((TEXTURE_page_face4));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_FACE5:
                SET_TEXTURE((TEXTURE_page_face5));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_FACE6:
                SET_TEXTURE((TEXTURE_page_face6));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_COLOUR_ALPHA:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_NO_TEXTURE;

                break;

            case POLY_PAGE_COLOUR:
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_NO_TEXTURE;

                break;

            case POLY_PAGE_COLOUR_WITH_FOG:
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                SET_NO_TEXTURE;
                break;

            case POLY_PAGE_WATER:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_water));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_DRIP:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_drip));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_FOG:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_fog));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_STEAM:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_steam));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_BANG:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_bang));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_TEXT:
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_font));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_LOGO:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_logo));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_DROPLET:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_droplet));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                break;

            case POLY_PAGE_RAINDROP:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_TEXTURE((TEXTURE_page_raindrop));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                break;

            case POLY_PAGE_SPARKLE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_sparkle));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                break;

            case POLY_PAGE_FLAMES:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_flames));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_FLAMES2:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_flame2));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_SMOKE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
                SET_TEXTURE((TEXTURE_page_smoke));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_MENUFLAME:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_menuflame));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                break;

            case POLY_PAGE_MENUTEXT:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ZBIAS, 0);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_flames));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                break;

            case POLY_PAGE_MENUPASS:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_TEXTURE((TEXTURE_page_moon));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#ifndef TARGET_DC
                TEXTURE_set_colour_key(TEXTURE_page_moon);
                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
#endif
                break;

            case POLY_PAGE_BARBWIRE:
                // SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_barbwire));
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_FOOTPRINT:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_footprint));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                break;

            case POLY_PAGE_FONT2D:
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_font2d));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                // SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE,true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;

            case POLY_PAGE_BIGBANG:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_bigbang));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_EFFECT(RS_InvAlphaPremult);

                break;

            case POLY_PAGE_FLAMES3:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_flames3));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_EFFECT(RS_InvAlphaPremult);
                break;

            case POLY_PAGE_DUSTWAVE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_dustwave));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                // SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS,D3DTADDRESS_WRAP);
                SET_EFFECT(RS_InvAlphaPremult);

                break;

            case POLY_PAGE_BLOODSPLAT:
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_bloodsplat));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);

                break;

            case POLY_PAGE_BLOOM1:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_bloom1));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_BLOOM2:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_bloom2));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_SNOWFLAKE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_snowflake));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                break;
            case POLY_PAGE_HITSPANG:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_hitspang));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_LENSFLARE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_lensflare));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                break;

            case POLY_PAGE_TYRETRACK:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_tyretrack_alpha));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                break;

            case POLY_PAGE_TYRESKID:
                /*				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
                                                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE,false);
                                                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE,true);
                                                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO);
                                                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR);
                                                SET_TEXTURE((TEXTURE_page_tyretrack));
                                                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,true);			*/

                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_tyretrack));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ZBIAS, 4);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_NEWFONT_INVERSE:

                /*				SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE,false);
                //				SET_TEXTURE((TEXTURE_page_newfont));
                                                SET_TEXTURE((TEXTURE_page_lcdfont));
                                                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
                                                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO);
                                                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
                                                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE,true);
                                                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);*/

                /*

                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE,false);
//				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE,TEXTURE_get_handle(TEXTURE_page_lcdfont));
                SET_TEXTURE((TEXTURE_page_lcdfont));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE,true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);

                */

                //
                // I want to try a special effect that needs additive alpha!
                //

#ifdef TARGET_DC
                // We use the new font on the DC that is just alpha-blended on.
                SET_TEXTURE((TEXTURE_page_lcdfont));
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#else
                SET_TEXTURE((TEXTURE_page_lcdfont));
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
#endif

                break;

            case POLY_PAGE_MENULOGO:
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREHANDLE,TEXTURE_get_handle(TEXTURE_page_menulogo));
                SET_TEXTURE((TEXTURE_page_menulogo));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_POLAROID:
                SET_TEXTURE((TEXTURE_page_polaroid));
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);

                break;

            case POLY_PAGE_SMOKER:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_smoker));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_NEWFONT:
                SET_RENDER_STATE(D3DRENDERSTATE_ZENABLE, false);
                SET_TEXTURE((TEXTURE_page_lcdfont));
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_EXPLODE1:

                /*

                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND,D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE,true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND,D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_explode1));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                */

                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_explode1));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                break;

            case POLY_PAGE_EXPLODE2:

                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_explode2));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                break;

            case POLY_PAGE_EXPLODE1_ADDITIVE:

                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_explode1));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_EXPLODE2_ADDITIVE:

                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAG,D3DFILTER_NEAREST);
                //				SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMIN,D3DFILTER_NEAREST);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_explode2));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);

                break;

            case POLY_PAGE_SPLASH:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_splash));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_EFFECT(RS_InvAlphaPremult);
                break;

            case POLY_PAGE_METEOR:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_meteor));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_SUBTRACTIVEALPHA:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_NO_TEXTURE;
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_SMOKECLOUD:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_smokecloud));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_EFFECT(RS_BlackWithAlpha);
                break;

            case POLY_PAGE_SMOKECLOUD2:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_smokecloud));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);

                break;

            case POLY_PAGE_BIG_BUTTONS:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_bigbutton));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_TINY_BUTTONS:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_tinybutt));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;

            case POLY_PAGE_BIG_LEAF:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((TEXTURE_page_bigleaf));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                break;
            case POLY_PAGE_BIG_RAIN:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_bigrain));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_EFFECT(RS_AlphaPremult);
                break;

            case POLY_PAGE_PCFLAMER:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                SET_TEXTURE((TEXTURE_page_pcflamer));
                // SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE,false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                SET_EFFECT(RS_InvAlphaPremult);

                break;

#ifdef TARGET_DC
            case POLY_PAGE_BACKGROUND_IMAGE:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, false);
                SET_TEXTURE((TEXTURE_page_background_use_instead));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                // SET_EFFECT(RS_InvAlphaPremult);
                break;

            case POLY_PAGE_BACKGROUND_IMAGE2:
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, false);
                SET_TEXTURE((TEXTURE_page_background_use_instead2));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                // SET_EFFECT(RS_InvAlphaPremult);
                break;

            case POLY_PAGE_JOYPAD_A:
            case POLY_PAGE_JOYPAD_B:
            case POLY_PAGE_JOYPAD_C:
            case POLY_PAGE_JOYPAD_X:
            case POLY_PAGE_JOYPAD_Y:
            case POLY_PAGE_JOYPAD_Z:
            case POLY_PAGE_JOYPAD_L:
            case POLY_PAGE_JOYPAD_R:
            case POLY_PAGE_JOYPAD_PAD_L:
            case POLY_PAGE_JOYPAD_PAD_R:
            case POLY_PAGE_JOYPAD_PAD_D:
            case POLY_PAGE_JOYPAD_PAD_U: {
                int iPage = (ii - POLY_PAGE_JOYPAD_A) + TEXTURE_page_joypad_a;
                SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                SET_TEXTURE((iPage));
                SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
            } break;
#endif

            default:

                if (ii < TEXTURE_page_num_standard && !(draw_3d && (ii == 510 || ii == 511))) {
                    SET_TEXTURE((ii));

                    if (POLY_page_flag[ii]) {
                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_TRANSPARENT) {
#ifndef TARGET_DC
                            bool use_chroma = false;

                            if (POLY_page_flag[ii] & POLY_PAGE_FLAG_SELF_ILLUM) {
                                if (the_display.GetDeviceInfo()->DestInvSourceColourSupported() &&
                                    !the_display.GetDeviceInfo()->ModulateAlphaSupported()) {
                                    // it's a RagePro, so use chroma keying
                                    use_chroma = true;
                                }
                            }

                            if (use_chroma) {
                                // use colour keying to make the windows work on the Rage Pro
                                TEXTURE_set_colour_key(ii);
                                SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE, true);
                            } else
#endif
                            {
                                SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, true);
                            }
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_WRAP) {
                            SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_WRAP);
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_ADD_ALPHA) {
                            SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                            SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                            SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                            SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                        }

                        if ((POLY_page_flag[ii] & POLY_PAGE_FLAG_SELF_ILLUM) && !draw_3d) {
                            SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_NO_FOG) {
                            SET_RENDER_STATE(D3DRENDERSTATE_FOGENABLE, false);
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_WINDOW) {
                            /*

                            TEXTURE_set_colour_key(ii);
                            SET_RENDER_STATE(D3DRENDERSTATE_COLORKEYENABLE,true);

                            */
                            SET_RENDER_STATE(D3DRENDERSTATE_ALPHATESTENABLE, true);

                            SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                            SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
                            SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_WINDOW_2ND) {
                            SET_RENDER_STATE(D3DRENDERSTATE_ZFUNC, D3DCMP_LESS);
                        }

                        if (POLY_page_flag[ii] & POLY_PAGE_FLAG_ALPHA) {
                            SET_RENDER_STATE(D3DRENDERSTATE_ZWRITEENABLE, false);
                            SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                            SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                            SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
                        }
                    }

                    if (the_display.GetDeviceInfo()->AdamiLightingSupported()) {
                        switch (ii >> 6) {
                        case 9:
                        case 10:
                        case 18:
                        case 19:
                        case 20:
                        case 21:

                            //
                            // This is a people texture page!
                            //

                            SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
                            SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);

                            // SET_RENDER_STATE(D3DRENDERSTATE_SRCBLEND,D3DBLEND_DESTCOLOR);
                            // SET_RENDER_STATE(D3DRENDERSTATE_DESTBLEND,D3DBLEND_SRCCOLOR);

                            SET_RENDER_STATE(D3DRENDERSTATE_ALPHABLENDENABLE, true);
                            SET_RENDER_STATE(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);

                            break;
                        }
                    }
                }
                /*
                                                else
                                                if (ii > TEXTURE_page_people3 && !draw_3d  && ii<TEXTURE_num_textures)
                                                {
                                                        SET_TEXTURE((ii));
                                                }
                */
                break;
            }
        }

        // TRACE ( "0x%x ", POLY_Page[ii].RS.GetTexture() );
    }

#ifdef TEX_EMBED

    // Before we do this, we must flush all the currently queued polys,
    // because this will change the sorting order, and thus some
    // pages may not be drawn that should be and stuff.
    extern void POLY_ClearAllPages();
    POLY_ClearAllPages();

    // now sort the pages on texture
    TRACE("Sorting texture pages: ");
    for (int ii = 0; ii < POLY_NUM_PAGES; ii++) {
        PageOrdered[ii] = false;
    }

#ifdef DEBUG
    int iNullTextureCount = 0;
#endif

    std::int32_t pos = 0;
    while (pos < POLY_NUM_PAGES) {
        int ii;
        for (ii = 0; ii < POLY_NUM_PAGES; ii++) {
            if (!PageOrdered[ii]) break;
        }
        if (ii == POLY_NUM_PAGES) {
            // All done
            break;
        }

        // TRACE("%d", ii);
        POLY_Page[ii].pTheRealPolyPage = &(POLY_Page[ii]);

        PageOrder[pos++] = ii;
        PageOrdered[ii] = true;
        LPDIRECT3DTEXTURE2 tex = POLY_Page[ii].RS.GetTexture();
        PolyPage* pPolyPage = &(POLY_Page[ii]);

        if (tex != nullptr) {
            for (ii++; ii < POLY_NUM_PAGES; ii++) {
                if (POLY_Page[ii].RS.GetTexture() == tex) {
                    // Now see if the render states are identical as well.
                    // If so, we can make this page a "ghost" page that
                    // never actually gets rendered - all polys sent to it will
                    // actually go to the real page.
                    if (POLY_Page[ii].RS.IsSameRenderState(&(pPolyPage->RS))) {
                        // Yes, they are the same, so this is a ghost page.
                        POLY_Page[ii].pTheRealPolyPage = pPolyPage;
                    } else {
                        // Nope - this is a separate one.
                        POLY_Page[ii].pTheRealPolyPage = &(POLY_Page[ii]);
                        // TRACE(" %d", ii);
                        PageOrder[pos++] = ii;
                    }
                    PageOrdered[ii] = true;
                }
            }
        }
#ifdef DEBUG
        else {
            iNullTextureCount++;
        }
#endif

        // TRACE("\n");
    }

#ifdef DEBUG
    TRACE("Number of nullptr pages %i\n", iNullTextureCount);
#endif

    extern int iPolyNumPagesRender;
    iPolyNumPagesRender = pos;

#endif

#ifndef TARGET_DC
#if 0
	for (int ii = 0; ii < POLY_NUM_PAGES; ii++)
	{
		if (char* err = POLY_Page[ii].RS.Validate())
		{
			TRACE("Page %d : %s\n", ii, err);
		}
	}
#endif
#endif

    RenderStates_OK = true;
}
