//
// A symbol table.
//

#ifndef _ST_
#define _ST_


//
// There is a stack of symbol tables. The bottom one contains
// system defined variables and doesn't change. The next one
// contains variables from the libraries that you the program
// uses. The one above that contains globals and all the others
// above that are for function definitions.
//

#define ST_TABLE_SYSTEM  0
#define ST_TABLE_LIBRARY 1
#define ST_TABLE_GLOBAL  2
#define ST_TABLE_LOCAL   3
#define ST_TABLE_NUMBER  4		// The number of symbol tables in the stack.


//
// Initialises all the symbol tables. Call once at the start
// of the whole program.
//

void ST_init();



//
// Adds a symbol value pair to the given symbol table.
//
// DONT ADD THE SAME SYMBOL TWICE TO THE SAME TABLE!
//

void ST_add(std::int32_t table, char* string, std::int32_t value, std::int32_t flag);


//
// Finds the symbol. It checks the CALL,LOCAL,GLOBAL,LIBRRARY,SYSTEM
// symbol tables in that order. Returns true if it found the symbol.
//

extern std::int32_t  ST_found_table;	// The table the symbol was in.
extern std::int32_t  ST_found_value;	// The value associated with that symbol.
extern std::int32_t  ST_found_flag;
extern char* ST_found_string;

std::int32_t ST_find(char* string);


//
// Changes the flag variable associated with the given string.
// It is an error if the symbol is not found.
//

void ST_update_flag(char* string, std::int32_t new_flag);



//
// Finds everything in the symbol tables. Call ST_find_all_start(),
// then every call to ST_find_all_next() that returns true fills in
// the ST_found_* variables. When a call to ST_find_all_next() returns
// false there is nothing left to find.
//

void ST_find_all_start();
std::int32_t ST_find_all_next ();



//
// Clears all the symbols tables or a particular one.
//

void ST_clear    (std::int32_t table);
void ST_clear_all();



#endif
