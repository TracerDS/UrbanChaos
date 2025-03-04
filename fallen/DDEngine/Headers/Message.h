//
// Message passing to the user.
//


#ifndef _MSG_
#define _MSG_



//
// Clears all current messages.	You don't have to call this at the
// start of the program.
//

void MSG_clear();

//
// Adds the current to the list of messages displayed to the user. The string is
// copied to a non-volatile area of memory, so you can use a local string to pass
// the message if you like.
//
// If you add the same message more than once, only one copy will appear.
//

//void MSG_add(char* message, ...);


//
// Draws the messages onto the screen. It expects the screen to be locked.
//

void MSG_draw();



#endif
