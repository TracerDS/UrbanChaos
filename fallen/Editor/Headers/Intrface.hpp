// Intrface.hpp
// Guy Simmons, 18th February 1997.

#pragma once

class	Interface
{
	private:
		std::uint32_t			ContentColour,
						ContentColourBr,
						HiliteColour,
						LoliteColour,
						ActiveColour,
						InactiveColour,
						WhiteColour,
						GreyColour,
						YellowColour,
						RedColour,
						GreenColour,
						BlueColour,
						TextColour,
						SelectColour;
		std::uint8_t				InterfacePalette[768];
		std::uint8_t			*interface_sprite_data;
		BSprite			*interface_sprites;

	public:
						Interface();
						~Interface();
		void			SetupInterfaceDefaults();
		inline std::uint32_t	GetContentColour()			{	return	ContentColour;			};
		inline std::uint32_t	GetContentColourBr()		{	return	ContentColourBr;		};
		inline std::uint32_t	GetHiliteColour()			{	return	HiliteColour;			};
		inline std::uint32_t	GetWhiteColour()			{	return	WhiteColour;			};
		inline std::uint32_t	GetGreyColour()				{	return	GreyColour;				};
		inline std::uint32_t	GetYellowColour()			{	return	YellowColour;			};
		inline std::uint32_t	GetRedColour()				{	return	RedColour;				};
		inline std::uint32_t	GetGreenColour()			{	return	GreenColour;			};
		inline std::uint32_t	GetBlueColour()				{	return	BlueColour;				};
		inline std::uint32_t	GetLoliteColour()			{	return	LoliteColour;			};
		inline std::uint32_t	GetActiveColour()			{	return	ActiveColour;			};
		inline std::uint32_t	GetInactiveColour()			{	return	InactiveColour;			};
		inline std::uint32_t	GetTextColour()				{	return	TextColour;				};
		inline std::uint32_t	GetSelectColour()			{	return	SelectColour;			};
		inline std::uint8_t	*GetPalette()				{	return	InterfacePalette;		};
		inline BSprite*	GetInterfaceSprite(std::uint16_t id)	{	return	&interface_sprites[id];	};
//		inline BSprite*	GetInterfacePointer(std::uint16_t id)	{	return	&interface_pointers[id];};
};




extern Interface				*InterfaceDefaults;
#define	CONTENT_COL_BR			InterfaceDefaults->GetContentColourBr()
#define	CONTENT_COL				InterfaceDefaults->GetContentColour()
#define	HILITE_COL  			InterfaceDefaults->GetHiliteColour()
#define	LOLITE_COL		  		InterfaceDefaults->GetLoliteColour()
#define	ACTIVE_COL				InterfaceDefaults->GetActiveColour()
#define	INACTIVE_COL			InterfaceDefaults->GetInactiveColour()
#define	TEXT_COL				InterfaceDefaults->GetTextColour()
#define	SELECT_COL	  			InterfaceDefaults->GetSelectColour()
#define	WHITE_COL  				InterfaceDefaults->GetWhiteColour()
#define	GREY_COL  				InterfaceDefaults->GetGreyColour()
#define	YELLOW_COL  				InterfaceDefaults->GetYellowColour()
#define	RED_COL  				InterfaceDefaults->GetRedColour()
#define	GREEN_COL  				InterfaceDefaults->GetGreenColour()
#define	BLUE_COL  				InterfaceDefaults->GetBlueColour()

#define	INTERFACE_SPRITE(ID)	InterfaceDefaults->GetInterfaceSprite(ID)
#define	INTERFACE_POINTER(ID)	InterfaceDefaults->GetInterfacePointer(ID)

#define	PALETTE					InterfaceDefaults->GetPalette()

#define	DEFAULTS_DIRECTORY		"EditDefs"

#define	GROW_ICON				1

#define	UP_ICON					6
#define	DOWN_ICON				7
#define	LEFT_ICON				8
#define	RIGHT_ICON				9
#define	CHECK_ICON				10
#define	RADIO_ICON1				11
#define	RADIO_ICON2				12
#define	RADIO_ICON3				13
#define	RADIO_ICON4				14
#define MENU_ICON				15

#define	NO_CLICK				0
#define	LEFT_CLICK				1
#define	RIGHT_CLICK				2
#define	MIDDLE_CLICK			3


