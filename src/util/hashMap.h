/*
 * this hash map implementation is made mainly for loading object files more efficiently
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>

typedef struct {
    int v;
    int vt;
    int vn;
} VertexKey;

enum EntryState {
    EMPTY,
    OCCUPIED,
    DEAD
};

typedef struct {
    void *key;
    void *val;
    enum EntryState occupation;
} MapEntry;

typedef struct {
    size_t capacity;
    size_t size;

    size_t keyStride;
    size_t valStride;
    
    size_t (*hash)(void*);
    bool (*equals)(void*, void*);

    MapEntry *entries;
} HashMap;

// hash functions
size_t intHash(void *key);
size_t vertexKeyHash(void *key); 
bool equalsIntHashMap(void *key1, void *key2);
bool equalsVertexKeyHashMap(void *key1, void *key2);

// hashMap functions
void reserveHashMap(HashMap *map, size_t capacity);
void addHashMap(HashMap *map, void *key, void *val);
void removeHashMap(HashMap *map, void *key);
size_t getHashMap(HashMap *map, void *key);         // returns index of given entry
HashMap makeHashMap(size_t keySize, size_t valSize);

#endif
