// Game.cpp
// Guy Simmons, 17th October 1997

#include "demo.h"
#include "Game.h"
#include "pap.h"
#include "Attract.h"
#include "id.h"
#include "io.h"
#include "light.h"
#include "heap.h"
#include "hm.h"
#include "dirt.h"
#include "fog.h"
#include "mist.h"
#include "water.h"
#include "cnet.h"
#include "interfac.h"
#include "puddle.h"
#include "road.h"
#include "az.h"
#include "cam.h"
#include "door.h"
#include "sewer.h"
#include "drip.h"
#include "Sound.h"
#include "gamemenu.h"
#ifndef PSX
#include "..\ddengine\headers\menufont.h"
#include "..\ddlibrary\headers\net.h"
#endif
#include "bang.h"
#include "mav.h"
#include "..\editor\headers\extra.h"
#include "spark.h"
#include "statedef.h"
#include "glitter.h"
#include "ob.h"
#include "morph.h"
#include "qedit.h"
#include "trip.h"
#include "pap.h"
#include "night.h"
#include "shadow.h"
#include "cloth.h"
#include "ns.h"
#include "supermap.h"
#include "..\sedit\headers\es.h"
#include "build2.h"
#include "eway.h"
#include "elev.h"
#include "pause.h"
#include "snipe.h"
#include "pow.h"
#include "Game.h"
#include "widget.h"
#include "memory.h"
#include "fc.h"
#include "startscr.h"
#include "save.h"

#ifndef PSX
#include "spark.h"
#include "font2d.h"
#include "env.h"
#endif
#include "wmove.h"
#include "balloon.h"
#include "memory.h"
#ifndef PSX
#include "..\DDEngine\Headers\console.h"
#include "..\DDEngine\Headers\poly.h"
#include "..\DDEngine\Headers\map.h"
#endif
#include "psystem.h"
#include "ribbon.h"
#include "overlay.H"
#include "music.h"
#include "grenade.h"
#ifndef PSX
#include "drawxtra.h"
#include "..\ddlibrary\headers\ddlib.h"
#include "..\ddengine\headers\planmap.h"
#include <math.h>
#include "..\ddengine\headers\BreakTimer.h"
#include "frontend.h"
#include "..\ddengine\headers\truetype.h"
#include "panel.h"

#else
#include "c:\fallen\psxeng\headers\psxeng.h"
#include <ctrller.h>
extern ControllerPacket PAD_Input1, PAD_Input2;

#endif

#include "xlat_str.h"
#include "DCLowLevel.h"

#define TIMEOUT_DEMO 0
#define VERIFY_PLAYBACK 0 // !$$! set to 1 to verify demo playback

#include "mfx.h"
#include "..\ddengine\headers\superfacet.h"
#include "..\ddengine\headers\farfacet.h"
#include "..\ddengine\headers\fastprim.h"
#include "..\ddengine\headers\supercrinkle.h"

std::int32_t CAM_cur_x, CAM_cur_y, CAM_cur_z,
    CAM_cur_yaw, CAM_cur_pitch, CAM_cur_roll; // these are set appropriate to whichever cam

#ifdef PSX
std::uint8_t continue_waiting, replay_waiting;
extern std::int32_t PSX_eog_timer;
#endif

//
// The editor.
//

std::int32_t save_psx = 0; // this was nicked from edit.cpp in the editor...

// bool g_bGoToCreditsPleaseGameHasFinished = false;

extern std::uint8_t editor_loop();
#ifndef PSX
extern bool allow_debug_keys;
#endif
//
// Nearly everything in the whole game.
//

//
// FOR PC for GERMAN/French we only need edit this file
//

#ifndef PSX
// #define	VERSION_GERMAN	1
// #define	VERSION_FRENCH	1
#endif

#define VIOLENCE_ALLOWED 1

#ifdef VERSION_GERMAN
// #error
#undef VIOLENCE_ALLOWED
#define VIOLENCE_ALLOWED 0
#endif

#ifdef VERSION_FRENCH
// #error
#undef VIOLENCE_ALLOWED
#ifdef TARGET_DC
// Hooray! We're allowed to roast live babies over open fires now!
#define VIOLENCE_ALLOWED 1
#else
#define VIOLENCE_ALLOWED 0
#endif
#endif

Game the_game;
std::uint8_t VIOLENCE = VIOLENCE_ALLOWED;

extern std::int32_t go_into_game; // This is in attract.cpp If it is true when we leave the loop, then attract goes into the game again.

void stop_all_fx_and_music() {
    MFX_QUICK_stop();
    MUSIC_mode(0);
    MUSIC_mode_process();
    MUSIC_reset();
    MFX_stop(MFX_CHANNEL_ALL, MFX_WAVE_ALL);
}

//
// Loads data that only needs to be loaded once, or after we exit
// the editor.
//

#ifndef PSX
#ifndef TARGET_DC

void global_load() {
    init_memory();

    clear_prims();
    init_draw_tweens();
    setup_people_anims();
    setup_extra_anims();
    setup_global_anim_array();
    record_prim_status();
}
#endif
#endif

