//
// Provides a PRINT and INPUT for our graphical display.
// Uses the FONT module and takes control of the display.
//

#ifndef _CONSOLE_
#define _CONSOLE_



//
// Iniailises the CONSOLE output.
//

void CONSOLE_init();



//
// Prints the text.
//

void CONSOLE_print(char* fmt, ...);

//
// Gets a line of input from the user.
//

char* CONSOLE_input();



#endif
