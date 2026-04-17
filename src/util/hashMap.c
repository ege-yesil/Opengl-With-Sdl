#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "string.h"
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
size_t stringHash(void *k) {
    const char *s = (const char*)k;
    const int p = 31;
    const int m =  1e9 + 9;
    size_t hash = 0;
    size_t pow = 1;

    size_t i = 0;
    while (s[i] != '\0') { 
        char c = s[i];
        hash = (hash + (c - 'a' + 1) * pow) % m;
        pow = (pow * p) % m;
        i++;
    }
    return hash;
}

bool equalsIntHashMap(void *key1, void *key2) {
    if (*(int*)key1 == *(int*)key2) return true;
    return false;
}
bool equalsVertexKeyHashMap(void *a, void *b) {
    VertexKey *A = a;
    VertexKey *B = b;

    return A->v  == B->v &&
           A->vn == B->vn &&
           A->vt == B->vt;
}
bool equalsStringHashMap(void *key1, void *key2) {
    const char *k1 = (const char*)key1;
    const char *k2 = (const char*)key2;
    
    return strcmp(k1, k2) == 0;
}

static void padCstr(HashMap *map, char *dst, const char *src) {
    memset(dst, 0, map->keyStride);
    strncpy(dst, src, map->keyStride - 1);
}

void addHashMap_cstr(HashMap *map, const char *key, void *val) {
    char buffer[map->keyStride];
    padCstr(map, buffer, key); 
    addHashMap(map, buffer, val);
}
void addHashMap_cstr_i32(HashMap *map, const char *key, int32_t val) {
    char buffer[map->keyStride];
    padCstr(map, buffer, key); 
    addHashMap(map, buffer, &val);
}

void *getHashMap_cstr(HashMap *map, const char *key) {
    char buffer[map->keyStride];
    padCstr(map, buffer, key); 
    return getHashMap(map, buffer);
}
int32_t i32_getHashMap_cstr(HashMap *map, const char *key) {
    char buffer[map->keyStride];
    padCstr(map, buffer, key);
    void *o = getHashMap(map, buffer);
    if (o == NULL) return INT_MAX;
    return *(int32_t*)o;
}

void freeHashMap(HashMap *map) {
    for (size_t i = 0; i < map->capacity; i++) {
        if (map->entries[i].key != NULL)
            free(map->entries[i].key);
   
        if (map->entries[i].val != NULL)
            free(map->entries[i].val);
    }
    free(map->entries);
}

void resizeHashMap(HashMap *map, size_t capacity) {
    size_t oldCap = map->capacity;
    MapEntry *oldMap = map->entries;

    map->entries = calloc(capacity, sizeof(MapEntry));
    map->capacity = capacity;
    map->size = 0;

    for (size_t i = 0; i < capacity; i++) {
        map->entries[i].key = malloc(map->keyStride);
        map->entries[i].val = malloc(map->valStride);
        map->entries[i].occupation = EMPTY;
    }
    for (size_t i = 0; i < oldCap; i++) {
        if (oldMap[i].occupation == OCCUPIED)
            addHashMap(map, oldMap[i].key, oldMap[i].val);
    }
    
    free(oldMap);
}

void *getHashMap(HashMap *map, void *key) {
    size_t index = getHashMapIndex(map, key);
    if (index == SIZE_MAX) return NULL;
    return map->entries[index].val;
}

size_t getHashMapIndex(HashMap *map, void *key) {
    size_t index = map->hash(key) % map->capacity;
    while (map->entries[index].occupation != EMPTY && map->entries[index].occupation != DEAD) {
        if (map->equals(map->entries[index].key, key))
            return index;
 
        index = (index + 1) % map->capacity;
    }
    return SIZE_MAX; 
}

void removeHashMap(HashMap *map, void *key) {
    size_t index = getHashMapIndex(map, key); 
    if (index == SIZE_MAX) return;

    free(map->entries[index].key);
    free(map->entries[index].val);
    map->entries[index].key = NULL;
    map->entries[index].val = NULL;
    map->entries[index].occupation = DEAD;
    map->size--;
}



void addHashMap(HashMap *map, void *key, void *val) {
    size_t index = map->hash(key) % map->capacity; 

    while (map->entries[index].occupation == OCCUPIED) {
        if (map->equals(map->entries[index].key, key)) return;
        index = (index + 1) % map->capacity;
    }

    memcpy(map->entries[index].key, key, map->keyStride);
    memcpy(map->entries[index].val, val, map->valStride);
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

    o.entries = malloc(64 * sizeof(MapEntry));
    for (int i = 0; i < 64; i++) {
        o.entries[i].key = malloc(keySize);
        o.entries[i].val = malloc(valSize);
        o.entries[i].occupation = EMPTY;
    }

    return o;
}
