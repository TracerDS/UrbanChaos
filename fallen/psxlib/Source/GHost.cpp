// GHost.cpp
// Guy Simmons, 20th November 1997.

#include <MFStdLib.h>
#include "c:\fallen\psxlib\headers\gmem.h"
#include "libsn.h"
#include "libapi.h"
#include <sys\file.h>
#include "kernel.h"
#include "c:\fallen\headers\inline.h"
#include "c:\fallen\headers\interfac.h"
#include "ctrller.h"
#include "libcd.h"
#include <libpad.h>
#include "libgpu.h"

#include "myheap.h"
#include "psxeng.h"
#include "game.h"

#ifdef VERSION_PAL
#include "libcrypt.h"
#endif

#define PAUSE_TIMEOUT 500
#define PAUSE (1 << 0)
#define PAUSE_ACK (1 << 1)

volatile bool ShellActive;

volatile std::uint8_t AltFlag,
    ControlFlag,
    ShiftFlag;
volatile std::uint8_t Keys[256],
    LastKey;

extern std::int16_t music_current_level;
std::uint8_t Wadmenu_PadType;

#define WAD_SEL_LEVEL 0
#define WAD_SEL_CAN 1
#define WAD_LOADING 2
#define WAD_AUD_MONO 3
#define WAD_AUD_STEREO 4
#define WAD_PAD_NEWTARG 5
#define WAD_PAD_LEFT 6
#define WAD_PAD_RIGHT 7
#define WAD_PAD_BACK 8
#define WAD_PAD_KICK 9
#define WAD_PAD_PUNCH 10
#define WAD_PAD_ACTION 11
#define WAD_PAD_JUMP 12
#define WAD_PAD_FPMODE 13
#define WAD_PAD_RUN 14
#define WAD_CAM_LEFT 15
#define WAD_CAM_RIGHT 16
#define WAD_PAD_INV 17
#define WAD_PAD_PAUSE 18
#define WAD_CFG_NUMBER 19
#define WAD_CFG_FREE 20
#define WAD_VIB_ON 21
#define WAD_VIB_OFF 22
#define WAD_CFG_PRESS 23
#define WAD_PAD_CONFIG 24
#define WAD_SCR_ADJUST1 25
#define WAD_SCR_ADJUST2 26
#define WAD_SEL_SELECT 27

#define WAD_MENU_FXVOL 28
#define WAD_MENU_MUSVOL 29
#define WAD_MENU_RETURN 30

#define WAD_MENU_BRIGHT 31
#define WAD_MENU_POS 32

#define WAD_LANG_ENG 33
#define WAD_LANG_FRENCH 34
#define WAD_LANG_SPAN 35
#define WAD_LANG_ITAL 36

#define WAD_MENU_AUDIO 37
#define WAD_MENU_VIDEO 38
#define WAD_MENU_PAD 39
#define WAD_MENU_LANG 40

#define WAD_MENU_NEW 41
#define WAD_MENU_LOAD 42
#define WAD_MENU_CONFIG 43

#define WAD_MENU_TIMS 44

#define WAD_PAD_SNAP 45

#define WAD_MAX_ITEMS

//---------------------------------------------------------------

extern std::int32_t MFX_music_wave;
extern std::int32_t MFX_music_end;
extern std::int32_t MFX_music_queued;
extern std::int32_t MFX_music_gain;
extern std::int32_t MFX_music_q_flag;

int sfx_volume = 128;
int music_volume = 128;
int speech_volume = 128;
int sound_mode = 1;
int pad_config = 0;
int vibra_mode = 0;

#ifdef VERSION_ENGLISH
std::uint8_t IsEnglish = 1;
#endif

#ifdef VERSION_KOREA
std::uint8_t IsEnglish = 1;
#else
#ifdef VERSION_GERMAN
#ifndef VERSION_DEMO
std::uint8_t IsEnglish = 0;
#endif
#endif
#endif

#ifdef VERSION_SPANISH
std::uint8_t IsEnglish = 0;
#endif

#ifdef VERSION_ITALIAN
std::uint8_t IsEnglish = 0;
#endif

#ifdef VERSION_USA
std::uint8_t IsEnglish = 1;
#endif

#ifdef VERSION_JAPAN
std::uint8_t IsEnglish = 1;
#endif

#ifdef VERSION_NTSC
int screen_x = -6;
int screen_y = 1;
#else
int screen_x = 0;
int screen_y = 30;
#endif

int wad_level = 0;

int sel_language = DEFAULT_LANGUAGE;

char wadmenu_filename[32];

char level_done[48];

