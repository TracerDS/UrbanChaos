//
// xlat_str.h
// matthew rosenfeld 1 july 99
//
// translates strings to furrigan languages and remap buttons for the user's settings
//

#pragma once

#include "MFStdLib.h"

//----------------------------------------------------------------------------
// MISC CONFIG DEFINES
//

#ifndef PSX
#define MAX_STRING_SPACE	(8192)
#define MAX_STRINGS			(256)
#else
#define MAX_STRING_SPACE	(4096)
#define MAX_STRINGS			(256)
#endif

//----------------------------------------------------------------------------
// TOKEN DEFINES
//

#define	XCTL_JUMP					(1)
#define XCTL_PUNCH					(2)
#define XCTL_KICK					(3)
#define XCTL_ACTION					(4)
#define XCTL_LEFT					(5)
#define XCTL_RIGHT					(6)
#define XCTL_START					(7)
#define XCTL_SELECT					(8)
#define XCTL_SPACE					(9)
#define XCTL_ENTER					(10)
#define XCTL_CAM_FIRST				(11)
#define XCTL_RUN					(12)
#define XCTL_CAM_HIGH				(13)
#define XCTL_CAM_LOW				(14)
#define XCTL_CAM_ESC				(15)
#define XCTL_CAM_CONTINUE			(16)

//----------------------------------------------------------------------------
// TRANSLATION DEFINES
//

#define X_THIS_LANGUAGE_IS			(0)
#define X_PRESS_TO_CONTINUE			(1)
#define X_GAME_OVER					(2)
#define X_CAR_LOCKED				(3)
#define X_AMMO						(4)
#define X_GUN						(5)
#define X_HEALTH					(6)
#define X_FUSE_SET					(7)
#define X_ENTER_VEHICLE				(8)
#define X_USE_BIKE					(9)
#define X_PICK_UP					(10)
#define X_GRAB_CABLE				(11)
#define X_SEARCHING					(12)
#define X_COMPLETE					(13)
#define X_QUIT_TO_MAIN_MENU			(14)
#define X_RESTART_AUTOSAVE			(15)
#define X_LEVEL_COMPLETE			(16)
#define X_START						(17)
#define X_LOAD_GAME					(18)
#define X_SAVE_GAME					(19)
#define X_OPTIONS					(20)
#define X_GRAPHICS					(21)
#define X_SOUND						(22)
#define X_MUSIC						(23)
#define X_QUALITY					(24)
#define X_VOLUME					(25)
#define X_STEREO					(26)
#define X_MONO						(27)
#define X_REVERSE_STEREO			(28)
#define X_DRIVERS					(29)
#define X_RESOLUTION				(30)
#define X_REFLECTIONS				(31)
#define X_DRAW_DISTANCE				(32)
#define X_SHADOWS					(33)
#define X_DETAIL					(34)
#define X_HIGH						(35)
#define X_MEDIUM					(36)
#define X_LOW						(37)
#define X_EXIT						(38)
#define X_OKAY						(39)
#define X_CANCEL					(40)
#define X_CONTROLS					(41)
#define X_KEYPAD					(42)
#define X_KEYBOARD					(43)
#define X_JOYSTICK					(44)
#define X_JOYPAD					(45)
#define X_SPACE						(46)
#define X_ENTER						(47)
#define X_JUMP						(48)
#define X_KICK						(49)
#define X_PUNCH						(50)
#define X_ACTION					(51)
#define X_LEFT						(52)
#define X_RIGHT						(53)
#define X_FORWARDS					(54)
#define X_BACKWARDS					(55)
#define X_SELECT					(56)
#define X_LOOK_AROUND				(57)
#define X_RUN						(58)
#define X_CAMERA					(59)
#define X_BADGE						(60)
#define X_COLLECTED					(61)
#define X_COMPLETED					(62)
#define X_FAILED					(63)
#define X_SCORE						(64)
#define X_PRIMARY					(65)
#define X_SECONDARY					(66)
#define X_OBJECTIVES				(67)
#define X_MAP						(68)
#define X_WEAPONS					(69)
#define X_SHOTGUN					(70)
#define X_AK						(71)
#define X_GRENADE					(72)
#define X_EXPLOSIVES				(73)
#define X_KNIFE						(74)
#define X_BASEBALL_BAT				(75)
#define X_BOMB						(76)
#define X_KEYCARD					(77)
#define X_FILE						(78)
#define X_FLOPPY_DISK				(79)
#define X_CROWBAR					(80)
#define X_GAS_MASK					(81)
#define X_WRENCH					(82)
#define X_VIDEO						(83)
#define X_GLOVES					(84)
#define X_WEED_AWAY					(85)
#define X_RED_CAR_KEYS				(86)
#define X_BLUE_CAR_KEYS				(87)
#define X_GREEN_CAR_KEYS			(88)
#define X_BLACK_CAR_KEYS			(89)
#define X_WHITE_CAR_KEYS			(90)
#define X_WIRECUTTERS				(91)
#define X_ARE_YOU_SURE				(92)
#define X_CRIME_RATE				(93)
#define X_ARRESTED					(94)
#define X_COMPANY					(95)
#define X_COPYING_GLOBAL_MEDIA		(96)
#define X_COPYING_LOCAL_MEDIA		(97)
#define X_COPYING_PROGRAM			(98)
#define X_REQUIRES_WINDOWS			(99)
#define X_COMPONENT					(100)
#define X_INST_FILE					(101)
#define X_FILE_GROUP				(102)
#define X_ERROR_OCCURRED			(103)
#define X_REQUIRES_DIRECTX			(104)
#define X_FAILED_TO_INITIALIZE		(105)
#define X_REQUIRES_VGA				(106)
#define X_URBAN_CHAOS_SETUP			(107)
#define X_EDITOR					(108)
#define X_AMBIENT					(109)
#define X_STARS						(110)
#define X_MOON						(111)
#define X_PEOPLE					(112)
#define X_PUDDLES					(113)
#define X_DIRT						(114)
#define X_MIST						(115)
#define X_RAIN						(116)
#define X_SKYLINE					(117)
#define X_CON_INCREASED				(118)
#define X_STR_INCREASED				(119)
#define X_STA_INCREASED				(120)
#define X_REF_INCREASED				(121)
#define X_LOAD_UCM					(122)
#define X_SCANNER					(123)
#define X_TRACK  					(124)
#define X_CHARACTER					(125)
#define X_GAMMA						(126)
#define X_RESET_DEFAULT				(127)
#define	X_STR						(128)
#define X_CON						(129)
#define X_DEX						(130)
#define X_STA						(131)
#define X_NO_CONES_HIT				(132)
#define X_NUM_CONES_HIT				(133)
#define X_RESUME_LEVEL				(134)
#define X_RESTART_LEVEL				(135)
#define X_ABANDON_GAME				(136)
#define X_TEXTURE_MAP   			(137)
#define X_PERSPECTIVE 				(138)
#define X_BILINEAR					(139)
#define X_TIME_PENALTY				(140)
#define X_LEVEL_LOST				(141)
#define X_GAME_PAUSED				(142)
#define X_GET_DOWN					(143)
#define X_CANT_SHOOT_COP			(144)
#define X_HEY_WHY_HITTING			(145)
#define X_CANT_SHOOT_UNARMED		(146)
#define X_CRIME_RATE_REDUCED		(147)
#define X_GAME_SAVED				(148)
#define X_CRINKLES					(149)
#define X_COLOUR_DEPTH				(150)
#define X_CIVSDEAD_WARNING_1		(151)
#define X_CIVSDEAD_WARNING_2		(152)
#define X_CIVSDEAD_WARNING_3		(153)
#define X_CIVSDEAD_WARNING_4		(154)
#define X_WON_KILLED				(155)
#define X_WON_ARRESTED				(156)
#define X_WON_AT_LARGE				(157)
#define X_WON_BONUS_FOUND			(158)
#define X_WON_BONUS_MISSED			(159)
#define X_WON_TIMETAKEN				(160)
#define X_WON_BESTTIME				(161)
#define X_WON_MUCKYTIME				(162)
#define X_YES						(163)
#define X_NO						(164)
#define X_EMPTY						(165)
#define X_3DCARD					(166)
#define X_USE_PRIMARY				(167)
#define X_USE_SECONDARY				(168)
#define X_USE_SOFTWARE				(169)
#define X_NUM_COLOURS				(170)
#define X_16BIT						(171)
#define X_24BIT						(172)
#define X_DO_NOT_SHOW				(173)
#define X_MILES_SETUP				(174)
#define X_SELECT_SOUND				(175)
#define X_BUTTON					(176)
#define X_CREDITS					(177)
#define X_BONUS_MISSION_UNLOCKED	(178)
#define X_JUMP_CAM					(179)

