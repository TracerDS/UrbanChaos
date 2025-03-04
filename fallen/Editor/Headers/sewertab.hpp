// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef	_SewerTab_HPP_
#define	_SewerTab_HPP_

#include	"ModeTab.hpp"
#include	"Stealth.h"
#include	"EditMod.hpp"
#include	"undo.hpp"


#define	FLAG_WINDOW_USED		(1<<0)
#define	FLAG_WINDOW_LEDGE		(1<<1)
#define	FLAG_WINDOW_FAT_LEDGE	(1<<2)
#define	FLAG_WINDOW_WIDE_LEDGE	(1<<3)
#define	FLAG_WINDOW_SUNK_IN		(1<<4)
#define	FLAG_WINDOW_FLAT		(1<<5)
#define	FLAG_WINDOW_EDGED		(1<<6)
#define	FLAG_WINDOW_EDGED_DEEP	(1<<7)
#define	FLAG_WINDOW_EDGED_FAT	(1<<8)


/*
#define	FLAG_WINDOW_	(1<<)
*/

/*
#define	FLAG_WALL_			(1<<)
*/

/*
#define	FLAG_STOREY_		(1<<)
*/

#define	FLAG_BUILDING_
/*
#define	FLAG_BUILDING_
*/




#define	PASTE_TEXTURE	(1<<0)
#define	PASTE_ALTITUDE	(1<<1)


extern std::int16_t	get_new_storey();
extern void	free_wall(std::int16_t wall);
extern std::int16_t	get_new_building();
extern std::int16_t	get_new_wall();







class	SewerTab	:	public	ModeTab
{
	private:
		std::int32_t				Axis;
		std::int32_t				GridFlag;
		std::uint8_t				AxisMode;
		EdRect				View1;
		EdRect				View2;
		EdRect				View3;
		std::uint8_t				RedrawTabContent;
		std::int32_t				X1,Y1,Z1,X2,Y2,Z2;
		std::int32_t				FloorHead;
		std::int32_t				EditWindow;
		std::int32_t				EditY;
		std::int32_t				CurrentY;
		std::int32_t				ViewX,ViewY,ViewZ;
		std::int32_t				ViewSize;
		std::int32_t				RoomID;

	public:
							SewerTab(EditorModule *parent);
							~SewerTab();
		void				DrawTabContent();
		void				HandleTab(MFPoint *current_point);
		std::uint16_t				HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void				HandleControl(std::uint16_t control_id);
		void				DrawModuleContent(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				HandleModuleContentClick(MFPoint	*clicked_point,std::uint8_t flags,std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				SetWorldMouse(std::uint32_t flag);
		std::int32_t				KeyboardInterface();
		std::int32_t				DragEngine(std::uint8_t flags,MFPoint *clicked_point);
		std::int32_t				CalcMapCoord(std::int32_t	*mapx,std::int32_t	*mapy,std::int32_t	*mapz,std::int32_t	x,std::int32_t	y,std::int32_t	w,std::int32_t	h,MFPoint	*clicked_point);
		std::int32_t				DragPaint(std::uint8_t flags);
		std::int32_t				DragMark(std::uint8_t flags);
		std::int32_t				DragVertex(std::uint8_t flags);
		std::int32_t				DragBuilding(std::uint8_t flags,std::uint8_t type);
		std::int32_t				DragStairs(std::uint16_t stairs,std::uint8_t flags);

		std::int32_t				MouseInContent();
		std::int32_t				DoKeys();
		std::int32_t  				DoZoom();
		void				HighlightVertexes(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		std::int32_t				ClickInVertexStoreyList(std::int32_t building,std::int32_t storey_index,std::int32_t w,std::int32_t h,MFPoint	*mouse_point);
		std::int32_t				ClickInVertex(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h,MFPoint	*mouse_point);
		std::int32_t				ClickNearWall(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h,MFPoint	*mouse_point);
		std::int32_t				WallOptions();
		std::int32_t				RoofOptions();
		std::int32_t				FenceOptions();
		void				DeleteVertex();
		void				AddHeightOffset(std::int32_t *x,std::int32_t *y);
		std::int32_t				GetHeightColour(std::int32_t storey);
		void				DrawContentLine(std::int32_t x1,std::int32_t y1,std::int32_t x2,std::int32_t y2,std::int32_t col);
		void				DrawContentLineY(std::int32_t x1,std::int32_t y1,std::int32_t z1,std::int32_t x2,std::int32_t y2,std::int32_t z2,std::int32_t col);

		std::int32_t				DrawWall(std::int32_t px,std::int32_t pz,std::int32_t x1,std::int32_t z1,std::int32_t index,std::int32_t storey);
		std::int32_t				DrawWindow(std::int32_t px,std::int32_t pz,std::int32_t x1,std::int32_t z1,std::int32_t dx,std::int32_t dz);
		void				DrawRoofFaces(std::int32_t roof,std::int32_t storey);
		void				DrawFloorFaces(std::int32_t wall);
		void				CheckStoreyIntegrity(std::uint16_t storey);
		void				ResetSewerTab();
		void				DrawFloorTextures(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		void				DrawFloorLabels(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h);
		inline	void		SetViewToEngine()	{ViewX=(engine.X>>8)&ELE_AND;ViewY=(engine.Y>>8)&ELE_AND;ViewZ=(engine.Z>>8)&ELE_AND;}
		inline	void		SetEngineToView()	{engine.X=ViewX<<8;engine.Y=ViewY<<8;engine.Z=ViewZ<<8;}
		inline	void		SetView(std::int32_t x,std::int32_t z)	{ViewX=x;ViewZ=z;}
		void				DrawContentRect(std::int32_t x1,std::int32_t z1,std::int32_t x2,std::int32_t z2,std::int32_t col);
		void				DoStairPopUp(std::int32_t stair,MFPoint *clicked_point);
//		Undo				MyUndo;
		std::uint8_t				RedrawModuleContent;
		void				Clear();
		std::uint16_t				Mode;
		std::int32_t				EditBuilding;
		std::int32_t				EditStorey;
		std::int32_t				EditWall;
		std::int32_t				OutsideEditStorey;
		std::int32_t				OutsideEditWall;
		std::uint16_t				Texture;
		std::uint16_t				RoofTop;
		std::uint16_t				CurrentFloorType;
		EditorModule		*Parent;
};



#endif