//---------------------------------------------------------------
// #define	DebugText
void game_startup() {
    GAME_STATE = 0;

#ifndef PSX
    init_memory();
#else
    extern void UCA_LookupSetup();
    UCA_LookupSetup();
#endif
    FC_init();

#ifndef PSX

    //
    // Do PC setup.
    //

    //	NET_init();

    /*	{
                    //
                    // Enumerate connections!
                    //

                    std::int32_t i;
                    std::int32_t num_connections = NET_get_connection_number();

                    for (i = 0; i < num_connections; i++)
                    {
                            TRACE("Connection %d: %s\n", i, NET_get_connection_name(i));
                    }
            }
    */

    if (OpenDisplay(640, 480, 16, FLAGS_USE_3D | FLAGS_USE_WORKSCREEN) == 0) {
        GAME_STATE = GS_ATTRACT_MODE;
    } else {
#ifndef TARGET_DC
        MessageBox(nullptr, "Unable to open display", nullptr, MB_OK | MB_ICONWARNING);
#else
        ASSERT(false);
#endif
        exit(1);
    }

    AENG_init();

    // Get a loading screen up as soon as possible.
    ATTRACT_loadscreen_init();

    // NOW we can load the sounds, coz we have something sensible on-screen
    // #ifdef TARGET_DC
    extern void MFX_init();
    MFX_init();
    // #endif

    ATTRACT_loadscreen_draw(160);

    void init_joypad_config();
    init_joypad_config();
    ANIM_init();

#ifdef TARGET_DC
    // Don't actually get a device until someone presses a button on one of them.
    GetInputDevice(JOYSTICK, 0, false);
#else
    GetInputDevice(JOYSTICK, 0, true);
#endif

    MORPH_load();

#ifndef TARGET_DC
    if (ENV_get_value_number("clumps", 0, "Secret")) {
        extern void make_all_clumps();
        make_all_clumps();
    }
#endif

    TEXTURE_load_needed("levels\\frontend.ucm", 160, 256, 65);

#ifdef TARGET_DC
    // Load up the VMU screens needed.

    extern VMU_Screen *pvmuscreenAmmo;
    extern VMU_Screen *pvmuscreenMFLogo;
    extern VMU_Screen *pvmuscreenPressStart;
    extern VMU_Screen *pvmuscreenSaved;
    extern VMU_Screen *pvmuscreenUCLogo;
    extern VMU_Screen *pvmuscreenWait;

#define LOAD_VMU_SCREEN(myname, filename) \
    if (pvmuscreen##myname == nullptr) CreateVMUScreenFromTGA(filename, &(pvmuscreen##myname));

    LOAD_VMU_SCREEN(Ammo, "server\\textures\\extras\\DC\\VMU_ammo_count2.tga");
    LOAD_VMU_SCREEN(MFLogo, "server\\textures\\extras\\DC\\VMU_muckylogo.tga");
    LOAD_VMU_SCREEN(PressStart, "server\\textures\\extras\\DC\\VMU_Press_start.tga");
    LOAD_VMU_SCREEN(Saved, "server\\textures\\extras\\DC\\VMU_saved.tga");
    LOAD_VMU_SCREEN(UCLogo, "server\\textures\\extras\\DC\\VMU_Urbanlogo.tga");
    LOAD_VMU_SCREEN(Wait, "server\\textures\\extras\\DC\\VMU_Wait.tga");

#undef LOAD_VMU_SCREEN
#endif

#else

    //
    // PSX setup.
    // Take setting up the display from here since it's about 10 seconds before
    // anything useful gets drawn on it, lets do it someplace where it might not
    // fucking fail Sony standards.
    //
    /*
            if (OpenDisplay(640,480,16,FLAGS_USE_3D|FLAGS_USE_WORKSCREEN)==0)
            {
    */
    GAME_STATE = GS_PLAY_GAME;
    /*
            }
    */

    AENG_init();
    extern void Wadmenu_Introduction();

    extern std::uint8_t Eidos_Played;
    //
    // Load our sound effects.
    //

    //	LoadWaveList("Data\\SFX\\1622\\","Data\\SFX\\Samples.txt");

#endif

    //
    // Load the console font
    //

#ifdef PSX
    CONSOLE_font("data\\font3d\\all\\", 100);
#else
    CONSOLE_font("data\\font3d\\all\\", 0.2F);
#endif

    //
    // Load stuff in.
    //

    //	global_load();
}

//---------------------------------------------------------------

void game_shutdown() {
#ifndef PSX

    //
    // PC shutdown.
    //

    CloseDisplay();

    NET_kill();
    AENG_fini();
    ANIM_fini();

#else

    extern void* mem_all;

    //
    // PSX shutdown.
    //

    if (mem_all) {
        bool SetupMemory();
        SetupMemory();
        mem_all = 0;
    }

    CloseDisplay();

#endif
}

//---------------------------------------------------------------

#ifndef PSX

//
// Playback file stuff....
//

// extern char       *playback_name = "Data\\Game.pkt";
extern char *playback_name = "C:\\Windows\\Desktop\\UrbanChaosRecordedGame.pkt";
extern MFFileHandle playback_file;

extern char *verifier_name = "C:\\Windows\\Desktop\\UrbanChaosRecordedGame.tst";
extern MFFileHandle verifier_file;

#endif

//
// The player position is loaded into here by load_level()
//

extern GameCoord player_pos;

//---------------------------------------------------------------

//
// These #defines are nicked from interface.cpp
//

#define AXIS_CENTRE 32768
#define NOISE_TOLERANCE 1024
#define AXIS_MIN (AXIS_CENTRE - NOISE_TOLERANCE)
#define AXIS_MAX (AXIS_CENTRE + NOISE_TOLERANCE)

#ifndef PSX
extern DIJOYSTATE the_state;
#endif

//
// Does the game paused stuff...
//

#define PAUSED_KEY_START (1 << 0)
#define PAUSED_KEY_UP (1 << 1)
#define PAUSED_KEY_DOWN (1 << 2)
#define PAUSED_KEY_SELECT (1 << 3)

#define PAUSE_MENU_SIZE 2

char *pause_menu[PAUSE_MENU_SIZE] =
    {
        "CONTINUE GAME",
        "EXIT",
};

std::uint8_t game_paused_key;
std::int8_t game_paused_highlight;

extern bool text_fudge;
extern std::uint32_t text_colour;
#ifndef PSX
extern void draw_centre_text_at(float x, float y, char *message, std::int32_t font_id, std::int32_t flag);
extern void draw_text_at(float x, float y, char *message, std::int32_t font_id);
#else
extern void draw_centre_text_at(std::int32_t x, std::int32_t y, char* message, std::int32_t font_id, std::int32_t flag);
extern void draw_text_at(std::int32_t x, std::int32_t y, char* message, std::int32_t font_id);
#endif

//
// Prints up stuff to the screen...
//
#if !defined(PSX) && !defined(TARGET_DC)
#define NUM_BULLETS 15

char *bullet_point[NUM_BULLETS] =
    {
        "A - Punch : B - Kick : C - Jump\n",
        "Try running over a coke can...\n",
        "Press JUMP and push back to do a backwards summersault!\n",
        "Stand in a puddle and see your reflection!\n",
        "Z is your action button. Press it to climb ladders\nand to take out and put away your gun\n",
        "You can swirl the mist by running through it.\n",
        "Your shadow goes up walls!\n",
        "Press JUMP and push left or right to do a cartwheel\n",
        "Jump on fences to clamber over them.\nWatch out! Some of them are electrified.\n",
        "You can climb onto the rooftops.\nSee how high you can go!\n",
        "Hold down Y to look around in first person.\n",
        "Sneak up behind enemies and press punch.\nIf you're in the right place you'll break their necks!\n",
        "The shoulder buttons rotate the camera.\n",
        "Press action to climb up ladders.\n",
        "X is your mode change button.\nChange through RUN, WALK and SNEAK modes\n"};

std::int32_t bullet_upto;
std::int32_t bullet_counter;

void process_bullet_points() {
    bullet_counter -= 1;

    if (bullet_counter < 0) {
        bullet_upto += 1;
        bullet_upto = bullet_upto % NUM_BULLETS;
        bullet_counter = 250;
    }

    std::int32_t bright = bullet_counter * 32;

    if (bright > 255) {
        bright = 255;
    }

    text_fudge = false;
    text_colour = bright * 0x00010101;

    draw_centre_text_at(10, 420, bullet_point[bullet_upto], 0, 0);
}
#endif

//---------------------------------------------------------------

bool game_init() {
    std::int32_t ret;

    // stop_all_fx_and_music();

#ifdef TARGET_DC
    // Frontend unloading to save video memory
    the_display.FiniBackCache();
#endif

    //
    // Set the seed and initialise game variables.
    //
#if !defined(PSX) && !defined(TARGET_DC)
    global_load();
#endif

    GAME_TURN = 0;
    GAME_FLAGS = 0;
    if (!CNET_network_game) {
        NO_PLAYERS = 1;
        PLAYER_ID = 0;
    }
    TICK_RATIO = (1 << TICK_SHIFT);
    DETAIL_LEVEL = 0xffff;

    ResetSmoothTicks();

#ifndef TARGET_DC
    INDOORS_INDEX = 0;
    INDOORS_INDEX_NEXT = 0;
    INDOORS_INDEX_FADE_EXT_DIR = 0;
    INDOORS_INDEX_FADE_EXT = 0;
    INDOORS_DBUILDING = 0;
#endif

    SetSeed(0);
    srand(1234567);

    // Load up game settings from the env values.

    // Panel position. Numbers divided by 4 to fit in a byte.
    extern int m_iPanelXPos;
    extern int m_iPanelYPos;
    // m_iPanelXPos = 4 * ENV_get_value_number ( "panel_x", 32 / 4, "" );
    // m_iPanelYPos = 4 * ENV_get_value_number ( "panel_y", (480-32) / 4, "" );

#ifndef PSX

    //
    // Open the game record/playback file.
    //

#ifdef TARGET_DC
    playback_file = nullptr;
    verifier_file = nullptr;
#else

    if (GAME_STATE & GS_RECORD) {
        DebugText(" PLAYBACK GAME\n");
        playback_file = FileCreate(playback_name, true);
#if VERIFY_PLAYBACK
        verifier_file = FileCreate(verifier_name, true);
#else
        verifier_file = nullptr;
#endif
    } else if (GAME_STATE & GS_PLAYBACK) {
        DebugText(" RECORD GAME\n");
        playback_file = FileOpen(playback_name);
#if VERIFY_PLAYBACK
        verifier_file = FileOpen(verifier_name);
#else
        verifier_file = nullptr;
#endif
    }

    if (playback_file == FILE_CREATION_ERROR) {
        playback_file = nullptr;
    }
    if (verifier_file == FILE_CREATION_ERROR) {
        verifier_file = nullptr;
    }

#endif
#endif

    //
    // Initialise pause mode...
    //

    game_paused_key = -1;
    GAME_FLAGS &= ~GF_PAUSED;

    //
    // Initiliase the bullet points...
    //
#if !defined(PSX) && !defined(TARGET_DC)
    bullet_upto = -1;
    bullet_counter = 0;
#endif
    //
    // Load our sound effects.
    //
    /*
    #ifndef USE_A3D
            LoadWaveList("Data\\SFX\\1622\\","Data\\SFX\\Samples.txt");
    #else
            A3DLoadWaveList("Data\\SFX\\1622\\","Data\\SFX\\Samples.txt");
    #endif
    */

    extern THING_INDEX PANEL_wide_top_person;
    extern THING_INDEX PANEL_wide_bot_person;

    PANEL_wide_top_person = 0;
    PANEL_wide_bot_person = 0;

    if (GAME_STATE & GS_REPLAY) {
        ATTRACT_loadscreen_init();

#ifndef PSX
        extern char ELEV_fname_level[];
        extern std::int32_t quick_load;

        quick_load = true;

        ANIM_init();

        ELEV_load_name(ELEV_fname_level);

        quick_load = false;

        ret = 1;

#else
        extern char* psx_game_name;
        extern void reload_level();
        extern void load_whole_game(char* gamename);

        reload_level();
        ret = 1;
//		load_whole_game(psx_game_name);
#endif

    } else {
        //
        // Load the game.
        //
#ifndef PSX
        ret = ELEV_load_user(go_into_game);
#else
        MFX_load_wave_list(); // handled after loading the mission -- to select 'audio worlds'
        ret = ELEV_load_user(0);
        /*
        static long trk_list[]={1,2,3,4,5,6,7,8,9,10,11,0};
                        CdPlay(2,trk_list,0);
        */
        void NIGHT_light_the_map();
#ifdef VERSION_DEMO
        if (ret)
#endif
            NIGHT_light_the_map();

#endif

#ifndef PSX

        extern std::int32_t save_psx;
        if (save_psx)
            if (ret == 5 || ret == 1) // loaded a level
            {
                char save_wad[100];

                extern char ELEV_fname_level[];
                process_things(0);

                extern void save_whole_game(char *gamename);

                change_extension(ELEV_fname_level, "wad", save_wad);
                save_whole_game(save_wad);
            }
#endif
    }

    void init_stats();
    init_stats();

    EWAY_tutorial_string = nullptr;

#ifdef TARGET_DC
    // Try to get the camera in a sane position at the start of the game.
    FC_setup_initial_camera(0);
#endif

    return (ret);

    //	return ELEV_load_name("levels\\e7.ucm");
}

#ifndef PSX
#ifndef TARGET_DC
bool game_create_psx(char *mission_name) {
    std::int32_t ret;
    DebugText("PSX create psx mission %s\n", mission_name);

    //
    // Set the seed and initialise game variables.
    //
#if !defined(PSX) && !defined(TARGET_DC)
    global_load();
#endif

    GAME_TURN = 0;
    GAME_FLAGS = 0;
    if (!CNET_network_game) {
        NO_PLAYERS = 1;
        PLAYER_ID = 0;
    }
    TICK_RATIO = (1 << TICK_SHIFT);
    DETAIL_LEVEL = 0xffff;

    ResetSmoothTicks();

#ifndef TARGET_DC
    INDOORS_INDEX = 0;
    INDOORS_INDEX_NEXT = 0;
    INDOORS_INDEX_FADE_EXT_DIR = 0;
    INDOORS_INDEX_FADE_EXT = 0;
    INDOORS_DBUILDING = 0;
#endif

    SetSeed(0);
    srand(1234567);
    GAME_STATE = GS_PLAY_GAME;

    extern std::int32_t quick_load;
    quick_load = 1;

    //
    // Initialise pause mode...
    //

    game_paused_key = -1;

    GAME_FLAGS &= ~GF_PAUSED;

    //
    // Initiliase the bullet points...
    //
#if !defined(PSX) && !defined(TARGET_DC)
    bullet_upto = -1;
    bullet_counter = 0;
#endif

    {
        //
        // Load the game.
        //
        //		ret=ELEV_load_user(go_into_game);
        ret = ELEV_load_name(mission_name);

        if (ret == 5 || ret == 1) // loaded a level
        {
            char save_wad[100];

            extern char ELEV_fname_level[];
            process_things(0);

            extern void save_whole_game(char *gamename);

            change_extension(mission_name, "wad", save_wad);
            DebugText("PSX create nad %s\n world %d", save_wad, TEXTURE_SET);
            save_whole_game(save_wad);

        } else
            ASSERT(0);
    }

    return (ret);
}

bool make_texture_clumps(char *mission_name) {
#ifdef TARGET_DC
    ASSERT(false);
#endif
    std::int32_t ret;
    DebugText("Making texture clumps %s\n", mission_name);

    //
    // Set the seed and initialise game variables.
    //
    global_load();

    GAME_TURN = 0;
    GAME_FLAGS = 0;
    if (!CNET_network_game) {
        NO_PLAYERS = 1;
        PLAYER_ID = 0;
    }
    TICK_RATIO = (1 << TICK_SHIFT);
    DETAIL_LEVEL = 0xffff;

    ResetSmoothTicks();

#ifndef TARGET_DC
    INDOORS_INDEX = 0;
    INDOORS_INDEX_NEXT = 0;
    INDOORS_INDEX_FADE_EXT_DIR = 0;
    INDOORS_INDEX_FADE_EXT = 0;
    INDOORS_DBUILDING = 0;
#endif

    SetSeed(0);
    srand(1234567);
    GAME_STATE = GS_PLAY_GAME;

    extern std::int32_t quick_load;
    quick_load = 0;

    //
    // Initialise pause mode...
    //

    game_paused_key = -1;

    GAME_FLAGS &= ~GF_PAUSED;

    //
    // Initiliase the bullet points...
    //
#if !defined(PSX) && !defined(TARGET_DC)
    bullet_upto = -1;
    bullet_counter = 0;
#endif

    {
        //
        // Load the game.
        //
        //		ret=ELEV_load_user(go_into_game);
        ret = ELEV_load_name(mission_name);
    }

    return (ret);
}
#endif
#endif

//---------------------------------------------------------------

void game_fini() {
    // Stop any background music, or the disk drive thrashes horribly trying to load stuff.
    // And all the looping stuff from the game as well.
    TRACE("gf1 ");
    stop_all_fx_and_music();

    // Start the loading bar.
    ATTRACT_loadscreen_init();

    //
    // Free up the FASTPRIM memory.
    //
    TRACE("gf2 ");

    FASTPRIM_fini();

    //
    // Free up the SUPERFACET memory.
    //

    TRACE("gf4 ");

    SUPERFACET_fini();

    //
    // Free up the FARFACET memory.
    //

    TRACE("gf5 ");

    FARFACET_fini();

    TRACE("gf6 ");

    // Free up the figure caches.
    void FIGURE_clean_all_LRU_slots();
    FIGURE_clean_all_LRU_slots();

    //
    // Unload our sound-effects.
    //

    TRACE("gf7 ");

    if (GAME_STATE != GS_REPLAY) {
        // Don't free if replaying mission - no need.
        MFX_free_wave_list();
    }

    //	Close the game record/playback file.
#if !defined(PSX) && !defined(TARGET_DC)
    if (playback_file) {
        FileClose(playback_file);
        playback_file = nullptr;
    }

    if (verifier_file) {
        FileClose(verifier_file);
        verifier_file = nullptr;
    }
#endif

    TRACE("gf8 ");

#ifdef TARGET_DC
    // Unload the non-frontend textures - unless we are replaying the mission.
    if (GAME_STATE != GS_REPLAY) {
        TEXTURE_free_unneeded();
    }

    TRACE("gf9 ");

    // Clear out all the rendering pages' VBs and IBs.
    extern void POLY_ClearAllPages();
    POLY_ClearAllPages();

    TRACE("gf10 ");

    // And reload the frontend caches.
    if (GAME_STATE != GS_REPLAY) {
        the_display.InitBackCache();
    }

    // And load the frontend again.

    TRACE("gf11 ");

    if (GAME_STATE != GS_REPLAY) {
        TEXTURE_load_needed("levels\\frontend.ucm", 0, 256, 65);
    }
#endif

    NotGoingToLoadTexturesForAWhileNowSoYouCanCleanUpABit();

    TRACE("game_fini done\n");
}

//---------------------------------------------------------------

// extern std::uint32_t	get_hardware_input(std::uint16_t type);

void game() {
    game_startup();

#ifndef PSX
#ifdef VERSION_DEMO
    InitBackImage("demotitle.tga");

    for (;;) {
        MSG msg;

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
#ifdef TARGET_DC
            std::uint32_t input = get_hardware_input(INPUT_TYPE_JOY);
#else
            std::uint32_t input = get_hardware_input(INPUT_TYPE_JOY) | get_hardware_input(INPUT_TYPE_KEY);
#endif

            if (input & (INPUT_MASK_JUMP | INPUT_MASK_START | INPUT_MASK_SELECT | INPUT_MASK_KICK | INPUT_MASK_PUNCH | INPUT_MASK_ACTION)) {
                break;
            }
        }

        ShowBackImage(false);
        the_display.Flip(nullptr, DDFLIP_WAIT);
    }

    ResetBackImage();

#endif
#endif

#ifdef TARGET_DC
    // Display the Eidos and MF FMVs.
    TRACE("Playing intro FMVs\n");
    // Softdec Codec
    // MUST NOT BE EXITABLE WITH JOYPAD BUTTONS
    the_display.RunCutscene(-3, 0 /*ENV_get_value_number("lang_num", 0, "" )*/, false);
    // Eidos
    the_display.RunCutscene(-2, 0 /*ENV_get_value_number("lang_num", 0, "" )*/);
    // Muckyfoot
    the_display.RunCutscene(-1, 0 /*ENV_get_value_number("lang_num", 0, "" )*/);

#if 0
#ifdef DEBUG
	// Play all the movies, to check quality, etc.
	the_display.RunCutscene( 0, 0 );
	the_display.RunCutscene( 1, 0 );
	the_display.RunCutscene( 2, 0 );
	the_display.RunCutscene( 3, 0 );
	the_display.RunCutscene( 0, 1 );
	the_display.RunCutscene( 1, 1 );
	the_display.RunCutscene( 2, 1 );
	the_display.RunCutscene( 3, 1 );
#endif
#endif
#endif

    while (SHELL_ACTIVE && GAME_STATE) {
#ifndef PSX
        game_attract_mode();

#ifdef EDITOR

        if (GAME_STATE & GS_EDITOR) {
            if (editor_loop())
                GAME_STATE = 0;
            else {
                //				global_load();  //editor screws up the global load

                GAME_STATE = GS_ATTRACT_MODE;
            }
        }
#endif
#endif
        if (GAME_STATE & GS_PLAY_GAME) {
#ifndef PSX
            if (game_loop())
                GAME_STATE = 0;
            else
                GAME_STATE = GS_ATTRACT_MODE;
#else
            game_loop();
            GAME_STATE = 0;
#endif
        }

#ifndef TARGET_DC
#ifndef PSX
        if (GAME_STATE & GS_CONFIGURE_NET) {
            if (CNET_configure())
                GAME_STATE = 0;
            else
                GAME_STATE = GS_ATTRACT_MODE;
        }
#endif
#endif
    }

#ifndef PSX
#ifdef VERSION_DEMO
    InitBackImage("demoendscreen.tga");

    for (;;) {
        MSG msg;

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
#ifdef TARGET_DC
            std::uint32_t input = get_hardware_input(INPUT_TYPE_JOY);
#else
            std::uint32_t input = get_hardware_input(INPUT_TYPE_JOY) | get_hardware_input(INPUT_TYPE_KEY);
#endif
            if (input & (INPUT_MASK_JUMP | INPUT_MASK_START | INPUT_MASK_SELECT | INPUT_MASK_KICK | INPUT_MASK_PUNCH | INPUT_MASK_ACTION)) {
                break;
            }
        }

        ShowBackImage(false);
        the_display.Flip(nullptr, DDFLIP_WAIT);
    }

    ResetBackImage();
