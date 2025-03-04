//
// The code generator and linker. Converts the output of the parse into
// an executable.
//

#ifndef _CG_
#define _CG_



//
// Takes the output of the PARSE module and spews out
// code to the given filename.  Returns false on failure.
// CG_output[] hold the errors/warnings/status of the
// code generation phase.
//

extern char* CG_output;
extern std::int32_t  CG_num_errors;
extern std::int32_t  CG_num_warnings;

#define CG_OUTPUT_EXECUTABLE  (1 << 0)
#define CG_OUTPUT_OBJECT_FILE (1 << 1)
#define CG_OUTPUT_DEBUG_INFO  (1 << 2)

std::int32_t CG_do(char* fname, std::int32_t output = CG_OUTPUT_EXECUTABLE);





#endif
