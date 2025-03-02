// StdMem.h
// Guy Simmons, 18th December 1997

#ifndef	STD_MEM_H
#define	STD_MEM_H

//---------------------------------------------------------------

bool	SetupMemory();
void	ResetMemory();
void	*MemAlloc(ULONG size);
void	MemFree(void *mem_ptr);

//---------------------------------------------------------------

#endif
