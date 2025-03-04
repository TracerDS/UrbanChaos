// TexTab.hpp
// Guy Simmons, 20th February 1997

#pragma once

#include	"ModeTab.hpp"


class	FileRequester : public	EdRect 
{
	private:	char		*WildCard;
				char		*Title;
				EdRect		OK;
				EdRect		Cancel;
				EdRect		TextEdit;
				EdRect		TextList[30];
	public:
							FileRequester(char	*Path,char	*Extension,char* Title,char* FileName);
				char		*Path;
				char		FileName[200];
				std::int32_t		Draw();
				ControlSet	Controls;

};


