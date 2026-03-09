#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"

char *readFile(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;
    
    size_t size = 256;
    size_t realSize = 0;
    char *str = calloc(size, sizeof(char));
    if (!str) {
        close(fd);
        return NULL;
    }

    while (1) { 
        ssize_t byte = read(fd, str + realSize, size - realSize);
        if (byte < 0) {
            free(str);
            close(fd);
            return NULL;
        }
        if (byte == 0) break;
        realSize += byte;
        if (realSize == size) {
            size *= 2;
            char *tmp = realloc(str, size);
            if (!tmp) {
                close(fd);
                free(str);
                return NULL;
            }
            str = tmp;
        }
    }
    char *tmp = realloc(str, realSize + 1);
    if (tmp) {
        str = tmp;
        str[realSize] = '\0';
    } else {
        free(str);
        str = NULL;
    }
    close(fd);
    return str; 
}

String intToStr(int n) {
    String o = makeStr("0");
    if (n == 0)
        return o;
    popStr(&o, 1);

    while (n > 0) {
        char add = (char)(n % 10 + '0');
        appendStrC(&o, &add, 1); 
        n /= 10;
    }

    // reverse
    for (int i = 0; i < o.size / 2; i++) {
        size_t j = o.size - 1 - i;
        char tmp = o.str[i];
        o.str[i] = o.str[j];
        o.str[j] = tmp;
    }

    return o;
}

String getParentDir(const char *path) {
    String o;
    char *slash = strrchr(path, '/');
    if (!slash) return makeStr("");
    size_t len = slash - path + 1;
    o = makeStrN(path, len);

    return o;
}

String mergeDir(const char *parent, const char *child) {
    size_t sizeParent = strlen(parent);
    String o = makeStr(parent);
    char *betterChild = (char*)child; 
    if (child[0] == '/') betterChild++;
    if (parent[sizeParent - 1] != '/') appendStrC(&o, "/", 1);
    
    appendStrC(&o, betterChild, strlen(betterChild));

    return o;
}

String makeStr(const char *str) {
    String o;
    o.size = strlen(str);
    o.capacity = o.size + 1;
    o.str = malloc(o.capacity);
    memcpy(o.str, str, o.capacity);
    return o;
}

String makeStrN(const char *str, size_t n) {
    String o;
    o.size = n;
    o.capacity = o.size + 1;
    o.str = malloc(o.capacity);
    memcpy(o.str, str, n);
    o.str[n] = '\0';
    return o;
}

uint8_t reserveStr(String *s, size_t capacity) {
    if (capacity <= s->capacity)
        return 1;
    
    size_t cap = s->capacity;
    if (s->capacity == 0) cap = 16; 
    while (capacity > cap)
        cap *= 2;
    
    char *new = realloc(s->str, cap);
    if (!new) return 1;
    s->str = new;
    s->capacity = cap;
    return 0;
}

void appendStr(String *s1, String s2, size_t size) {
    reserveStr(s1, s1->size + size + 1);
    memcpy(s1->str + s1->size, s2.str, size);
    s1->size += size;
    s1->str[s1->size] = '\0'; 
}
void appendStrC(String *s1, char *s2, size_t size) {
    reserveStr(s1, s1->size + size + 1);
    memcpy(s1->str + s1->size, s2, size);
    s1->size += size;
    s1->str[s1->size] = '\0'; 
}
void appendStrN(String *s1, int n) {
    String s = intToStr(n);    
    appendStr(s1, s, s.size);
    free(s.str);
}

void popStr(String *s, size_t count) {
    size_t popStart = s->size - count;
    if (popStart <= 0) return;
    for (int i = popStart; i < s->size; i++) {
        s->str[i] = (char)0;
    }
};


