//
// The linker
//

#ifndef _LINK_
#define _LINK_


// ========================================================
//
// STRUCTURES IN THE OBJECT FILE.
//
// ========================================================

typedef struct
{
	std::uint16_t index;	// This is the number of the global.
	std::uint8_t export;	// true => This global is exported
	std::uint8_t local;	// true => This global is local to the file.
	std::int32_t name;		// Index into the debug data

} LINK_Global;

typedef struct
{
	std::int32_t name;
	std::int32_t export;		// true => this function is exported.
	std::int32_t line_start;	// The first and last line of the function body
	std::int32_t line_end;
	std::int32_t num_args;		// The number of arguments to the function

} LINK_Function;

typedef struct
{
	std::int32_t instruction;	// The first instruction generated for this line of code.

} LINK_Line;

typedef struct
{
	std::int32_t instruction;	// Index into instruction memory (std::int32_t *..) that contains a jump address.

} LINK_Jump;

typedef struct
{
	std::int32_t index;		// This is the number of the field.
	std::int32_t name;			// Index into the debug data.

} LINK_Field;

typedef struct
{
	std::int32_t instruction;	// Index into instruction memory that contains a global index.

} LINK_Globalref;

typedef struct
{
	std::int32_t name;			// Index into debug data for the name of the undefined function.
	std::int32_t instruction;	// Index into instruction memory that should contain the GOSUB jump to the function.

} LINK_Undefref;

typedef struct
{
	std::int32_t instruction;	// Index into the instruction memory for which field.

} LINK_Fieldref;

typedef struct
{
	std::int32_t instruction;

} LINK_Datatableref;

typedef struct
{
	std::int32_t version;
	std::int32_t num_instructions;		// Each instruction is a std::int32_t
	std::int32_t data_table_length_in_bytes;
	std::int32_t num_globals;
	std::int32_t num_functions;
	std::int32_t num_lines;
	std::int32_t num_jumps;
	std::int32_t num_fields;
	std::int32_t num_global_refs;
	std::int32_t num_undef_refs;
	std::int32_t num_field_refs;
	std::int32_t num_data_table_refs;
	
} LINK_Header;


// ========================================================
//
// THE FORMAT OF THE OBJECT FILE.
//
// ========================================================

//
// LINK_Header    lh
//
// std::int32_t             Instructions           [lh.num_instructions          ]  Followed by MAGIC std::int32_t(12345678)
// char             Data table             [lh.data_table_length_in_bytes]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Global       Globals                [lh.num_globals               ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Function     Functions              [lh.num_functions             ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Line         Lines                  [lh.num_lines                 ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Jump         Jumps                  [lh.num_jumps                 ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Field        Fields                 [lh.num_fields                ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Globalref    Gobal refs             [lh.num_global_refs           ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Undefref     Undefined function refs[lh.num_undefined_funcion_refs]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Fieldref     Field refs             [lh.num_field_refs            ]	 Followed by MAGIC std::int32_t(12345678)
// LINK_Datatableref Data table refs        [lh.num_data_table_refs       ]	 Followed by MAGIC std::int32_t(12345678)
//
// std::int32_t          debug_data_length_in_bytes
// char          debug data[debug_data_length_in_bytes]
//




// ========================================================
//
// THE LINKER
//
// ========================================================

//
// Links all the object files into an executable. Returns false on failure.
//

std::int32_t LINK_do(char* object_fname[], std::int32_t num_object_files, char* exec_fname);




#endif
