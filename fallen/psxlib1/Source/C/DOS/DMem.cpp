// Mem.cpp
// Guy Simmons, 10th February 1997.

#include	<MFHeader.h>


//---------------------------------------------------------------

bool SetupMemory()
{
	return	true;
}

//---------------------------------------------------------------

void ResetMemory()
{
}

//---------------------------------------------------------------

void* MemAlloc(ULONG size)
{
	size	=	(size+3)&0xfffffffc;
	return (void*)malloc(size);
}

//---------------------------------------------------------------

void MemFree(void* mem_ptr)
{
	free(mem_ptr);
}

//---------------------------------------------------------------

