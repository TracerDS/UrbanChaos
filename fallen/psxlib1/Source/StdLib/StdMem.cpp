// StdMem.cpp
// Guy Simmons, 18th December 1997

#include	<MFStdLib.h>

#define		INITIAL_HEAP_SIZE		(8*1024*1024)
#define		MAXIMUM_HEAP_SIZE		0

HANDLE MFHeap	=	nullptr;

//---------------------------------------------------------------

bool SetupMemory()
{
	if(MFHeap==nullptr)
	{
	   MFHeap	=	HeapCreate(0,INITIAL_HEAP_SIZE,MAXIMUM_HEAP_SIZE);
	}
	if(MFHeap)
		return	true;
	else
		return	false;
}

//---------------------------------------------------------------

void ResetMemory()
{
	if(MFHeap)
	{
		HeapDestroy(MFHeap);
		MFHeap	=	nullptr;
	}
}

//---------------------------------------------------------------

void* MemAlloc(std::uint32_t size)
{
	size	=	(size+3)&0xfffffffc;
	return (void*)HeapAlloc(MFHeap,HEAP_ZERO_MEMORY,size);
}

//---------------------------------------------------------------

void MemFree(void* mem_ptr)
{
	HeapFree(MFHeap,0,mem_ptr);
}

//---------------------------------------------------------------

/*
void MemClear(void* mem_ptr,std::uint32_t size)
{
}
*/

//---------------------------------------------------------------

