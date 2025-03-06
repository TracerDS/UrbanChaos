// Mem.cpp
// Guy Simmons, 10th February 1997.

#include <MFHeader.h>

#define INITIAL_HEAP_SIZE (18 * 1024 * 1024)
#define MAXIMUM_HEAP_SIZE (24 * 1024 * 1024)

HANDLE MFHeap = nullptr;

//---------------------------------------------------------------

bool SetupMemory() {
    if (MFHeap == nullptr) {
        MFHeap = HeapCreate(0, INITIAL_HEAP_SIZE, MAXIMUM_HEAP_SIZE);
    }
    ERROR_MSG(MFHeap, "Can't setup memory.")
    if (MFHeap)
        return true;
    else
        return false;
}

//---------------------------------------------------------------

void ResetMemory() {
    if (MFHeap) {
        HeapDestroy(MFHeap);
        MFHeap = nullptr;
    }
}

//---------------------------------------------------------------

void* MemAlloc(std::uint32_t size) {
    size = (size + 3) & 0xfffffffc;
    return (void*) HeapAlloc(MFHeap, HEAP_ZERO_MEMORY, size);
}

//---------------------------------------------------------------

void MemFree(void* mem_ptr) {
    HeapFree(MFHeap, 0, mem_ptr);
}

//---------------------------------------------------------------

void MemClear(void* mem_ptr, std::uint32_t size) {
    /*
            std::uint32_t	c0;

            for(c0=0;c0<size;c0++)
                    *(mem_ptr++)	=	0;
    */
}

//---------------------------------------------------------------
