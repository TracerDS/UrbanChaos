// StdMem.h
// Guy Simmons, 18th December 1997

#pragma once

//---------------------------------------------------------------

bool SetupMemory();
void ResetMemory();
void* MemAlloc(std::uint32_t size);
void* MemReAlloc(void* ptr, std::uint32_t size);
void MemFree(void* mem_ptr);



#ifdef DEBUG
void MFnewTrace ( void* pvAddr, size_t size );
void MFdeleteTrace ( void* pvAddr );
#endif


// Some templated new and delete stand-ins.
template <class T> T *MFnew ( void )
{
	T *ptr = new T;
#ifdef DEBUG
	MFnewTrace ( ptr, sizeof ( ptr ) );
#endif
	return ptr;
}

template<class T> void MFdelete(T *thing)
{
#ifdef DEBUG
	MFdeleteTrace ( thing );
#endif
	delete thing;
}

//---------------------------------------------------------------

