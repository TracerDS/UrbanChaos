// ObjectEd.hpp
// Guy Simmons, 26th February 1997.

#pragma once

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
		void			HandleControlClick(std::uint8_t flags,MFPoint *clicked_point);
		void			HandleModule();
};

struct	ObjectEdDefaults
{
	std::int32_t		Left,
				Top;
	std::int32_t		Height,
				Width;
};


