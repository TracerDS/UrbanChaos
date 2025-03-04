//
// Memory allocation for the virtual machine.
//

#include "always.h"
#include "mem.h"
#include <malloc.h>


//
// The number of bytes of memory allocated by malloc() - not
// neccessarily the same number as that passed to MEM_alloc()
//

std::int32_t MEM_bytes_used;


void* MEM_alloc(std::int32_t num_bytes)
{
	void* ans;

	ans = malloc(num_bytes);

	ASSERT(ans);

	MEM_bytes_used += _msize(ans);

	return ans;
}


void MEM_free(void* memory)
{
	ASSERT(memory);

	MEM_bytes_used -= _msize(memory);

	free(memory);
}




std::int32_t MEM_block_size(void* memory)
{
	ASSERT(memory);

	return _msize(memory);
}


std::int32_t MEM_total_bytes_allocated()
{
	return MEM_bytes_used;
}