std::uint8_t Video_Played;
std::uint8_t Eidos_Played;
std::uint8_t Wadmenu_Video;
char *Wadmenu_CivMess;
std::int16_t Wadmenu_Citations;
std::int16_t Wadmenu_Current_Con;
std::int16_t Wadmenu_Current_Ref;
std::int16_t Wadmenu_Current_Sta;
std::int16_t Wadmenu_Current_Str;
// Par Time and names
std::int32_t Wadmenu_MuckyTime;
char Wadmenu_MuckyName[12];

#ifdef VERSION_DEMO
std::int32_t demo_mode, demo_timeout;
#endif

extern void MUSIC_stop(bool fade);
extern std::uint8_t MUSIC_play(std::uint16_t wave, std::uint8_t flags);
extern ControllerPacket PAD_Input1, PAD_Input2;

PadInfo pad_cfg0 = {
    {{(WAD_PAD_RUN), PAD_LU, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_PAD_LEFT), PAD_LL, INPUT_MASK_LEFT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RIGHT), PAD_LR, INPUT_MASK_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_BACK), PAD_LD, INPUT_MASK_BACKWARDS, PAD_FLAG_NONE, 0},

     {(WAD_PAD_KICK), PAD_RU, INPUT_MASK_KICK, PAD_FLAG_NONE, 0},
     {(WAD_PAD_PUNCH), PAD_RL, INPUT_MASK_PUNCH, PAD_FLAG_NONE, 0},
     {(WAD_PAD_ACTION), PAD_RR, INPUT_MASK_ACTION, PAD_FLAG_NONE, 0},
     {(WAD_PAD_JUMP), PAD_RD, INPUT_MASK_JUMP, PAD_FLAG_NONE, 0},

     {(WAD_PAD_FPMODE), PAD_FLT, INPUT_MASK_CAMERA, PAD_FLAG_NONE, 0},
     {(WAD_PAD_NEWTARG), PAD_FRT, INPUT_MASK_STEP_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_CAM_LEFT), PAD_FLB, -KB_DEL, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_CAM_RIGHT), PAD_FRB, -KB_PGDN, PAD_FLAG_DEBOUNCE, 0},

     {(WAD_PAD_PAUSE), PAD_START, -KB_ESC, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_PAD_INV), PAD_SEL, INPUT_MASK_SELECT, PAD_FLAG_DEBOUNCE, 0}}};

PadInfo pad_cfg1 = {
    {{(WAD_PAD_RUN), PAD_LU, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_PAD_LEFT), PAD_LL, INPUT_MASK_LEFT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RIGHT), PAD_LR, INPUT_MASK_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_BACK), PAD_LD, INPUT_MASK_BACKWARDS, PAD_FLAG_NONE, 0},

     {(WAD_PAD_KICK), PAD_RU, INPUT_MASK_KICK, PAD_FLAG_NONE, 0},
     {(WAD_PAD_PUNCH), PAD_RL, INPUT_MASK_PUNCH, PAD_FLAG_NONE, 0},
     {(WAD_PAD_ACTION), PAD_RR, INPUT_MASK_ACTION, PAD_FLAG_NONE, 0},
     {(WAD_PAD_JUMP), PAD_RD, INPUT_MASK_JUMP, PAD_FLAG_NONE, 0},

     {(WAD_PAD_FPMODE), PAD_FLT, INPUT_MASK_CAMERA, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RUN), PAD_FRT, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_CAM_LEFT), PAD_FLB, -KB_DEL, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_CAM_RIGHT), PAD_FRB, -KB_PGDN, PAD_FLAG_DEBOUNCE, 0},

     {(WAD_PAD_PAUSE), PAD_START, -KB_ESC, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_PAD_INV), PAD_SEL, INPUT_MASK_SELECT, PAD_FLAG_DEBOUNCE, 0}}};
