//
// A memory heap with defragmentation. It is designed to
// given largish blocks of memory out...
//

#ifndef HEAP_H
#define HEAP_H

//
// Initialises the heap.
// Returns a block of memory.
// Gives back an unused block of memory.
//

void HEAP_init();
void* HEAP_get(std::int32_t num_bytes);
void HEAP_give(void*, std::int32_t num_bytes);

//
// A chunk of useful memory for you to do with as you please.
//

#define HEAP_PAD_SIZE (1024 * 4)

extern std::uint8_t HEAP_pad[HEAP_PAD_SIZE];

#endif
