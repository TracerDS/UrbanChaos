// SaveTab.h
// Guy Simmons, 26th January 1998.

#ifndef	SAVETAB_H
#define	SAVETAB_H

//---------------------------------------------------------------

class	SaveTab	:	public	ModeTab
{
	private:
		bool			SaveState;
		std::uint32_t			LevelsMap[32];
		std::int32_t			CurrentLevel,
						HilitedLevel;

	public:

						SaveTab();
						~SaveTab();

		void			DrawTabContent();
		std::uint16_t			HandleTabClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleTab(MFPoint *current_point);
		void			HandleControl(std::uint16_t control_id);

		void			DrawLevelBox();
		std::uint16_t			LevelHilitePos(MFPoint *current_point);
		void			MapLevels();

		void			LoadLevel();
		void			SaveLevel();

		inline void		SetSaveState(bool state)		{	SaveState=state;		}
		inline bool		GetSaveState()				{	return SaveState;		}

		inline void		SetMapBit(std::uint8_t bit)			{	LevelsMap[bit>>5]	|=	(1<<(bit&0x1f));	}
		inline bool		GetMapBit(std::uint8_t bit)			{	if(LevelsMap[bit>>5]&(1<<(bit&0x1f)))return true;else return false;	}
};

//---------------------------------------------------------------


#endif
