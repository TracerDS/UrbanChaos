//
// Loads an environment from a file.
//

#pragma once


//
// now we use standard Windows .INI files
//

#ifndef TARGET_DC

void ENV_load(char* fname);

#else

// Oh no we don't...


// Feed it the start of the env save/load block, and
// it loads the block and returns a pointer to the end of the block.
extern char *ENV_load ( char *pcData );

// Call once with pcData = nullptr, and it returns the number of
// bytes it needs to save the data. Call it again with a pointer
// to the start of this data, and it will actually fill in the data
// (and return the number of bytes again).
extern int ENV_save ( char *pcData );


#endif

//
// retrieve values
//

char* ENV_get_value_string(char* name, char* section = "Game");				// returns nullptr if not found - NOTE: string is in a static buffer
std::int32_t  ENV_get_value_number(char* name, std::int32_t def, char* section = "Game");	// returns def if not found

//
// store values
//

void ENV_set_value_string(char* name, char* value, char* section = "Game");
void ENV_set_value_number(char* name, std::int32_t value, char* section = "Game");

