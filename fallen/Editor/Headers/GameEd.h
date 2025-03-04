// GameEd.h
// Guy Simmons, 12th January 1998.

#pragma once

#include	"engine.h"
#include	"ComTab.h"
#include	"CondTab.h"
#include	"SaveTab.h"
#include	"ThingTab.h"

//---------------------------------------------------------------

#define	CLASS_NONE			0
#define	CLASS_PLAYER		1
#define	CLASS_CAMERA		2
#define	CLASS_PROJECTILE	3
#define CLASS_BUILDING		4
#define CLASS_PERSON		5

#define	TAB_NONE			0
#define	TAB_THINGS			1
#define	TAB_LEVELS			2
#define	TAB_COMMANDS		3
#define	TAB_CONDITIONS		4

#define	SELECT_NONE				0
#define	SELECT_WAYPOINT			1
#define	SELECT_NEXT_WAYPOINT	2
#define	SELECT_PREV_WAYPOINT	3
#define	SELECT_COND_TAB_THING	4
#define	SELECT_COND_TAB_SWITCH	5
#define	SELECT_THING_TAB_THING	6
#define	SELECT_THING_TAB_SWITCH	7
#define	SELECT_COM_TAB_WAYPOINT	8
#define	SELECT_COM_TAB_THING	9
#define	SELECT_COM_TAB_SWITCH	10

//---------------------------------------------------------------

class	GameEditor		:	public	EditorModule
{
	private:

		bool			FlashState;
		std::uint8_t			SelectMode;
		std::int32_t			CurrentThing,
						FlashCount;
		EdItem			BackupItem,
						HilitedItem,
						LastItem,
						SelectedItem;
		EngineStuff		EdEngine;
		MFPoint			DownPoint;

		CommandTab		*CommandMode;
		ConditionTab	*ConditionMode;
		SaveTab			*SaveMode;
		ThingTab		*ThingMode;


	public:

						~GameEditor();

		void			SetupModule();
		void			CreateTabs();
		void			DestroyTabs();

		void			DrawContent();
		void			HandleContentClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleControlClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleModule();
		void			HandleThingDrag();
		std::uint16_t			EngineKeys();
		void			DoThingPopup(MFPoint *clicked_point);
		void			DoBlockPopup(MFPoint *clicked_point);
		void			HandleWaypointDrag();
		void			DoWaypointPopup(MFPoint *clicked_point);
		void			HandleSizeDrag();

		void			GameEdEngine();
		std::int32_t			DrawFacet(std::uint16_t facet_index,std::int32_t x,std::int32_t y,std::int32_t z);
		void			ScanEngine();
		void			RenderEngine();
		void			MapText(std::int32_t x,std::int32_t y,char* the_str,std::uint32_t col);
		void			MapThingInfo(std::int32_t x,std::int32_t y,BucketMapThing *the_map_thing);
		void			MapWaypointInfo(std::int32_t x,std::int32_t y,BucketWaypoint *the_waypoint);
		void			MapSphereInfo(std::int32_t x,std::int32_t y,BucketSphereArea *the_sphere);

		void			ClearTabMode();

		inline void		SetSelectMode(std::uint8_t mode)	{	SetLocalEscape();SelectMode=mode;BackupItem=SelectedItem;SelectedItem=HilitedItem;	}
		inline void		ClearSelectMode()		{	ClearTabMode();ClearLocalEscape();SelectMode=0;SelectedItem=BackupItem;	}
};

//---------------------------------------------------------------

struct	GameEdDefaults
{
	std::int32_t		Left,
				Top;
	std::int32_t		Height,
				Width;
};

//---------------------------------------------------------------

