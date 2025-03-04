// Controls.hpp
// Guy Simmons, 18th February 1997

#pragma once

#include	"Primativ.hpp"

#define	BUTTON				1
#define	RADIO_BUTTON		2
#define	CHECK_BOX			3
#define	STATIC_TEXT			4
#define	EDIT_TEXT			5
#define	PULLDOWN_MENU		6
#define	POPUP_MENU			7
#define	H_SLIDER			8
#define	V_SLIDER			9

#define	CONTROL_NONE			0
#define	CONTROL_INACTIVE	(1<<0)
#define	CONTROL_HILITED		(1<<1)
#define	CONTROL_SELECTED	(1<<2)
#define	CONTROL_CLICKED		(1<<3)
#define	CONTROL_SHOW_EXTRA	(1<<4)

#define	EDIT_TEXT_LENGTH	64

// ^ - Seperator line.
// ~ - Check line.
// @ - Inactive.
// ! - End of list.

#define	MENU_NORMAL			(1<<0)
#define	MENU_SEPERATOR		(1<<1)
#define	MENU_END			(1<<2)
#define	MENU_HILITED		(1<<3)
#define	MENU_INACTIVE		(1<<4)
#define	MENU_CHECK			(1<<5)
#define	MENU_CHECK_MASK		(1<<7)

#define	CTRL_SELECTED	0
#define	CTRL_DESELECTED	1
#define	CTRL_ACTIVE		2
#define	CTRL_INACTIVE	3

class MenuDef2
{
public:
	char		*ItemText;
	std::uint8_t		HotKey,
				ItemFlags,
				ItemID,
				MutualExclusiveID;
	EdRect		ItemRect;
/*
#ifdef	_MSC_VER
				MenuDef2(char* s)			{	ItemText=s;	}
				MenuDef2(char* s,std::uint8_t k)	{	ItemText=s;HotKey=k;	}
#endif
*/
};

struct ControlDef
{
	std::uint8_t		ControlType,
				HotKey;
	char		*Title;
	std::int16_t		ControlLeft,
				ControlTop,
				ControlWidth,
				ControlHeight;
	MenuDef2	*TheMenuDef;
};


class	Control	:	public EdRect
{
	private:
		std::uint8_t			Flags,
						ControlID,
						ControlType,
						HotKey;
		char			*ControlTitle;
		Control			*LastControl,
						*NextControl;
		ControlDef		*TheDef;

	public:
								Control()							{	ControlTitle=nullptr;LastControl=nullptr;NextControl=nullptr;	}
		virtual void			DrawControl();
		virtual std::uint16_t			TrackControl(MFPoint *down_point);
		virtual	void			TrackKey();
		virtual	void			HiliteControl(MFPoint *current_point);
		virtual	void			UnHiliteControl();

		virtual inline bool		PointInControl(MFPoint *the_point)		{	return PointInRect(the_point);	}

		inline void				SetFlags(std::uint8_t flags)					{	Flags=flags;					}
		inline std::uint8_t			GetFlags()							{	return Flags;					}
		inline void				SetID(std::uint8_t id)							{	ControlID=id;					}
		inline std::uint8_t			GetID()								{	return ControlID;				}
		inline void				SetType(std::uint8_t type)						{	ControlType=type;				}
		inline std::uint8_t			GetType()							{	return ControlType;				}
		inline void				SetTitle(char* title)					{	ControlTitle=title;				}
		inline char			*GetTitle()							{	return ControlTitle;			}
		inline void				SetHotKey(std::uint8_t key)					{	HotKey=key;						}
		inline std::uint8_t			GetHotKey()							{	return HotKey;					}

		inline void				SetLastControl(Control *last_control)	{	LastControl=last_control;		}
		inline void				SetNextControl(Control *next_control)	{	NextControl=next_control;		}
		inline Control			*GetLastControl()					{	return LastControl;				}
		inline Control			*GetNextControl()					{	return NextControl;				}
};


class	CButton	:	public	Control
{
	public:
						CButton(ControlDef *the_def);
		void			DrawControl();
};

class	CRadioButton	:	public Control
{
	public:
						CRadioButton(ControlDef *the_def);
		void			DrawControl();
};

class	CCheckBox	:	public Control
{
	public:
						CCheckBox(ControlDef *the_def);
		void			DrawControl();
};

class	CStaticText	:	public Control
{
	private:
		char			String1[EDIT_TEXT_LENGTH],
						String2[EDIT_TEXT_LENGTH];
	public:
						CStaticText(ControlDef *the_def);
		void			DrawControl();
		inline char	*SetString1(char* the_string)		{	strncpy(String1,the_string,EDIT_TEXT_LENGTH);String1[EDIT_TEXT_LENGTH-1]=0;return String1;	}
		inline char	*SetString2(char* the_string)		{	strncpy(String2,the_string,EDIT_TEXT_LENGTH);String2[EDIT_TEXT_LENGTH-1]=0;return String2;	}
};

class	CEditText	:	public Control
{
	private:
		char			EditText[EDIT_TEXT_LENGTH];
		std::uint32_t			SelectEnd,
						SelectStart;
		std::int32_t			CursorPos,
						TextX;

