//
// For displaying debug messages.
//

#ifndef _LOG_
#define _LOG_


void LOG_init   ();
void LOG_message(std::uint32_t colour, char* fmt, ...);
void LOG_draw   ();
void LOG_fini   ();

//
// Sends the message to c:\debuglog.txt
//

void LOG_file(char* fmt, ...);


#endif
