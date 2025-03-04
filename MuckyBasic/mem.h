//
// Memory allocation for the virtual machine.
//

#ifndef _MEM_
#define _MEM_


//
// Allocates an area of memory.
//

void* MEM_alloc(std::int32_t num_bytes);


//
// Frees an area of memory allocated with MEM_alloc();
//

void MEM_free(void* memory);


//
// Returns the length in bytes of the given block.
//

std::int32_t MEM_block_size(void* memory);



//
// Returns the total amount of memory allocated in bytes.
//

std::int32_t MEM_total_bytes_allocated();




#endif
