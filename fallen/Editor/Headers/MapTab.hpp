// TexTab.hpp
// MCD, 20th February 1997

#ifndef	_MAPTAB_HPP_
#define	_MAPTAB_HPP_

#include	"ModeTab.hpp"
#include	"Stealth.h"
#include	"EditMod.hpp"
#include	"undo.hpp"

#define	MAP_TYPE_PLANE	1
#define	MAP_TYPE_BEZIER	2

struct	MapInfo
{
	std::int32_t	Left;
	std::int32_t	Right;
	std::int32_t	Top;
	std::int32_t	Bottom;

	std::int32_t	X;
	std::int32_t	Y;
	std::int32_t	Z;
	std::int16_t	AngleY;
	std::int16_t	Background;
	void	*PtrMap;
	std::int16_t	MapWidth;
	std::int16_t	MapHeight;
	char	Name[20];
	std::int32_t	Dummy[4];


};

class	MapTab	:	public	ModeTab
{
	private:
		std::int32_t				Axis;
		std::int32_t				GridFlag;
		std::uint8_t				AxisMode;
		EdRect				View1;
		EdRect				View2;
		EdRect				View3;
		std::uint8_t				RedrawTabContent;
		std::uint16_t				CurrentMap;
		std::uint16_t				DefMode;
	public:
							MapTab(EditorModule *parent);
							~MapTab();
		void				DrawTabContent();
		void				HandleTab(MFPoint *current_point);
		std::uint16_t				HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void				HandleControl(std::uint16_t control_id);
		void				DrawModuleContent(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				HandleModuleContentClick(MFPoint	*clicked_point,std::uint8_t flags,std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				SetWorldMouse(std::uint32_t flag);
		std::int32_t				KeyboardInterface();
		std::int32_t				DragAMap(std::uint8_t flags,MFPoint *clicked_point,std::uint16_t copy);
		std::int32_t				DragAMapDef(std::uint8_t flags,MFPoint *clicked_point,std::uint16_t copy);
		std::int32_t				DragAMapDefXYZ(std::uint8_t flags,MFPoint *clicked_point,std::uint16_t copy);
		std::int32_t				DragEngine(std::uint8_t flags,MFPoint *clicked_point);
		void				SetMapPos(std::int32_t x,std::int32_t y,std::int32_t z);
//		Undo				MyUndo;
		std::uint8_t				RedrawModuleContent;
		void				Recalc();
		void				Clear();
		std::uint16_t				PlaceBezier();
		std::uint16_t				Mode;
		EditorModule		*Parent;
};


#define	MAX_MAP_INFO	1000
	
extern struct	MapInfo	map_info[MAX_MAP_INFO];
extern std::uint16_t	next_map_info;

#endif

