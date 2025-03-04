// MFMem.h
// Guy Simmons, 10th February 1997.

#ifndef _MF_MEM_H_
#define _MF_MEM_H_

bool	SetupMemory();
void	ResetMemory();
void	*MemAlloc(std::uint32_t size);
void	MemFree(void* mem_ptr);

#endif