	public:
						CEditText(ControlDef *the_def);
		void			DrawControl();
		std::uint16_t			TrackControl(MFPoint *down_point);
		inline char	*GetEditString()				{	return EditText;								}
		inline char	*SetEditString(char* the_string)	{	strcpy(EditText,the_string); return EditText;	}
};

class	CPullDown	:	public Control
{
	private:
		MenuDef2		*TheMenu;
		EdRect			ItemsRect;

	public:
						CPullDown(ControlDef *the_def);
		void			DrawControl();
		std::uint16_t			TrackControl(MFPoint *down_point);
		inline void		SetItemFlags(std::uint16_t item,std::uint8_t flags){	TheMenu[item-1].ItemFlags=flags;	}
		inline std::uint8_t	GetItemFlags(std::uint16_t item)			{	return	TheMenu[item-1].ItemFlags;	}
};

class	CPopUp	:	public Control
{
	private:
		MenuDef2		*TheMenu;
		EdRect			ItemsRect;

	public:
						CPopUp(ControlDef *the_def);
		void			DrawControl();
		std::uint16_t			TrackControl(MFPoint *down_point);
		void			SetItemState(std::uint16_t item,std::uint8_t state);
		inline void		SetItemFlags(std::uint16_t item,std::uint8_t flags){	TheMenu[item-1].ItemFlags=flags;	}
		inline std::uint8_t	GetItemFlags(std::uint16_t item)			{	return	TheMenu[item-1].ItemFlags;	}
};

#define	SLIDER_SIZE		13

class	CHSlider	:	public	Control
{
	private:
		std::uint8_t			DragFlags,
						LeftButtonFlags,
						RightButtonFlags;
		std::int32_t			CurrentValue,
						MinValue,
						MaxValue,
						ValueStep;
		std::int32_t			CurrentDrag,
						MinDrag,
						MaxDrag,
						DragStep;
		EdRect			DragRect,
						LeftButtonRect,
						RightButtonRect;
		void			(*update_function)();

		void			SetupDrag();

	public:
						CHSlider(ControlDef *the_def);
		void			DrawControl();
		void			HiliteControl(MFPoint *current_point);
		void			UnHiliteControl();
		std::uint16_t			TrackControl(MFPoint *down_point);
		bool			PointInControl(MFPoint *the_point);

		void			SetCurrentValue(std::int32_t value);
		inline void		SetUpdateFunction(void (*the_fn)())		{	update_function=the_fn;					}
		inline std::int32_t	GetCurrentValue()				{	return CurrentValue;					}
		inline void		SetValueRange(std::int32_t min,std::int32_t max)	{	MinValue=min;MaxValue=max;SetupDrag();	}
		inline void		SetValueStep(std::int32_t value_step)		{	ValueStep=value_step;					}

		inline void		SetDragFlags(std::uint8_t flags)			{	DragFlags=flags;					}
		inline std::uint8_t	GetDragFlags()					{	return DragFlags;					}
		inline void		SetLeftButtonFlags(std::uint8_t flags)		{	LeftButtonFlags=flags;				}
		inline std::uint8_t	GetLeftButtonFlags()			{	return LeftButtonFlags;				}
		inline void		SetRightButtonFlags(std::uint8_t flags)	{	RightButtonFlags=flags;				}
		inline std::uint8_t	GetRightButtonFlags()			{	return RightButtonFlags;			}
};

class	CVSlider	:	public	Control
{
	private:
		std::uint8_t			DragFlags,
						TopButtonFlags,
						BottomButtonFlags;
		std::int32_t			CurrentValue,
						MinValue,
						MaxValue,
						ValueStep;
		std::int32_t			CurrentDrag,
						MinDrag,
						MaxDrag,
						DragStep;
		EdRect			DragRect,
						TopButtonRect,
						BottomButtonRect;
		void			(*update_function)();

		void			SetupDrag();

	public:
						CVSlider(ControlDef *the_def);
		void			DrawControl();
		void			HiliteControl(MFPoint *current_point);
		void			UnHiliteControl();
		std::uint16_t			TrackControl(MFPoint *down_point);
		bool			PointInControl(MFPoint *the_point);

		void			SetCurrentValue(std::int32_t value);
		inline void		SetUpdateFunction(void (*the_fn)())		{	update_function=the_fn;					}
		inline std::int32_t	GetCurrentValue()				{	return CurrentValue;					}
		inline void		SetValueRange(std::int32_t min,std::int32_t max)	{	MinValue=min;MaxValue=max;SetupDrag();	}
		inline void		SetValueStep(std::int32_t value_step)		{	ValueStep=value_step;					}

		inline void		SetDragFlags(std::uint8_t flags)			{	DragFlags=flags;					}
		inline std::uint8_t	GetDragFlags()					{	return DragFlags;					}
		inline void		SetTopButtonFlags(std::uint8_t flags)		{	TopButtonFlags=flags;				}
		inline std::uint8_t	GetTopButtonFlags()				{	return TopButtonFlags;				}
		inline void		SetBottomButtonFlags(std::uint8_t flags)	{	BottomButtonFlags=flags;			}
		inline std::uint8_t	GetBottomButtonFlags()			{	return BottomButtonFlags;			}
};

