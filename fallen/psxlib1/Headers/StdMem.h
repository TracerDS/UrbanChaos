// StdMem.h
// Guy Simmons, 18th December 1997

#ifndef STD_MEM_H
#define STD_MEM_H

//---------------------------------------------------------------

bool SetupMemory();
void ResetMemory();
void* MemAlloc(std::uint32_t size);
void MemFree(void* mem_ptr);

//---------------------------------------------------------------

#endif