/*
        // Select
PadInfo pad_cfg1={
        {
        {(WAD_PAD_RUN),		PAD_LU,		INPUT_MASK_RUN			,PAD_FLAG_NONE,		0},
        {(WAD_PAD_LEFT),	PAD_LL,		INPUT_MASK_LEFT			,PAD_FLAG_NONE,		0},
        {(WAD_PAD_RIGHT),	PAD_LR,		INPUT_MASK_RIGHT		,PAD_FLAG_NONE,		0},
        {(WAD_PAD_BACK),	PAD_LD,		INPUT_MASK_BACKWARDS	,PAD_FLAG_NONE,		0},

        {(WAD_PAD_PUNCH),	PAD_RU,		INPUT_MASK_PUNCH		,PAD_FLAG_NONE,		0},
        {(WAD_PAD_ACTION),	PAD_RL,		INPUT_MASK_ACTION		,PAD_FLAG_NONE,		0},
        {(WAD_PAD_KICK),	PAD_RR,		INPUT_MASK_KICK			,PAD_FLAG_NONE,		0},
        {(WAD_PAD_JUMP),	PAD_RD,		INPUT_MASK_JUMP			,PAD_FLAG_NONE,		0},

        {(WAD_CAM_LEFT),	PAD_FLT,	-KB_DEL					,PAD_FLAG_DEBOUNCE, 0},
        {(WAD_CAM_RIGHT), 	PAD_FRT,	-KB_PGDN				,PAD_FLAG_DEBOUNCE, 0},
        {(WAD_PAD_FPMODE),	PAD_FLB,	INPUT_MASK_CAMERA		,PAD_FLAG_NONE,		0},
        {(WAD_PAD_NEWTARG),	PAD_FRB,	INPUT_MASK_STEP_RIGHT	,PAD_FLAG_NONE,		0},

        {(WAD_PAD_PAUSE),	PAD_START, 	-KB_ESC					,PAD_FLAG_DEBOUNCE, 0},
        {(WAD_PAD_INV),		PAD_SEL,   	INPUT_MASK_SELECT		,PAD_FLAG_DEBOUNCE, 0}
        }
};						// Select
*/
#ifdef FS_ISO9660
PadInfo pad_cfg2 = {
    {{(WAD_PAD_RUN), PAD_LU, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_PAD_LEFT), PAD_LL, INPUT_MASK_LEFT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RIGHT), PAD_LR, INPUT_MASK_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_BACK), PAD_LD, INPUT_MASK_BACKWARDS, PAD_FLAG_NONE, 0},

     {(WAD_PAD_PUNCH), PAD_RU, INPUT_MASK_PUNCH, PAD_FLAG_NONE, 0},
     {(WAD_PAD_KICK), PAD_RL, INPUT_MASK_KICK, PAD_FLAG_NONE, 0},
     {(WAD_PAD_JUMP), PAD_RR, INPUT_MASK_JUMP, PAD_FLAG_NONE, 0},
     {(WAD_PAD_ACTION), PAD_RD, INPUT_MASK_ACTION, PAD_FLAG_NONE, 0},

     {(WAD_PAD_FPMODE), PAD_FLT, INPUT_MASK_CAMERA, PAD_FLAG_NONE, 0},
     {(WAD_PAD_NEWTARG), PAD_FRT, INPUT_MASK_STEP_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_CAM_LEFT), PAD_FLB, -KB_DEL, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_CAM_RIGHT), PAD_FRB, -KB_PGDN, PAD_FLAG_DEBOUNCE, 0},

     {(WAD_PAD_PAUSE), PAD_START, -KB_ESC, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_PAD_INV), PAD_SEL, INPUT_MASK_SELECT, PAD_FLAG_DEBOUNCE, 0}}}; // Select
#else
PadInfo pad_cfg2 = {
    {{(WAD_PAD_RUN), PAD_LU, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_PAD_LEFT), PAD_LL, INPUT_MASK_LEFT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RIGHT), PAD_LR, INPUT_MASK_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_BACK), PAD_LD, INPUT_MASK_BACKWARDS, PAD_FLAG_NONE, 0},

     {(WAD_PAD_PUNCH), PAD_RU, INPUT_MASK_PUNCH, PAD_FLAG_NONE, 0},
     {(WAD_PAD_KICK), PAD_RL, INPUT_MASK_KICK, PAD_FLAG_NONE, 0},
     {(WAD_PAD_JUMP), PAD_RR, INPUT_MASK_JUMP, PAD_FLAG_NONE, 0},
     {(WAD_PAD_ACTION), PAD_RD, INPUT_MASK_ACTION, PAD_FLAG_NONE, 0},

     {(WAD_PAD_FPMODE), PAD_FLT, INPUT_MASK_CAMERA, PAD_FLAG_NONE, 0},
     {(WAD_PAD_NEWTARG), PAD_FRT, -KB_S, PAD_FLAG_NONE, 0},
     {(WAD_CAM_LEFT), PAD_FLB, -KB_DEL, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_CAM_RIGHT), PAD_FRB, -KB_PGDN, PAD_FLAG_DEBOUNCE, 0},

     {(WAD_PAD_SNAP), PAD_START, -KB_S, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_PAD_INV), PAD_SEL, INPUT_MASK_SELECT, PAD_FLAG_DEBOUNCE, 0}}}; // Select
