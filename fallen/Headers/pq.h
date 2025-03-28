//
// The pq module is static code. You must #include "pq.cpp" into the
// source file the needs the heap after defining
//
//  PQ_Type	(typedef)
//	PQ_HEAP_MAX_SIZE
//  std::int32_t PQ_better(PQ_Type *a, PQ_Type *b);
//

//
// Initialises the heap to be empty.
// Adds an element to the heap
// Removes the best element from the heap
// Returns the best heap element.
//

static void PQ_init();
static void PQ_add(PQ_Type);
static void PQ_remove();
static PQ_Type PQ_best();
static std::int32_t PQ_empty();