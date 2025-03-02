//
// For displaying debug messages.
//

#ifndef _LOG_
#define _LOG_


void LOG_init   ();
void LOG_message(ULONG colour, CBYTE* fmt, ...);
void LOG_draw   ();
void LOG_fini   ();

//
// Sends the message to c:\debuglog.txt
//

void LOG_file(CBYTE* fmt, ...);


#endif