#endif

PadInfo pad_cfg3 = {
    {{(WAD_PAD_RUN), PAD_LU, INPUT_MASK_RUN, PAD_FLAG_NONE, 0},
     {(WAD_PAD_LEFT), PAD_LL, INPUT_MASK_LEFT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_RIGHT), PAD_LR, INPUT_MASK_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_PAD_BACK), PAD_LD, INPUT_MASK_BACKWARDS, PAD_FLAG_NONE, 0},

     {(WAD_PAD_PUNCH), PAD_RU, INPUT_MASK_PUNCH, PAD_FLAG_NONE, 0},
     {(WAD_PAD_JUMP), PAD_RL, INPUT_MASK_JUMP, PAD_FLAG_NONE, 0},
     {(WAD_PAD_KICK), PAD_RR, INPUT_MASK_KICK, PAD_FLAG_NONE, 0},
     {(WAD_PAD_ACTION), PAD_RD, INPUT_MASK_ACTION, PAD_FLAG_NONE, 0},

     {(WAD_PAD_FPMODE), PAD_FLT, INPUT_MASK_CAMERA, PAD_FLAG_NONE, 0},
     {(WAD_PAD_NEWTARG), PAD_FRT, INPUT_MASK_STEP_RIGHT, PAD_FLAG_NONE, 0},
     {(WAD_CAM_LEFT), PAD_FLB, -KB_DEL, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_CAM_RIGHT), PAD_FRB, -KB_PGDN, PAD_FLAG_DEBOUNCE, 0},

     {(WAD_PAD_PAUSE), PAD_START, -KB_ESC, PAD_FLAG_DEBOUNCE, 0},
     {(WAD_PAD_INV), PAD_SEL, INPUT_MASK_SELECT, PAD_FLAG_DEBOUNCE, 0}}}; // Select

PadInfo pad_free;

PadInfo *PAD_Current;

std::int32_t PSX_msecond_timer;
extern MFX_Seek_delay;
extern MFX_Cd_Position;

extern std::uint16_t music_request_mode;
extern std::uint16_t music_current_mode;

std::uint8_t PAD_Type = 0;

void Host_VbRoutine() {
    //	char param[8];

    PSX_msecond_timer += PSX_TIMER_VBLANK;

    if (PadInfoMode(0, InfoModeCurID, 0) == 7) // Pad Analogue
    {
        std::int32_t padx, pady;

        padx = JoystickLeftX(&PAD_Input1) - 128;
        pady = JoystickLeftY(&PAD_Input1) - 128;

        if (padx < -96)
            PAD_Input1.data.pad &= ~PAD_LL;
        else if (padx > 96)
            PAD_Input1.data.pad &= ~PAD_LR;
        if (pady < -96)
            PAD_Input1.data.pad &= ~PAD_LU;
        else if (pady > 96)
            PAD_Input1.data.pad &= ~PAD_LD;
    }

    extern MFX_sound_frame;
    MFX_sound_frame++;
}

//---------------------------------------------------------------
std::int32_t InitHardware();

extern void set_next_prim_point(std::int32_t v);
extern std::uint16_t next_prim_point;
// extern void MFX_Callback_CdRead(std::uint8_t status,std::uint8_t *result);

bool SetupHost(std::uint32_t flags) {
#ifdef FS_ISO9660
    CdInit();
//	_96_init();
#endif

    ShellActive = true;

    if (!SetupMemory())
        return false;

    InitHardware();
#ifdef VERSION_PAL
#ifndef VERSION_DEMO
    // Do the encrypt library stuff here, going to have to read the key during
    // Reading Memory card.
    InitCrypt();
#endif
#endif
    //	ClearImage(&r,0,0,0);
    PSX_msecond_timer = 0;
    //	VSyncCallback(Host_VbRoutine);
    return (true);
}

//---------------------------------------------------------------

void ResetHost() {
    ResetMemory();
}

//---------------------------------------------------------------
char cd_file_buffer[128];

void PCReadFile(char *name, std::uint8_t *addr, std::uint32_t len) {
#ifndef FS_ISO9660
    std::int32_t fh;

    fh = PCopen(name, 0, 0);
    if (fh != -1) {
        PCread(fh, addr, len);
        PCclose(fh);
    }
#else
    char *p;

    p = name;
    while (*p) {
        *p = toupper(*p);
        p++;
    }
    sprintf(cd_file_buffer, "\\%s;1", name);
    MFX_Seek_delay = INFINITY;
    if (!CdReadFile(cd_file_buffer, (std::uint32_t *) addr, (len + 2047) & 0xfffff800))
        memset((void *) addr, 0, len);
    CdReadSync(0, cd_file_buffer);
    MFX_Seek_delay = 20;
#endif
}

