// MFMem.h
// Guy Simmons, 10th February 1997.

#pragma once

bool SetupMemory();
void ResetMemory();
void* MemAlloc(std::uint32_t size);
void MemFree(void* mem_ptr);

