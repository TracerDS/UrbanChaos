// Mem.cpp
// Guy Simmons, 10th February 1997.


#include	<MFStdLib.h>
#include	"libapi.h"

#include	"myheap.h"
#include	"psxeng.h"

#if 0
#else
void* dud_malloc_init(void* base,void* end);
void* dud_malloc(std::int32_t size);
void dud_free(void* p);
#endif

std::uint32_t heap_index=0;
std::int32_t total_mem_size;
std::int32_t bucket_mem_size;

//---------------------------------------------------------------

bool SetupMemory()
{

#ifdef FS_ISO9660
#ifndef VERSION_DEMO
	total_mem_size=0x80200000-(std::int32_t)my_heap;
#else
	total_mem_size=0x801fff00-(std::int32_t)my_heap;
#endif
#else
	total_mem_size=0x80285b08-(std::int32_t)my_heap;
#endif

	InitHeap3((void*)&my_heap[0],total_mem_size);
	printf("Allocated Memory: %d\n",total_mem_size);

	return(true);

}

//---------------------------------------------------------------

void ResetMemory()
{
}

//---------------------------------------------------------------

void* MemAlloc(std::uint32_t size)
{
	/*
	std::int32_t	pos;
	pos=heap_index;
	if(heap_index+size<INITIAL_HEAP_SIZE)
		heap_index+=size;
	*/
#if 1
	size+=8;

	return((void*)malloc3(size));
#else
	return((void*)dud_malloc(size));
#endif
}

//---------------------------------------------------------------

void MemFree(void* mem_ptr)
{
#if 1
	free3(mem_ptr);
#else
	dud_free(mem_ptr);
#endif
}

//---------------------------------------------------------------

void ZeroMemory(void* mem_ptr,std::uint32_t size)
{
	memset((std::uint8_t*)mem_ptr,0,size);

}


//---------------------------------------------------------------