#endif
#endif

    TRACE("game before shutdown\n");

    game_shutdown();

    TRACE("game after shutdown\n");
}

//---------------------------------------------------------------

#define TAB_MAP_MIN_X 9
#define TAB_MAP_MIN_Z 13

#define TAB_MAP_SIZE 448

#ifndef PSX
#ifndef TARGET_DC
void GAME_map_draw_old() {
    Thing *darci = NET_PERSON(0);

    std::int32_t x, z, dx, dz, ndx, ndz, angle;

    POLY_frame_init(false, false);
    ShowBackImage();
    the_display.lp_D3D_Viewport->Clear(1, &the_display.ViewportRect, D3DCLEAR_ZBUFFER);

    x = darci->WorldPos.X >> 8;
    z = darci->WorldPos.Z >> 8;

    x = (x * TAB_MAP_SIZE) >> 15;
    z = (z * TAB_MAP_SIZE) >> 15;

    angle = darci->Draw.Tweened->Angle;

    dx = -SIN(angle);
    dz = -COS(angle);
    ndx = -dz;
    ndz = dx;

    dx >>= 12;
    dz >>= 12;
    ndx >>= 14;
    ndz >>= 14;

    x += TAB_MAP_MIN_X;
    z += TAB_MAP_MIN_Z;
    AENG_draw_col_tri(x + ndx, z + ndz, 0xff0000, x + dx, z + dz, 0xff0000, x - ndx, z - ndz, 0xff0000, 0);

    POLY_frame_draw(false, true);
}
#endif
#endif