void PSXOverLay(char *name, std::uint32_t len) {
    std::uint8_t buf[8];
    memcpy((void *) buf, (void *) my_heap, 8);
    PCReadFile(name, (std::uint8_t *) my_heap, len);
    memcpy((void *) my_heap, (void *) buf, 8);
    // Now setup bucket memory to point to directly after the loaded frontend
    // overlay (doing it here means that every time we load the front-end the
    // bucket memory ends up being in the correct place for it.
#if 1
    if (Eidos_Played)
        GDisp_SetupBucketMem(&my_heap[len], 65536);
#endif
}

bool LibShellActive() {
#ifndef FS_ISO9660
    pollhost();
#endif
    return (true);
}

//---------------------------------------------------------------

//---------------------------------------------------------------

void Time(MFTime *the_time) {
    the_time->Ticks = 0;
}

std::int32_t GetTickCount() {
    return PSX_msecond_timer;
}

bool GetInputDevice(std::uint8_t type, std::uint8_t sub_type) {
    return (1);
}

static unsigned char align[6] = {0, 1, 0xFF, 0xFF, 0xFF, 0xFF};
int psx_send = 0;
extern std::uint8_t psx_motor[2];

bool ReadInputDevice() {
    //	std::uint8_t motor[2]={0,0};
    std::int32_t id;

    extern int vibra_mode;
    //	PAD_Type = PadInfo
    id = PadInfoMode(0, InfoModeIdTable, 1);

    if (id == 7) {
        std::int32_t state = PadGetState(0);

        if (state == PadStateFindPad) {
        } else {
            if (!vibra_mode) {
                psx_motor[0] = psx_motor[1] = 0;
            }
            if (state != PadStateFindCTP1) {
                if (psx_send) {
                    PadSetAct(0x00, psx_motor, 2);
                    psx_motor[0] >>= 1;
                    id = psx_motor[1];
                    id = (id * 7) >> 3;
                    psx_motor[1] = id;
                    psx_send = 0;
                }

                if (state == PadStateStable) {
                    if (PadSetActAlign(0x00, align)) {
                        psx_send = 1;
                    }
                }
            }
        }
    }
    return (1);
}

//---------------------------------------------------------------

static unsigned long *current_tim = 0;
static int current_off;

int OpenTIM(unsigned long *addr) {
    current_tim = addr;
    current_off = 2;
    return (current_tim[0] = 0x10 ? 0 : 1);
}

TIM_IMAGE *ReadTIM(TIM_IMAGE *timing) {
    timing->mode = current_tim[1];

    if (timing->mode & 0x18) {
        timing->crect = (RECT *) &current_tim[current_off + 1];
        timing->caddr = &current_tim[current_off + 3];
        current_off += current_tim[current_off] >> 2;
    }

    timing->prect = (RECT *) &current_tim[current_off + 1];
    timing->paddr = &current_tim[current_off + 3];
    current_off += current_tim[current_off] >> 2;
}

std::int32_t Wadmenu_Levelwon;
std::uint8_t Wadmenu_Display = 1;

void Wadmenu_GetStats() {
    Wadmenu_Citations = the_game.DarciDeadCivWarnings;
    Wadmenu_Current_Con = NET_PLAYER(0)->Genus.Player->Constitution;
    Wadmenu_Current_Ref = NET_PLAYER(0)->Genus.Player->Skill;
    Wadmenu_Current_Sta = NET_PLAYER(0)->Genus.Player->Stamina;
    Wadmenu_Current_Str = NET_PLAYER(0)->Genus.Player->Strength;
}

void Wadmenu_PutStats() {
    Thing *darci = NET_PLAYER(0);
    the_game.DarciDeadCivWarnings = Wadmenu_Citations;
    darci->Genus.Player->Constitution = Wadmenu_Current_Con;
    darci->Genus.Player->Skill = Wadmenu_Current_Ref;
    darci->Genus.Player->Stamina = Wadmenu_Current_Sta;
    darci->Genus.Player->Strength = Wadmenu_Current_Str;
}

void Wadmenu_gameover(int won) {
    Wadmenu_Display = 1;

    Wadmenu_Levelwon = 1;

    if (won)
        Wadmenu_GetStats();
    else {
        wad_level = 0;
        Wadmenu_MuckyTime = 0;
    }

    level_done[wad_level] |= won;
}
