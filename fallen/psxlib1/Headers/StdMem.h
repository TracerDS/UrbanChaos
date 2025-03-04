// StdMem.h
// Guy Simmons, 18th December 1997

#pragma once

//---------------------------------------------------------------

bool SetupMemory();
void ResetMemory();
void* MemAlloc(std::uint32_t size);
void MemFree(void* mem_ptr);

//---------------------------------------------------------------

