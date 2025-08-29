// Bucket.cpp
// Guy Simmons, 24th October 1997.

#include <DDEngine/Headers/Bucket.h>
#include <DDEngine/Headers/Engine.h>

std::uint8_t e_bucket_pool[BUCKET_POOL_SIZE],
    *e_buckets,
    *e_end_buckets;
BucketHead *bucket_lists[MAX_LISTS][MAX_BUCKETS + 1];

//---------------------------------------------------------------

void init_buckets() {
    memset(bucket_lists, 0, sizeof(bucket_lists));
    reset_buckets();
}

//---------------------------------------------------------------