extern void overlay_beacons();

#ifdef PSX
/*
extern void plan_view_shot(std::int32_t wx,std::int32_t wz,std::int32_t pixelw,std::int32_t sx,std::int32_t sy,std::int32_t w,std::int32_t h);

void GAME_map_draw()
{
        Thing *darci = NET_PERSON(0);

        DrawSync(0);
        ClearOTag(the_display.CurrentDisplayBuffer->ot, OTSIZE);

        overlay_beacons();
        plan_view_shot(darci->WorldPos.X>>8,darci->WorldPos.Z>>8,10,10,10,492,236);
}
*/
#else

#ifndef TARGET_DC

std::uint8_t screen_mem[640 * 3][480];

void GAME_map_draw() {
    Thing* darci = NET_PERSON(0);

    plan_view_shot(darci->WorldPos.X >> 8, darci->WorldPos.Z >> 8, 1 + (MouseY >> 4), 77, 78, 401, 328, (std::uint8_t*) screen_mem);
    overlay_beacons();
    the_display.create_background_surface((std::uint8_t*) screen_mem);
    the_display.blit_background_surface();
    the_display.destroy_background_surface();
}
#endif

#endif

#ifdef PSX
std::int32_t tick1;
std::int32_t tick2;
std::int32_t timet;

std::int32_t cam_x;
std::int32_t cam_y;
std::int32_t cam_z;

std::int32_t cam_yaw;
std::int32_t cam_pitch;
std::int32_t cam_roll;

#else

bool leave_map_form_proc(Form* form, Widget* widget, std::int32_t message) {
    if (widget && widget->methods == &BUTTON_Methods && message == WBN_PUSH) {
        form->returncode = widget->tag;

        return true; // Exit
    } else {
        return false; // Don't exit
    }
}

#endif

extern void PANEL_draw_timer_do(std::int32_t time, std::int32_t x, std::int32_t y);

std::int32_t already_warned_about_leaving_map;
std::uint8_t draw_map_screen = 0;
std::uint8_t single_step = 0;
Form *form_leave_map = nullptr;
std::int32_t form_left_map = 0;

//****************************************************************
//****************************************************************
// You are now entering the cleanup zone
// These functions are made from code ripped from game_loop
// in the hope of making game_loop() readable
//****************************************************************
//****************************************************************

//
// don't let the game run faster than this framerate by making you sit in a check the clock loop
//
void lock_frame_rate(std::int32_t fps) {
#if !defined(PSX) && !defined(TARGET_DC)
    static std::int32_t tick1 = 0;
    std::int32_t tick2;
    std::int32_t timet;

    while (1) {
        tick2 = GetTickCount();
        timet = tick2 - tick1;

        if (timet > (1000 / fps)) {
            break;
        }
    }
    tick1 = tick2;
#endif
}

void demo_timeout(std::int32_t flag) {
#if !defined(PSX) && !defined(TARGET_DC)
#if TIMEOUT_DEMO

    static std::int32_t time_start = 0;
    static std::int32_t timeout = 0;
    std::int32_t time_now;

    if (flag) {
        time_start = GetTickCount();
        timeout = ENV_get_value_number("timeout", 300) * 1000;
    } else {
        time_now = GetTickCount();

        if (time_now - time_start > timeout) {
            GAME_STATE = 0;
        }
    }
#endif
#endif
}

