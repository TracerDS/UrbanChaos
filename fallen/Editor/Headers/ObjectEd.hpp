// ObjectEd.hpp
// Guy Simmons, 26th February 1997.

#ifndef	_OBJECTED_HPP_
#define	_OBJECTED_HPP_

#include	"EditMod.hpp"
#include	"TexTab.hpp"

#define	TAB_NONE			0
#define	TAB_TEXTURE			1


class	ObjectEditor	:	public	EditorModule
{
	private:
		TextureTab		*TextureMode;

	public:
		void			SetupModule();
		void			CreateObjectTabs();
		void			DestroyObjectTabs();
		void			HandleControlClick(UBYTE flags,MFPoint *clicked_point);
		void			HandleModule();
};

struct	ObjectEdDefaults
{
	SLONG		Left,
				Top;
	SLONG		Height,
				Width;
};

#endif

