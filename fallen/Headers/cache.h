//
// A cache designed for storing light info.
//

#pragma once


typedef std::uint8_t CACHE_Index;


//
// Initialises all the cache elements.
//

void CACHE_init();

//
// Creates a cache entry for the given thing. It saves all the
// light info.  If it returns nullptr, then it couldn't cache the
// info.
//

CACHE_Index CACHE_create(
				std::int32_t key,
				void* data,
				std::uint16_t num_bytes);

//
// Returns true if the given CACHE_Index contains valid info.
//

std::int32_t CACHE_is_valid(CACHE_Index c_index);

//
// Returns the data associated with the cache_index.
//

typedef struct
{
	std::int32_t key;
	void* data;
	std::int32_t num_bytes;

} CACHE_Info;

CACHE_Info CACHE_get_info(CACHE_Index c_index);


//
// Invalidates the given cache entry.
//

void CACHE_invalidate(CACHE_Index c_index);
void CACHE_invalidate_all();


//
// You can flag cache entries. All cache entries are created
// already with their flag set.
//

void CACHE_flag_clear_all();
void CACHE_flag_set      (CACHE_Index c_index);
void CACHE_flag_clear    (CACHE_Index c_index);

//
// Invalidates all cache entries that don't have their flag set.
//

void CACHE_invalidate_unflagged();