//
// Move off the edge of map test and dodgy widget stuff
//
#if 0
void edge_map_warning(std::int32_t flag)
{
#ifndef PSX
	Widget *widget_text;
	Widget *widget_yes;
	Widget *widget_no;
	std::int32_t  dx;
	std::int32_t  dz;
	Thing *darci = NET_PERSON(0);

	dx = darci->WorldPos.X >> 16;
	dz = darci->WorldPos.Z >> 16;

	if (dx == 2 || dx == PAP_SIZE_HI - 3 ||
		dz == 2 || dz == PAP_SIZE_HI - 3)
	{
		if (already_warned_about_leaving_map < GetTickCount())
		{
			//
			// Only warn once every 10 seconds
			//

			already_warned_about_leaving_map = GetTickCount() + 10000;

			//
			// Build the dialog box.
			//

			form_leave_map = FORM_Create(
								"Leave map?",
								leave_map_form_proc,
								0, 0,
								DisplayWidth,
								DisplayHeight,
								0xffffffff);

			FORM_AddWidget(
				form_leave_map,
				WIDGET_Create(
				   &STATIC_Methods,
					0, 50, 
					DisplayWidth,
					100,
					"Leaving the map will"));

			FORM_AddWidget(
				form_leave_map,
				WIDGET_Create(
				   &STATIC_Methods,
					0, 50 + 30 * 1, 
					DisplayWidth,
					100 + 30 * 1,
					"abort the mission."));

			FORM_AddWidget(
				form_leave_map,
				WIDGET_Create(
				   &STATIC_Methods,
						0, 50 + 40 * 2,  
					DisplayWidth,
					100 + 40 * 2,
					"Abort mission?"));

			widget_yes = WIDGET_Create(
						   &BUTTON_Methods,
							0, DisplayHeight - 270, 
							DisplayWidth,
							DisplayHeight,
							"Yes");

			widget_no = WIDGET_Create(
						   &BUTTON_Methods,
							0, DisplayHeight - 200, 
							DisplayWidth,
							DisplayHeight,
							"No");

			widget_yes->tag = 2;
			widget_no ->tag = 1;

			FORM_AddWidget(
				form_leave_map,
				widget_yes);

			FORM_AddWidget(
				form_leave_map,
				widget_no);
		}
	}

	if (dx == 0 || dx == PAP_SIZE_HI - 1 ||
		dz == 0 || dz == PAP_SIZE_HI - 1)
	{
		//
		// Abandon level!
		// 

		GAME_STATE = GS_LEVEL_LOST;
	}
#endif
}
#endif

//
// Provided in two parts
//
void do_leave_map_form() {
#ifndef PSX
    std::int32_t ret;

    form_left_map = 15;

    POLY_frame_init(false, false);

    ret = FORM_Process(form_leave_map);

    if (ret) {
        FORM_Free(form_leave_map);

        form_leave_map = nullptr;

        if (ret == 2) {
            GAME_STATE = 0;
        }

        {
            Thing *darci = NET_PERSON(0);

            if (darci->Genus.Person->Flags & FLAG_PERSON_DRIVING) {
                Thing *p_vehicle = TO_THING(darci->Genus.Person->InCar);

                ASSERT(p_vehicle->Class == CLASS_VEHICLE);

                p_vehicle->Velocity = 0;
                p_vehicle->Genus.Vehicle->VelX = 0;
                p_vehicle->Genus.Vehicle->VelZ = 0;
                p_vehicle->Genus.Vehicle->VelR = 0;
            }

#ifdef BIKE

            if (darci->Genus.Person->Flags & FLAG_PERSON_BIKING) {
                Thing *p_bike = TO_THING(darci->Genus.Person->InCar);

                ASSERT(p_bike->Class == CLASS_BIKE);

                p_bike->Velocity = 0;
                p_bike->Genus.Bike->back_dx = 0;
                p_bike->Genus.Bike->back_dy = 0;
                p_bike->Genus.Bike->back_dz = 0;

                p_bike->Genus.Bike->steer = 0;
                p_bike->Genus.Bike->accel = 0;
            }

#endif
        }
    } else {
        FORM_Draw(form_leave_map);

        POLY_frame_draw(false, false);
    }
#endif
}

//
// psx camera stuff (the PC does it in the engine?)
//
std::int32_t psx_camera() {
    //
    // AENG_draw() understands about cameras now because it has to
    // handle the splitscreen mode properly.
    //

    /*
    // snip   ( a load of camera stuff check out sourcesafe pre 20th may to see (MikeD)
    */

#ifdef PSX
    std::int32_t cx;
    std::int32_t cy;
    std::int32_t cz;

    std::int32_t ay;
    std::int32_t ap;
    std::int32_t ar;

    std::int32_t lens;
    std::int32_t warehouse;

    if (EWAY_grab_camera(
            &cx,
            &cy,
            &cz,
            &ay,
            &ap,
            &ar,
            &lens)) {
        //
        // We are using the cut-scene camera.
        //
        warehouse = EWAY_camera_warehouse();
    } else {
        //
        // We are using the normal FC camera.
        //

        cx = FC_cam[0].x;
        cy = FC_cam[0].y;
        cz = FC_cam[0].z;

        ay = FC_cam[0].yaw;
        ap = FC_cam[0].pitch;
        ar = FC_cam[0].roll;
        warehouse = (FC_cam[0].focus->Class == CLASS_PERSON && FC_cam[0].focus->Genus.Person->Ware);
    }

    AENG_set_camera_radians(
        cx >> 8,
        cy >> 8,
        cz >> 8,
        (-(ay >> 8)) & 2047,
        ap >> 8,
        ar >> 8);

    return warehouse;
#else
    return (0);
#endif
    return 0; // <-- otherwise error C4716: 'psx_camera' : must return a value
}

//
// Get what yoiu have drawn onto the Screen
//
inline void screen_flip() {
    //
    //	sCREENSHOT just before screen_flip
    //
#ifndef PSX
    extern void AENG_screen_shot();
    AENG_screen_shot();
#else
    extern void DoFigureDraw();
    DoFigureDraw();

#ifndef FS_ISO9660
    extern void AENG_screen_shot(std::int32_t width);
    if (Keys[KB_S]) {
        AENG_screen_shot(320);
        Keys[KB_S] = 0;
    }
#endif
#endif

#ifndef PSX
    //
    // any debug text required
    //
    if (ControlFlag && allow_debug_keys) {
        AENG_draw_messages();
        FONT_buffer_draw();
    }
    /*
            if (Keys[KB_F] && allow_debug_keys)
            {
                    AENG_draw_FPS();
            }
    */

    //
    // Blitting is faster... but looks SHITE on the 3DFX because
    // it doesn't have a hardware blitter from VIDEO to VIDEO.
    //

#ifdef TARGET_DC
    // DO THE DAMN FLIP!
    AENG_flip();
#else
    if (the_display.GetDriverInfo()->IsPrimary()) {
        PreFlipTT();
        AENG_blit();
    } else {
        AENG_flip();
    }
#endif

//	FLIP(nullptr,DDFLIP_WAIT);
#else
    //
    // Always flip on the PSX.
    //

    extern std::uint8_t psx_motor[];

    if (GAME_STATE & (GS_LEVEL_WON | GS_LEVEL_LOST)) {
        psx_motor[0] = 0;
        psx_motor[1] = 0;
    }

    AENG_flip();
#endif
}

void playback_game_keys() {
    if (Keys[KB_SPACE] || Keys[KB_ENTER] || Keys[KB_PENTER]) {
        Keys[KB_SPACE] = 0;
        Keys[KB_ENTER] = 0;
        Keys[KB_PENTER] = 0;

        GAME_STATE = 0;
    }

#ifndef PSX
    if (ReadInputDevice()) {
        std::int32_t i;

        for (i = 0; i <= 9; i++) {
            if (the_state.rgbButtons[i]) {
                GAME_STATE = 0;
            }
        }
    }
#endif
}

//
// For those funny fanny keys the PC likes to use
//

std::int32_t special_keys() {
#ifdef EDITOR
    if (ControlFlag && Keys[KB_E]) {
        GAME_STATE = GS_EDITOR;
    }
#endif

    if (GAME_STATE & GS_PLAYBACK) {
        playback_game_keys();
    }

    if (ControlFlag && Keys[KB_Q]) {
        return 1;
    }
#ifndef PSX
    if (allow_debug_keys)
        if (Keys[KB_QUOTE]) {
            Keys[KB_QUOTE] = 0;
            single_step ^= 1;
        }

    if (single_step) {
        if (Keys[KB_COMMA]) {
            Keys[KB_COMMA] = 0;

            process_things(0);
        }
    }
#endif
    /*

    if (Keys[KB_TAB] || (NET_PLAYER(0)->Genus.Player->Pressed & INPUT_MASK_START))
    {
            Keys[KB_TAB] = 0;

            draw_map_screen ^= true;
    }

    */

    return (0);
}

