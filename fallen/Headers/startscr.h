#pragma once


#define	STARTS_START	1
#define	STARTS_EDITOR	2
#define	STARTS_LOAD		3	
#define	STARTS_EXIT		4	
#define	STARTS_HOST		5	
#define	STARTS_JOIN		6
#define	STARTS_PLAYBACK	7
#define	STARTS_PSX		8
#define	STARTS_MULTI	9
#define STARTS_LANGUAGE_CHANGE 10

struct StartMenu
{
	std::uint8_t	StartIndex;
	std::uint8_t	Count;
	std::uint8_t	Current;
	std::uint16_t	Type;
};

struct StartMenuItemSimple
{
	char	*Str;
	std::int32_t	NextMenu;
	std::int32_t	Action;
	std::int32_t	Dummy1;
	std::int32_t	Dummy2;
};

struct StartMenuItemComplex
{
	char	*Str;
	char	*Strb[3];
	std::int32_t	NextMenu;
	std::int32_t	Action;
	std::int32_t	Item;
	std::int32_t	Dummy2;
};

void STARTSCR_notify_gameover(bool won);

typedef void (*MISSION_callback)(char* filename);
void MissionListCallback(char* script, MISSION_callback cb);



