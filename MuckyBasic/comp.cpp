#include "always.h"
#include "cg.h"
#include "lex.h"
#include "parse.h"
#include "st.h"
#include "vm.h"

#define COMP_MAX_PROGRAM (512 * 1024)		// Huge buffer!

char COMP_program[COMP_MAX_PROGRAM];

std::int32_t COMP_do(char* fname_input, char* fname_output)
{
	//
	// Load program.
	//

	FILE *handle = fopen(fname_input, "rb");

	if (!handle)
	{
		//
		// Could not open input file.
		//

		return false;
	}

	//
	// Load in the source.
	//

	std::int32_t bytes_read;

	bytes_read = fread(COMP_program, sizeof(char), COMP_MAX_PROGRAM, handle);

	if (bytes_read == 0)
	{
		//
		// No data read?
		//

		return false;
	}

	if (bytes_read >= COMP_MAX_PROGRAM)
	{
		//
		// Our source buffer isn't large enough!
		//

		return false;
	}

	ST_init();

	PARSE_do(COMP_program);

	CG_do(fname_output);

	return true;
}
