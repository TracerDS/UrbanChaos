// StdMem.cpp
// Guy Simmons, 18th December 1997

#include	<MFStdLib.h>

#define		INITIAL_HEAP_SIZE		(8*1024*1024)
#define		MAXIMUM_HEAP_SIZE		0

#ifdef DEBUG
// Allows heap debugging - logs interesting info, e.g. for memory leak tracking.
//#define HEAP_DEBUGGING_PLEASE_BOB defined
#endif


HANDLE MFHeap	=	nullptr;

#ifdef HEAP_DEBUGGING_PLEASE_BOB
struct HeapDebugInfo
{
	HeapDebugInfo	*pPrev;
	HeapDebugInfo	*pNext;
	std::uint32_t			ulSize;		// The size of this alloc.
	std::uint32_t			ulSeqNum;	// The malloc number.
};

#define MAX_NUM_DELNEW_TRACES 2000

int iNumNewDelTraces = 0;

struct newdeltrace
{
	void* pvAddr;
	std::uint32_t ulsize;
	std::uint32_t ulSequenceNumber;
} ndtList[MAX_NUM_DELNEW_TRACES];



HeapDebugInfo *pFirst = nullptr;
std::uint32_t ulCurrentSequenceNumber = 0;

// Set this to 1 in a debugger to dump the info.
volatile bool bDumpDebug = false;
// Set this to an ID you want to track in the debugger.
volatile std::uint32_t ulSpotted = -1;


void DumpDebug ( void )
{
	bDumpDebug = false;

	TRACE ( "\nMemory debug dump\n" );

	HeapDebugInfo *phdi = pFirst;
	while ( phdi != nullptr )
	{
		TRACE ( "ID<0x%x> size<0x%x> \n", phdi->ulSeqNum, phdi->ulSize );

		phdi = phdi->pNext;
	}

	TRACE ( "NewDel debug bump\n" );
	for ( int i = 0; i < iNumNewDelTraces; i++ )
	{
		TRACE ( "ID<0x%x> size<0x%x> \n", ndtList[i].ulSequenceNumber, ndtList[i].ulsize );
	}

	TRACE ( "\n" );

}


#endif

//---------------------------------------------------------------

bool SetupMemory()
{
#ifdef HEAP_DEBUGGING_PLEASE_BOB
	pFirst = nullptr;
	ulCurrentSequenceNumber = 0;
#endif
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
#ifdef HEAP_DEBUGGING_PLEASE_BOB
	pFirst = nullptr;
	// Sequence number is not reset.
#endif
}

//---------------------------------------------------------------

void* MemAlloc(std::uint32_t size)
{
#ifdef HEAP_DEBUGGING_PLEASE_BOB
	if ( bDumpDebug )
	{
		DumpDebug();
	}

	// Set up ulSpotted in a debugger to track interesting items.
	ASSERT ( ulSpotted != ulCurrentSequenceNumber );

	std::uint32_t ulOriginalSize = size;
	size += sizeof ( HeapDebugInfo );
#endif
	size	=	(size+3)&0xfffffffc;
	void* ptr = (void*)HeapAlloc(MFHeap,HEAP_ZERO_MEMORY,size);
	ASSERT ( ptr != nullptr );

#ifdef HEAP_DEBUGGING_PLEASE_BOB
	HeapDebugInfo *phdi = (HeapDebugInfo *)ptr;
	ptr = (void*)( (char*)ptr + sizeof ( HeapDebugInfo ) );

	phdi->ulSeqNum = ulCurrentSequenceNumber++;
	phdi->ulSize = ulOriginalSize;
	phdi->pNext = pFirst;
	phdi->pPrev = nullptr;
	if ( pFirst != nullptr )
	{
		ASSERT ( pFirst->pPrev == nullptr );
		pFirst->pPrev = phdi;
	}
	pFirst = phdi;
#endif

	return ptr;
}

void* MemReAlloc(void* ptr, std::uint32_t size)
{
	size = (size + 3) & 0xfffffffc;
	ptr = (void*)HeapReAlloc(MFHeap, HEAP_ZERO_MEMORY, ptr, size);
	ASSERT(ptr != nullptr);

	return ptr;
}

//---------------------------------------------------------------

void MemFree(void* mem_ptr)
{
#ifdef HEAP_DEBUGGING_PLEASE_BOB
	if ( bDumpDebug )
	{
		DumpDebug();
	}

	mem_ptr = (void*)( (char*)mem_ptr - sizeof ( HeapDebugInfo ) );
	HeapDebugInfo *phdi = (HeapDebugInfo *)mem_ptr;

	// Set up ulSpotted in a debugger to track interesting items.
	ASSERT ( ulSpotted != phdi->ulSeqNum );

	if ( phdi->pPrev != nullptr )
	{
		ASSERT ( phdi->pPrev->pNext == phdi );
		phdi->pPrev->pNext = phdi->pNext;
	}
	else
	{
		ASSERT ( pFirst == phdi );
		pFirst = phdi->pNext;
	}
	if ( phdi->pNext != nullptr )
	{
		ASSERT ( phdi->pNext->pPrev == phdi );
		phdi->pNext->pPrev = phdi->pPrev;
	}
#endif

	HeapFree(MFHeap,0,mem_ptr);
}

//---------------------------------------------------------------

/*
void MemClear(void* mem_ptr,std::uint32_t size)
{
}
*/

//---------------------------------------------------------------


#ifdef DEBUG
// Support for MFnew and MFdelete
#ifndef HEAP_DEBUGGING_PLEASE_BOB

// Do nothing.
void MFnewTrace ( void* pvAddr, size_t size )
{
}

void MFdeleteTrace ( void* pvAddr )
{
}

#else //#ifndef HEAP_DEBUGGING_PLEASE_BOB

void MFnewTrace ( void* pvAddr, size_t size )
{
	if ( bDumpDebug )
	{
		DumpDebug();
	}

	ASSERT ( iNumNewDelTraces < MAX_NUM_DELNEW_TRACES - 1 );

	ndtList[iNumNewDelTraces].pvAddr = pvAddr;
	ndtList[iNumNewDelTraces].ulsize = (std::uint32_t)size;
	ndtList[iNumNewDelTraces].ulSequenceNumber = ulCurrentSequenceNumber++;
	iNumNewDelTraces++;
}

void MFdeleteTrace ( void* pvAddr )
{
	if ( bDumpDebug )
	{
		DumpDebug();
	}

	// Search for this entry.
	bool bFound = false;
	for ( int i = 0; i < iNumNewDelTraces; i++ )
	{
		if ( ndtList[i].pvAddr == pvAddr )
		{
			iNumNewDelTraces--;
			ndtList[i] = ndtList[iNumNewDelTraces];
			bFound = true;
			break;
		}
	}
	ASSERT ( bFound );
}

#endif //#else //#ifndef HEAP_DEBUGGING_PLEASE_BOB


#endif





