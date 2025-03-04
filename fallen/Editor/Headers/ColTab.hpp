// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef	_COLTAB_HPP_
#define	_COLTAB_HPP_

#include	"ModeTab.hpp"
#include	"Stealth.h"
#include	"EditMod.hpp"
#include	"undo.hpp"

#define	COL_TYPE_PLANE	1
#define	COL_TYPE_BEZIER	2

struct	ColInfo
{
	std::uint16_t	Type;
	std::uint16_t	Next;
	union	
	{
		struct
		{
			std::int32_t	Left;
			std::int32_t	Right;
			std::int32_t	Top;
			std::int32_t	Bottom;
			std::int32_t	Depth;
		}Plane;
		struct
		{
			std::int32_t	Top;
			std::int32_t	Bottom;
			std::int32_t	X[4];
			std::int32_t	Z[4];
		}Bezier;
	};
};

class	ColTab	:	public	ModeTab
{
	private:
		std::int32_t				Axis;
		std::int32_t				GridFlag;
		std::uint8_t				AxisMode;
		EdRect				View1;
		EdRect				View2;
		EdRect				View3;
		std::uint8_t				RedrawTabContent;
		std::uint16_t				CurrentCol;
		std::uint16_t				ClipView;
	public:
							ColTab(EditorModule *parent);
							~ColTab();
		void				DrawTabContent();
		void				HandleTab(MFPoint *current_point);
		std::uint16_t				HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void				HandleControl(std::uint16_t control_id);
		void				DrawModuleContent(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				HandleModuleContentClick(MFPoint	*clicked_point,std::uint8_t flags,std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				SetWorldMouse(std::uint32_t flag);
		std::int32_t				KeyboardInterface();
		std::int32_t				DragACol(std::uint8_t flags,MFPoint *clicked_point,std::uint16_t copy);
		std::int32_t				DragEngine(std::uint8_t flags,MFPoint *clicked_point);
//		Undo				MyUndo;
		std::uint8_t				RedrawModuleContent;
		void				Recalc();
		void				Clear();
		std::uint16_t				PlaceBezier();
		std::uint16_t				Mode;
		EditorModule		*Parent;
};


#define	MAX_COL_INFO	1000
	
extern struct	ColInfo	col_info[MAX_COL_INFO];
extern std::uint16_t	next_col_info;

#endif