#ifdef TARGET_DC
#define X_PAD_MODE					(180)
#define X_PAD_CUSTOM				(181)
#define X_PAD_WALK					(182)
#define X_CHANGE_JOYPAD				(183)
#define X_TURN						(184)
#define X_MOVE						(185)
#define X_PANEL						(186)
#define X_LANGUAGE					(187)
#define X_ENGLISH					(188)
#define X_FRENCH					(189)
#define X_VMU_SELECT				(190)
#define X_VMU_NOT_PRESENT			(191)
#define X_VMU_FULL					(192)
#define X_VMU_CONTROLLER_SLOT		(193)
#define X_GAME_SAVE_FAILED			(194)
#define X_GAME_LOADED				(195)
#define X_GAME_LOAD_FAILED			(196)
#define X_GENERAL					(197)
#define X_OVERWRITE_SURE			(198)
#define X_NO_CONTROLLER_CONNECTED	(199)
#define X_CONTROLLER_REMOVED		(200)
#define X_VIBRATION					(201)
#define X_VIBRATION_ENG				(202)
#define X_GAME_LOADING				(203)
#define X_GAME_SAVING				(204)
#define X_VMU_X_BLOCKS_REQUIRED		(205)
#define X_GAME_NAME					(206)
#endif

//----------------------------------------------------------------------------
// FUNCS
//

char* XLAT_str(std::int32_t string_id, char* xlat_dest=0);
char* XLAT_str_ptr(std::int32_t string_id);
void XLAT_load(char* fn);
void XLAT_init();

