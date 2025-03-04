//
// For displaying debug messages.
//

#pragma once


void LOG_init   ();
void LOG_message(std::uint32_t colour, char* fmt, ...);
void LOG_draw   ();
void LOG_fini   ();

//
// Sends the message to c:\debuglog.txt
//

void LOG_file(char* fmt, ...);


