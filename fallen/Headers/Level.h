// Level.h
// Guy Simmons, 26th January 1998.

#ifndef LEVEL_H
#define LEVEL_H

//---------------------------------------------------------------

struct ThingDef {
    std::uint8_t Version;

    std::int32_t Class,
        Genus,
        X, Y, Z;
    std::uint16_t CommandRef;
    std::int32_t Data[10];
    std::uint16_t EdThingRef;
};

//---------------------------------------------------------------

struct WaypointDef {
    std::uint8_t Version;

    std::uint16_t Next,
        Prev;
    std::int32_t X, Y, Z;
    std::uint16_t EdWaypointRef;
};

//---------------------------------------------------------------

struct ConditionListDef {
    std::uint8_t Version;

    char ListName[32];
    std::uint32_t ConditionCount;
    std::uint16_t EdConListRef;
};

//---------------------------------------------------------------

struct ConditionDef {
    std::uint8_t Version;

    std::uint16_t Flags,
        ConditionType,
        GroupRef;
    std::int32_t Data1,
        Data2,
        Data3;
};

//---------------------------------------------------------------

struct CommandListDef {
    std::uint8_t Version;

    char ListName[32];
    std::uint32_t CommandCount;
    std::uint16_t EdComListRef;
};

//---------------------------------------------------------------

struct CommandDef {
    std::uint8_t Version;

    std::uint16_t Flags,
        CommandType,
        GroupRef;
    std::int32_t Data1,
        Data2,
        Data3;
};

//---------------------------------------------------------------
// Level format.

// VERSION 0

//	std::uint8_t		Version
//	std::uint32_t		ThingCount
//	ThingDef	ThingDefs * ThingCount

// VERSION 1

//	std::uint8_t		Version
//	std::uint32_t		ThingCount
//	ThingDef	ThingDefs * ThingCount
//	std::uint32_t		WaypointCount
//	WaypointDef	WaypointDefs * WaypointCount

// VERSION 2

//	std::uint8_t		Version
//	std::uint32_t		ThingCount
//	ThingDef	ThingDefs * ThingCount
//	std::uint32_t		WaypointCount
//	WaypointDef	WaypointDefs * WaypointCount
//	std::uint32_t		ConditionListCount
//	(
//	ConditionListDef
//	ConditionDef * ConditionListDef.ConditionCount
//	)	*	ConditionListCount

// VERSION 3

//	std::uint8_t		Version
//	std::uint32_t		ThingCount
//	ThingDef	ThingDefs * ThingCount
//	std::uint32_t		WaypointCount
//	WaypointDef	WaypointDefs * WaypointCount
//	std::uint32_t		ConditionListCount
//	(
//	ConditionListDef
//	ConditionDef * ConditionListDef.ConditionCount
//	)	*	ConditionListCount
//	std::uint32_t		CommandListCount
//	(
//	CommandListDef
//	ConmmandDef * CommandListDef.CommandCount
//	)	*	CommandListCount

//---------------------------------------------------------------

bool load_level(std::uint32_t level);

//---------------------------------------------------------------

#endif
