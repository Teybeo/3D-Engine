#include "utils/hashMap.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define WIDENING_FACTOR 10

HashMap* HashMap_create(unsigned int max_size) {

    HashMap* hashmap = malloc(sizeof(HashMap));
    hashmap->domain_size = max_size;
    hashmap->real_size = max_size * WIDENING_FACTOR;
    hashmap->current_size = 0;
    hashmap->collisions = 0;
    hashmap->buckets = calloc(sizeof(Bucket), hashmap->real_size );

    int hashmap_size = (sizeof(Bucket) * hashmap->domain_size) / 1000000;

    printf("\tNew hashmap: %d k elements, widening factor: %dx\n", hashmap->domain_size/1000, WIDENING_FACTOR);
    printf("\tSize %d Mo, Real size %d Mo\n", hashmap_size, hashmap_size * WIDENING_FACTOR);

    return hashmap;
}

unsigned int Vec3_hash(Vec3 vec, unsigned int max_size) {

    unsigned int hash;
    hash =  (((unsigned int) vec.x) * 73856093);
    hash ^= (((unsigned int) vec.y) * 19349663);
    hash ^= (((unsigned int) vec.z) * 83492791);

    return hash % max_size;
}unsigned int Vertex_hash(Vertex key, unsigned int max_size) {

    unsigned int hash;
    hash =  (unsigned int) (key.pos.x * 73856093);
    hash ^= (unsigned int) (key.pos.y * 19349663);
    hash ^= (unsigned int) (key.pos.z * 83492791);
    hash ^= (unsigned int) (key.normal.x * 1210439);
    hash ^= (unsigned int) (key.normal.y * 361093);
    hash ^= (unsigned int) (key.normal.z * 760637);
    hash ^= (unsigned int) (key.uv.x * 643099);
    hash ^= (unsigned int) (key.uv.y * 1299689);

    return hash % max_size;
}

unsigned int APHash(Vec3 vec, unsigned int max_size)
{
   unsigned int hash = 0xAAAAAAAA;

   hash =  (hash <<  7) ^ ((unsigned int)vec.x) * (hash >> 3);
   hash ^= (~((hash << 11) + (((unsigned int)vec.y) ^ (hash >> 5))));
   hash ^= (hash <<  7) ^ ((unsigned int)vec.z) * (hash >> 3);

   return hash % max_size;
}
unsigned int APHashVertex(Vertex key, unsigned int max_size)
{
   unsigned int hash = 0xAAAAAAAA;

   hash =  (hash <<  7) ^ ((unsigned int)key.pos.x) * (hash >> 3);
   hash ^= (~((hash << 11) + (((unsigned int)key.pos.y) ^ (hash >> 5))));
   hash ^= (hash <<  7) ^ ((unsigned int)key.pos.z) * (hash >> 3);

   hash ^= (~((hash << 11) + (((unsigned int)key.normal.x) ^ (hash >> 5))));
   hash ^= (hash <<  7) ^ ((unsigned int)key.normal.z) * (hash >> 3);
   hash ^= (~((hash << 11) + (((unsigned int)key.normal.y) ^ (hash >> 5))));

   hash ^= (hash <<  7) ^ ((unsigned int)key.uv.x) * (hash >> 3);
   hash ^= (~((hash << 11) + (((unsigned int)key.uv.y) ^ (hash >> 5))));

   return hash % max_size;
}

Bucket* getLastBucket(Bucket* bucket) {

    while (bucket->next != NULL)
        bucket = bucket->next;

    return bucket;
}

void HashMap_add(HashMap* hashmap, Vertex key, int value) {

    unsigned int hash = Vertex_hash(key, hashmap->real_size);

    // The bucket for this hash is already used for this hash, so there is a collision
    if (hashmap->buckets[hash].used == true)
    {
        // Create a new bucket and attach it to this one
        Bucket* lastBucket = getLastBucket(&hashmap->buckets[hash]);
        lastBucket->next = calloc(sizeof(Bucket), 1);
        lastBucket->next->key = key;
        lastBucket->next->value = value;
        hashmap->collisions++;
    }
    else // The bucket is free, use it
    {
        hashmap->buckets[hash].value = value;
        hashmap->buckets[hash].key = key;
        hashmap->buckets[hash].used = true;
        hashmap->current_size++;
    }

}

static inline int Vertex_Cmp(Vertex a, Vertex b) {
    return memcmp(&a, &b, sizeof(Vertex));
}

int HashMap_get(HashMap* hashmap, Vertex key) {

    unsigned int hash = Vertex_hash(key, hashmap->real_size);

    // We found a bucket for the hash of this key
    if (hashmap->buckets[hash].used == true)
    {
        // But we have to check if the keys are equal in case of hash collision

        // The keys matches, all good
        if (Vertex_Cmp(hashmap->buckets[hash].key, key) == 0) {
            return hashmap->buckets[hash].value;
        }
        else // The keys doesn't match = collision
        {
            // Try to find the good key in the collision list
            Bucket* bucket = hashmap->buckets[hash].next;

            while (bucket != NULL && Vertex_Cmp(bucket->key, key) != 0)
                bucket = bucket->next;

            if (bucket == NULL)
                return -1; // No buckets with this key in the collision list
            else
                return bucket->value; // We found the bucket for this key
        }
    }
    else // No bucket found for this key
    {
        return -1;
    }
}

void HashMap_printStats(HashMap* hashmap) {

    printf("\tCollisions: %d of %d => ", hashmap->collisions, hashmap->domain_size);
    printf("%f%% collision rate\n", ((float)hashmap->collisions)/hashmap->domain_size * 100.0);
}
