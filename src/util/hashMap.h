/* this hash map implementation
 * USAGE:
 * HashMap map = makeHashMap(sizeof(int), sizeof(int));
 * map.hash = intHash;
 * map.equals = equalsIntHashMap;
 *
 * size_t index = getHashMap(&map, &key);
 * int val = *(int*)map.entries[index].val;
*/

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>

typedef struct {
    uint32_t v;
    uint32_t vn;
    uint32_t vt;
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
size_t stringHash(void *key);
bool equalsIntHashMap(void *key1, void *key2);
bool equalsVertexKeyHashMap(void *key1, void *key2);
bool equalsStringHashMap(void *key1, void *key2);

// helper functions
// NOTE: NOT OPTIMAL FOR MEMORY
void addHashMap_cstr(HashMap *map, const char *key, void *val); // wastes a little space but 
                                                        // it copies the part of which a string fits and 
                                                        // if there is any empty space it pads it with \0
void addHashMap_cstr_i32(HashMap *map, const char *key, int32_t val);
void *getHashMap_cstr(HashMap *map, const char *key);
int32_t i32_getHashMap_cstr(HashMap *map, const char *key);

// hashMap functions
void freeHashMap(HashMap *map);
void reserveHashMap(HashMap *map, size_t capacity);
void addHashMap(HashMap *map, void *key, void *val);
void removeHashMap(HashMap *map, void *key);
void *getHashMap(HashMap *map, void *key);
size_t getHashMapIndex(HashMap *map, void *key);
HashMap makeHashMap(size_t keySize, size_t valSize);

#endif
