#include <stdlib.h>
#include <stdint.h>

#include "hashMap.h"

size_t intHash(void *k) {
    uint32_t x = *(uint32_t*)k;

    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;

    return x;
}

size_t vertexKeyHash(void *k) {
    VertexKey *key = (VertexKey*)k;

    size_t h = 0;
    h ^= key->v  * 73856093;
    h ^= key->vt * 19349663;
    h ^= key->vn * 83492791;

    return h;
}

bool equalsIntHashMap(void *key1, void *key2) {
    if (*(int*)key1 == *(int*)key2) return true;
    return false;
}

bool equalsVertexKeyHashMap(void *key1, void *key2) {
    VertexKey vkey1 = *(VertexKey*)key1;
    VertexKey vkey2 = *(VertexKey*)key2;

    if (vkey1.v != vkey2.v) return false;
    if (vkey1.vt != vkey2.vt) return false;
    if (vkey1.vn != vkey2.vn) return false;

    return true;
}

void resizeHashMap(HashMap *map, size_t capacity) {
    size_t oldCap = map->capacity;
    MapEntry *oldMap = map->entries;

    map->entries = calloc(capacity, sizeof(MapEntry));
    map->capacity = capacity;
    for (size_t i = 0; i < oldCap; i++) {
        if (oldMap[i].occupation == OCCUPIED)
            addHashMap(map, oldMap[i].key, oldMap[i].val);
    }
    
    free(oldMap);
}

size_t getHashMap(HashMap *map, void *key) {
    size_t index = map->hash(key) % map->capacity;
    while (map->entries[index].occupation != EMPTY) {
        if (map->equals(map->entries[index].key, key))
            return index;

        index = (index + 1) % map->capacity;
    }
    return SIZE_MAX; 
}

void removeHashMap(HashMap *map, void *key) {
    size_t index = getHashMap(map, key); 
    if (index == SIZE_MAX) return;

    map->entries[index].occupation = DEAD;
    map->size--;
}

void addHashMap(HashMap *map, void *key, void *val) {
    size_t index = map->hash(key) % map->capacity; 

    while (map->entries[index].occupation == OCCUPIED) {
        if (map->equals(map->entries[index].key, key)) return;
        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = key;
    map->entries[index].val = val;
    map->entries[index].occupation = OCCUPIED;

    map->size++;
    if ((float)map->size / map->capacity > 0.7) resizeHashMap(map, map->capacity * 2);
}

HashMap makeHashMap(size_t keySize, size_t valSize) {
    HashMap o;
    o.capacity = 64;
    o.size = 0;
    o.keyStride = keySize;
    o.valStride = valSize;

    o.entries = malloc(64);
    for (int i = 0; i < 64; i++) {
        o.entries[i].key = malloc(keySize);
        o.entries[i].val = malloc(valSize);
    } 

    return o;
}
