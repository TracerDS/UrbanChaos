// SaveTab.h
// Guy Simmons, 26th January 1998.

#ifndef	SAVETAB_H
#define	SAVETAB_H

//---------------------------------------------------------------

class	SaveTab	:	public	ModeTab
{
	private:
		bool			SaveState;
		ULONG			LevelsMap[32];
		SLONG			CurrentLevel,
						HilitedLevel;

	public:

						SaveTab();
						~SaveTab();

		void			DrawTabContent();
		UWORD			HandleTabClick(UBYTE flags,MFPoint *clicked_point);
		void			HandleTab(MFPoint *current_point);
		void			HandleControl(UWORD control_id);

		void			DrawLevelBox();
		UWORD			LevelHilitePos(MFPoint *current_point);
		void			MapLevels();

		void			LoadLevel();
		void			SaveLevel();

		inline void		SetSaveState(bool state)		{	SaveState=state;		}
		inline bool		GetSaveState()				{	return SaveState;		}

		inline void		SetMapBit(UBYTE bit)			{	LevelsMap[bit>>5]	|=	(1<<(bit&0x1f));	}
		inline bool		GetMapBit(UBYTE bit)			{	if(LevelsMap[bit>>5]&(1<<(bit&0x1f)))return true;else return false;	}
};

//---------------------------------------------------------------


#endif
