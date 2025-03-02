// SaveTab.h
// Guy Simmons, 26th January 1998.

#ifndef	SAVETAB_H
#define	SAVETAB_H

//---------------------------------------------------------------

class	SaveTab	:	public	ModeTab
{
	private:
		BOOL			SaveState;
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

		inline void		SetSaveState(BOOL state)		{	SaveState=state;		}
		inline BOOL		GetSaveState()				{	return SaveState;		}

		inline void		SetMapBit(UBYTE bit)			{	LevelsMap[bit>>5]	|=	(1<<(bit&0x1f));	}
		inline BOOL		GetMapBit(UBYTE bit)			{	if(LevelsMap[bit>>5]&(1<<(bit&0x1f)))return TRUE;else return FALSE;	}
};

//---------------------------------------------------------------


#endif