//
// Sound stuff, ask Matt
//
void handle_sfx() {
    //	MUSIC_process();
    MUSIC_mode_process(); // new version, much better

    std::int32_t cx, cy, cz, ay, ap, ar, lens;

    std::int32_t x = NET_PERSON(PLAYER_ID)->WorldPos.X,
                 y = NET_PERSON(PLAYER_ID)->WorldPos.Y,
                 z = NET_PERSON(PLAYER_ID)->WorldPos.Z;

    if (EWAY_grab_camera(&cx, &cy, &cz, &ay, &ap, &ar, &lens)) {
        MFX_set_listener(x, y, z, -(ay >> 8), -(ar >> 8), -(ap >> 8));
    } else {
        MFX_set_listener(x, y, z, -(FC_cam[0].yaw >> 8), -(FC_cam[0].roll >> 8), -(FC_cam[0].pitch >> 8));
    }

#ifndef PSX
    process_ambient_effects();
    process_weather();
#else
    {
        // heartbeat when you die
        static bool only_once = false;
        if (NET_PERSON(0)->State == STATE_DEAD) {
            if (!only_once) MFX_play_thing(THING_INDEX(NET_PERSON(0)), S_HEART_FAIL, 0, NET_PERSON(0));
            only_once = true;
        } else {
            only_once = false;
        }
    }

    if (wad_level == 14) {
        std::int32_t ware = NET_PERSON(PLAYER_ID)->Genus.Person->Ware;

        if (ware && (WARE_ware[ware].ambience == 4)) {
            MUSIC_bodge_code = 8;
        } else
            MUSIC_bodge_code = 0;
    }
#endif

    extern std::int32_t BARREL_fx_rate;
    if (BARREL_fx_rate > 1)
        BARREL_fx_rate -= 2;
    else
        BARREL_fx_rate = 0;
    MFX_update();
}

std::int32_t should_i_process_game() {
    if (EWAY_tutorial_string) {
        //
        // Pause the game while a tutorial message appears.
        //

        return false;
    }

#ifndef PSX

    if (GAMEMENU_is_paused()) {
        return false;
    }

    return true;

    if (!(GAME_FLAGS & (GF_PAUSED | (GF_SHOW_MAP * 0))) && !form_leave_map /* && GAME_STATE != GS_LEVEL_LOST && GAME_STATE != GS_LEVEL_WON*/)
        return (1);
#else

    extern std::int32_t PSX_inv_open;

    if (PSX_inv_open) {
        return (0);
    }

    if (GAME_FLAGS & GF_PAUSED)
        return 0;

    if ((GAME_STATE != GS_LEVEL_LOST) && (GAME_STATE != GS_LEVEL_WON))
        return (1);

    if (PSX_eog_timer) {
        PSX_eog_timer--;
        return (1);
    }

#endif
    return (0);
}

//
// engine or map, psx or pc, these are some of the questions we shall be resolving at compile time and runtime in the coming lines of code
//

void draw_debug_lines();

inline void draw_screen() {
    extern std::int32_t draw_3d;

#ifndef PSX

#ifdef DEBUG
    draw_debug_lines();
#endif

    if (draw_map_screen) {
        //		MAP_draw();
    } else {
        AENG_draw(draw_3d);
    }

    if (form_leave_map) {
        do_leave_map_form();
    }

#else
#ifdef TOPMAP_BACK
    if (draw_map_screen) {
        extern void MAP_draw();
        MAP_draw();

    } else
#endif
    {
        AENG_draw(draw_3d);
    }
#endif
}

//****************************************************************
//					End of cleanup_zone, have a nice day
//****************************************************************
std::int32_t hardware_input_replay() {
#ifdef PSX

    //
    // input that triggers a mision to be replayed (only triggers if level_lost
    //

    if (PadKeyIsPressed(&PAD_Input1, PAD_Current->data[4].pad_button)) {
        return (1 - replay_waiting);
    }
    replay_waiting = 0;

#else
    if (LastKey == KB_R) {
        LastKey = 0;
        return (1);
    }

#endif
    return (0);
}

std::int32_t hardware_input_continue() {
#ifdef PSX
    //
    // input that triggers you to continue with default action (level_won is go to next, level_lost is replay)
    //
    if (PadKeyIsPressed(&PAD_Input1, PAD_RD)) {
        return (1 - continue_waiting);
    } else
        continue_waiting = 0;

#else

    extern std::int32_t GAMEMENU_menu_type;
    if (GAMEMENU_menu_type == 0 /*GAMEMENU_MENU_TYPE_NONE*/) {
        // No pause menu up, so wait for a keypress.
#ifdef TARGET_DC
        std::int32_t input = get_hardware_input(INPUT_TYPE_JOY | INPUT_TYPE_REMAP_DPAD | INPUT_TYPE_REMAP_BUTTONS | INPUT_TYPE_GONEDOWN);
        if (input & (INPUT_MASK_DOMENU | INPUT_MASK_CANCEL))
#else
        std::int32_t input = get_hardware_input(INPUT_TYPE_ALL);
        if (LastKey == KB_SPACE || LastKey == KB_ESC || LastKey == KB_Z || LastKey == KB_X || LastKey == KB_C || LastKey == KB_ENTER || (input & (INPUT_MASK_SELECT | INPUT_MASK_PUNCH | INPUT_MASK_JUMP)))
#endif
        {
            LastKey = 0;

            return (1);
        }
    }

#endif
    return (0);
}

/*
std::uint16_t darci_dlight;
*/

#ifndef PSX
std::uint16_t last_fudge_message;
std::uint16_t last_fudge_camera;
std::uint8_t the_end;
#endif

#ifdef PSX
#ifdef VERSION_DEMO
extern std::int32_t game_timeout;
#endif
#endif

std::uint16_t env_frame_rate;

