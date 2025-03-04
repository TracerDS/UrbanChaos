// Alert.hpp
// Guy Simmons, 11th April 1997.

#pragma once

#include	"CtrlSet.hpp"
#include	"Primativ.hpp"


class	Alert	:	public	EdRect
{
	private:
		ControlSet	AlertSet;

	public:
				Alert();
				Alert(char* text1,char* text2);
				~Alert();
		bool	HandleAlert(char* text1,char* text2);
};