std::uint8_t game_loop() {
#ifndef PSX
#ifndef TARGET_DC
    extern void save_all_nads();
    extern std::int32_t save_psx;
    if (save_psx == 2)
        save_all_nads();
#endif
#endif

#ifndef PSX
#ifndef TARGET_DC
    env_frame_rate = ENV_get_value_number("max_frame_rate", 30, "Render");
#endif
    AENG_set_draw_distance(ENV_get_value_number("draw_distance", 22, "Render"));
#endif
round_again:;

#ifndef NDEBUG
    if (!(GAME_STATE & GS_PLAYBACK)) {
        //
        // allways record a game, good for debuging
        //

#ifndef TARGET_DC
        GAME_STATE |= GS_RECORD;
#endif
    }
#endif

#ifdef PSX
    continue_waiting = 1;
    replay_waiting = 1;
#else
#ifndef TARGET_DC
    MEMORY_quick_init();
#endif
#endif

    TRACE("game_loop init1\n");

    if (game_init()) {
        TRACE("game_loop init2\n");

        already_warned_about_leaving_map = GetTickCount();
        draw_map_screen = false;
        form_leave_map = nullptr;
        form_left_map = 0;
#ifndef PSX
        LastKey = 0;
        last_fudge_message = 0;
        last_fudge_camera = 0;
#endif

        demo_timeout(1);

#ifndef PSX
        PANEL_fadeout_init();
        GAMEMENU_init();
#endif

        // start timing
        if (GAME_STATE & GS_PLAYBACK)
            BreakStart();
#ifndef PSX
        std::int32_t exit_game_loop = false;

        TRACE("game_loop init3\n");

#ifdef TARGET_DC
        // Sod 'em on the PC - they just cause problems when converting levels.
        // Sod it on the DC as well - very low on memory.
        // SUPERCRINKLE_init();
#endif

        //
        // Initialise the SUPERFACET cache system. Allocates memory.
        //

        TRACE("game_loop init4\n");

        SUPERFACET_init();

        //
        // Initailises the FARFACET system. Allocates memory.
        //

        TRACE("game_loop init5\n");

        FARFACET_init();

        //
        // Initialises the FASTPRIM cached prim system. Allocates memory.
        //

        TRACE("game_loop init6\n");

        FASTPRIM_init();

        TRACE("game_loop init7\n");

        // MarkZA says put this in here.
        extern void envmap_specials();
        envmap_specials();

#endif

        TRACE("game_loop init8\n");

        while (SHELL_ACTIVE && (GAME_STATE & (GS_PLAY_GAME | GS_LEVEL_LOST | GS_LEVEL_WON))) {
#ifdef TARGET_DC
            extern bool g_bPunishMePleaseICheatedOnThisLevel;
            extern int g_iCheatNumber;
            if (g_iCheatNumber == 0xd01e7e1) {
                // Complete this level.
                g_bPunishMePleaseICheatedOnThisLevel = true;
                GAME_STATE = GS_LEVEL_WON;
                g_iCheatNumber = 0;
            }
#endif

#ifndef PSX
            if (!exit_game_loop) {
                exit_game_loop = GAMEMENU_process();
            }

#ifdef TARGET_DC
            if (g_bDreamcastABXYStartComboPressed) {
                // Bin this game - get back to the main menu.
                // er... actually, get back to the title screen, according to Sega standards.
                // g_bDreamcastABXYStartComboPressed = false;
                GAME_STATE = GS_LEVEL_LOST;
                exit_game_loop = GAMEMENU_DO_CHOOSE_NEW_MISSION;
            }
#endif

            if (exit_game_loop) {
                PANEL_fadeout_start();
            }

            if (PANEL_fadeout_finished()) {
                switch (exit_game_loop) {
                case GAMEMENU_DO_NOTHING:
                    break;

                case GAMEMENU_DO_RESTART:
                    GAME_STATE = GS_REPLAY;
                    break;

                case GAMEMENU_DO_CHOOSE_NEW_MISSION:
                    GAME_STATE = GS_LEVEL_LOST;
                    break;

                case GAMEMENU_DO_NEXT_LEVEL:
                    GAME_STATE = GS_LEVEL_WON;
                    break;

                default:
                    ASSERT(0);
                    break;
                }

                break;
            }

            if (GAME_STATE & GS_LEVEL_WON) {
                //
                // Exit out of the last mission straight away.
                //

                extern std::int32_t playing_level(const char *name);

                if (playing_level("Finale1.ucm")) {
                    GAME_STATE = GS_LEVEL_WON;
                    break;
                }
            }
#else

            if (GAME_STATE & (GS_LEVEL_LOST | GS_LEVEL_WON)) {
                //
                // Exiting out of a mission...
                //

                if (GAME_STATE & GS_LEVEL_LOST) {
                    // MUSIC_play(S_DEAD,MUSIC_FLAG_LOOPED|MUSIC_FLAG_OVERRIDE);
                    if (NET_PERSON(0)->Genus.Person->Health > 0)
                        MUSIC_mode(MUSIC_MODE_CHAOS);
                    else
                        MUSIC_mode(MUSIC_MODE_GAMELOST);

                    if (hardware_input_replay()) {
                        {

#ifdef PSX
                            // Sony want's triangle to return us to the main menu
                            // rather than 'X' as we originally planned (and which
                            // is on the face of it the intuitive one, and consistant)
                            // Oh well, if Sony Insists I'll arse about with the
                            // code to make it work.
#ifndef VERSION_DEMO
                            GAME_STATE = GS_LEVEL_LOST;
#else
                            GAME_STATE = 0;
#endif
#else
                            GAME_STATE = GS_REPLAY;
#endif
                            break;
                        }
                    }
                    if (hardware_input_continue()) {
#ifdef PSX
                        // The reason this is all cocked up is because apparently
                        // Sony want us to use Triange to Return to the Menu
                        // Whereas on the PC it's SPACE like the level won
                        // option. Where is my consistancy?
#ifndef VERSION_DEMO
                        GAME_STATE = GS_REPLAY;
#else
                        GAME_STATE = 0;
#endif
#else
                        GAME_STATE = GS_LEVEL_LOST;
#endif
                        break;
                    }
                }
                if (GAME_STATE & GS_LEVEL_WON) {
                    // MUSIC_play(S_LEVEL_COMPLETE,MUSIC_FLAG_LOOPED|MUSIC_FLAG_OVERRIDE);
                    MUSIC_mode(MUSIC_MODE_GAMEWON);
                    if (hardware_input_continue()) {
#ifndef VERSION_DEMO
                        GAME_STATE = GS_LEVEL_WON;
#else
                        GAME_STATE = 0;
#endif
                        break;
                    }
                }
#ifdef VERSION_DEMO
                if (game_timeout == 0) {
                    GAME_STATE = 0;
                    break;
                }
                game_timeout--;
#endif
            }
#endif

            if (EWAY_tutorial_string) {
                EWAY_tutorial_counter += 64 * TICK_RATIO >> TICK_SHIFT;

                if (EWAY_tutorial_counter > 64 * 20 * 2) {
                    if (hardware_input_continue()) {
                        EWAY_tutorial_string = nullptr;

                        NET_PERSON(0)->Genus.Person->Flags &= ~(FLAG_PERSON_REQUEST_KICK | FLAG_PERSON_REQUEST_PUNCH | FLAG_PERSON_REQUEST_JUMP);
                        NET_PLAYER(0)->Genus.Player->InputDone = -1;
                    }
                } else {
                    if (hardware_input_continue()) {
                        EWAY_tutorial_counter = 64 * 20 * 2;
                    }
                }
            }

            //
            // if TIMEOUT_DEMO is defined will exit the game after delay specified in config.ini
            //
            demo_timeout(0);

            //
            // some pc keys, single step etc
            //
            if (special_keys())
                return (1);

            /*
            if (darci_dlight)
            {
                    Thing *darci = NET_PERSON(0);

                    NIGHT_dlight_move(
                            darci_dlight,
                            (darci->WorldPos.X >> 8),
                            (darci->WorldPos.Y >> 8) + 0x80,
                            (darci->WorldPos.Z >> 8));
            }
            */

            //
            // stuff PSX needs, does nothing on PC
            //
            // JDW: This has been moved into AENG_draw where it can do more good, it
            // now calculates if the camera is in a warehouse for me (like it should
            // have done in the first case, and returns true if it is.
            //			psx_camera();

            //
            // Warn the player if they go too near the edge of the map.
            //
            // don't do it anymore			edge_map_warning(0);

            //
            // Some processing and keyboard input for debug stuff
            //

            if (!(GAME_STATE & (GS_LEVEL_LOST | GS_LEVEL_WON)) && !EWAY_tutorial_string) {
                //				TRACE("Process Controls");

                process_controls();
            }
            void check_pows();
            check_pows();

            //
            // Process stuff
            //
            if (should_i_process_game()) {
                if (!single_step) {
                    //					TRACE("Process things\n");

                    process_things(1);
                }

                //				TRACE("Process Stuff1\n");
                PARTICLE_Run();
                OB_process();
                TRIP_process();
                DOOR_process();

#ifndef TARGET_DC
                TRACE("Eway process\n");
#endif

                EWAY_process();
#ifndef PSX

                //				TRACE("Process stuff2\n");

                SPARK_show_electric_fences();
                RIBBON_process();
                DIRT_process();
                ProcessGrenades();
#ifndef TARGET_DC
                WMOVE_draw();
                BALLOON_process();
#endif
                MAP_process();
#endif
                POW_process();
                FC_process();

            } else {
#ifdef PSX
                extern void do_packets();

                do_packets();
#endif
            }

            //
            // Always process these...
            //

#ifndef PSX
            {
#ifndef TARGET_DC
                PUDDLE_process();
#endif
                // Still need to have drips on DC (for hydrants, pissing, etc.
                DRIP_process();
            }
#endif

            //
            // Draw the game or map or whatever is going on
            //
            BreakTime("Done thing processing");

            //			TRACE("Draw screen\n");

            draw_screen();

            //
            // Draw panel and other exciting things
            //

            OVERLAY_handle();

            // pausing and floating text stuff

            std::int32_t i_want_to_exit = false;

#ifdef PSX

            if (PAUSE_handler()) {
                i_want_to_exit = true;
            }

#endif

            //
            // On screen Text
            //
            // #ifndef FINAL
            if (!(GAME_FLAGS & GF_PAUSED))
                CONSOLE_draw();
                // #endif

#ifndef PSX
            GAMEMENU_draw();

            //
            // Fading out.
            //

            PANEL_fadeout_draw();
#endif

            //
            // Lets show this stuff on the monitor
            //
            BreakTime("About to flip");

            screen_flip();

            BreakTime("Done flip");

            BreakFrame();
#ifdef MIKE
            if (Keys[KB_TAB]) {
                BreakEnd("C:\\Windows\\Desktop\\BreakTimes.txt");
                Keys[KB_TAB] = 0;
            }
#endif

            //
            // Lock frame-rate to 30 FPS
            //
#ifndef PSX
#ifndef TARGET_DC
#ifndef BREAKTIMER
            lock_frame_rate(env_frame_rate);
#endif
#endif
#endif

            //
            // process moveing sfx, ambient stuff etc
            //
            handle_sfx();

            GAME_TURN++;

            if ((GAME_TURN & 0x3ff) == 314) {
                //
                // Clearing this flag lets Darci get health when she sits on
                // a bench.
                //

                GAME_FLAGS &= ~GF_DISABLE_BENCH_HEALTH;

#ifndef NDEBUG

                // PANEL_new_text(NET_PERSON(0), 1000, "I could do with sitting on a bench!");

#endif
            }

            if (i_want_to_exit) {
                break;
            }
        }

        // end timing
        BreakEnd("C:\\Windows\\Desktop\\BreakTimes.txt");

        //
        // Game has finished, what do we do now
        //

        TRACE("game_loop game_fini\n");

        game_fini();

        TRACE("game_loop gf done\n");

        if (GAME_STATE == GS_LEVEL_WON) {
#ifndef PSX

            if (strstr(ELEV_fname_level, "park2.ucm")) {
                //
                // Time to play the MIB introduction cutscene.
                //

                stop_all_fx_and_music();
#ifdef TARGET_DC
                the_display.RunCutscene(1, ENV_get_value_number("lang_num", 0, ""));
#else
                the_display.RunCutscene(1);
#endif
            } else if (strstr(ELEV_fname_level, "Finale1.ucm")) {
                //
                // Game complete!
                //
                stop_all_fx_and_music();
#ifdef TARGET_DC
                the_display.RunCutscene(3, ENV_get_value_number("lang_num", 0, ""));
#else
                the_display.RunCutscene(3);
#endif

                //
                // Go into the outro.
                //

#ifndef TARGET_DC
                extern void OS_hack();

                the_end = true;

                OS_hack();
#else
                // Do the credits.
                // g_bGoToCreditsPleaseGameHasFinished = true;

                // Get the background loaded.
                void FRONTEND_scr_img_load_into_screenfull(char *name, CompressedBackground *screen);
                FRONTEND_scr_img_load_into_screenfull("title_blood1.tga", &(the_display.lp_DD_Background));
                extern LPDIRECTDRAWSURFACE4 lpBackgroundCache;
                ASSERT(lpBackgroundCache != nullptr);
                UnpackBackground((BYTE *) (the_display.lp_DD_Background), lpBackgroundCache);

                MUSIC_mode(MUSIC_MODE_FRONTEND);
                MUSIC_mode_process();
#endif

                the_end = false;
            } else
            //
            // Connected else...
            //

#endif

                if ((NETPERSON != nullptr) && (NET_PERSON(0) != nullptr) && (NET_PERSON(0)->Genus.Person->PersonType == PERSON_DARCI)) {
                if (NET_PLAYER(0)->Genus.Player->RedMarks > 1) {
                    char *mess;

                    InitBackImage("deadcivs.tga");

#ifdef TARGET_DC
                    // Do a grab to make the GONEDOWN thing grab current state.
                    // That way, if a button is held down, it won't skip past this screen.
                    get_hardware_input(INPUT_TYPE_JOY | INPUT_TYPE_REMAP_DPAD | INPUT_TYPE_REMAP_BUTTONS | INPUT_TYPE_REMAP_START_BUTTON | INPUT_TYPE_GONEDOWN);
#else
                    Keys[KB_ESC] = 0;
                    Keys[KB_SPACE] = 0;
                    Keys[KB_ENTER] = 0;
                    Keys[KB_PENTER] = 0;
#endif

                    while (SHELL_ACTIVE) {
#ifndef PSX
                        ShowBackImage();
                        POLY_frame_init(false, false);

                        switch (the_game.DarciDeadCivWarnings) {
                        case 0: mess = XLAT_str(X_CIVSDEAD_WARNING_1); break;
                        case 1: mess = XLAT_str(X_CIVSDEAD_WARNING_2); break;
                        case 2: mess = XLAT_str(X_CIVSDEAD_WARNING_3); break;

                        default:
                        case 3:
                            GAME_STATE = GS_LEVEL_LOST;

                            MENUFONT_Draw(
                                30, 320,
                                192,
                                XLAT_str(X_LEVEL_LOST),
                                0xffffffff,
                                0);

                            mess = XLAT_str(X_CIVSDEAD_WARNING_4);

                            break;
                        }

                        FONT2D_DrawStringWrapTo(mess, 32, 82, 0x000000, 256, POLY_PAGE_FONT2D, 0, 352);
                        FONT2D_DrawStringWrapTo(mess, 30, 80, 0xffffff, 256, POLY_PAGE_FONT2D, 0, 350);

                        // FONT2D_DrawStringWrap(mess, 10, 300, 0xffffffff);
                        POLY_frame_draw(true, true);
#else
                        extern char* Wadmenu_CivMess;
                        Wadmenu_CivMess = mess;
                        break;
#endif
                        AENG_flip();

#ifdef TARGET_DC
                        int input = get_hardware_input(INPUT_TYPE_JOY | INPUT_TYPE_REMAP_DPAD | INPUT_TYPE_REMAP_BUTTONS | INPUT_TYPE_REMAP_START_BUTTON | INPUT_TYPE_GONEDOWN);
                        if (0 != (input & (INPUT_MASK_START | INPUT_MASK_CANCEL | INPUT_MASK_DOMENU))) {
                            break;
                        }
#else
                        if (Keys[KB_ESC] ||
                            Keys[KB_SPACE] ||
                            Keys[KB_ENTER] ||
                            Keys[KB_PENTER]) {
                            break;
                        }
#endif
                    }

                    // Bin the memory again.
                    ResetBackImage();

#ifdef TARGET_DC
                    // Another one, again to get GONEDOWN working well.
                    get_hardware_input(INPUT_TYPE_JOY | INPUT_TYPE_REMAP_DPAD | INPUT_TYPE_REMAP_BUTTONS | INPUT_TYPE_REMAP_START_BUTTON | INPUT_TYPE_GONEDOWN);
#else
                    Keys[KB_ESC] = 0;
                    Keys[KB_SPACE] = 0;
                    Keys[KB_ENTER] = 0;
                    Keys[KB_PENTER] = 0;
#endif

                    the_game.DarciDeadCivWarnings += 1;
                }
            }
        }

        // game_fini();

        switch (GAME_STATE) {
        case 0:
#ifdef PSX
            extern void AENG_flip_init();
            AENG_flip_init();
            DrawSync(0);
#endif
            break;

        case GS_REPLAY:
            GAME_STATE = GS_PLAY_GAME | GS_REPLAY;
#ifdef PSX
            extern void AENG_flip_init();
            AENG_flip_init();
            DrawSync(0);
#endif

            goto round_again;
        case GS_LEVEL_WON:
#ifndef PSX
//				STARTSCR_notify_gameover(1);
#else
            AENG_flip_init();
            DrawSync(0);
            Wadmenu_gameover(1);
#endif
            break;
        case GS_LEVEL_LOST:
#ifndef PSX
//				STARTSCR_notify_gameover(0);
#else

            TRACE("game_loop LOST 1\n");
            AENG_flip_init();
            TRACE("game_loop LOST 2\n");
            DrawSync(0);
            TRACE("game_loop LOST 3\n");
            Wadmenu_gameover(0);
            TRACE("game_loop LOST 4\n");
#endif
            break;
        }

        // game_fini();

        NET_PERSON(0) = nullptr; // For the music system...

#ifndef PSX
        if (GAME_STATE == GS_LEVEL_WON) {
            FRONTEND_level_won();
        } else {
            TRACE("game_loop LOST 5\n");
            FRONTEND_level_lost();
            TRACE("game_loop LOST 6\n");
        }
#endif

        return 0;
    }

    NET_PERSON(0) = nullptr; // For the music system...

    return 1;
}

//---------------------------------------------------------------

// create SMOOTH_TICK_RATIO from TICK_RATIO averaged over 4 frames
// so the car doesn't jolt so badly

static std::int32_t tick_ratios[4];
static std::int32_t wptr;
static std::int32_t number;
static std::int32_t sum;

void ResetSmoothTicks() {
    wptr = 0;
    number = 0;
    sum = 0;
}

std::int32_t SmoothTicks(std::int32_t raw_ticks) {
    if (number < 4) {
        sum += raw_ticks;
        tick_ratios[wptr++] = raw_ticks;
        number++;
        if (number < 4) {
            return raw_ticks;
        }

        wptr = 0;
        return sum >> 2;
    }

    sum -= tick_ratios[wptr];
    tick_ratios[wptr] = raw_ticks;
    sum += raw_ticks;
    wptr = (wptr + 1) & 3;

    return sum >> 2;
}
