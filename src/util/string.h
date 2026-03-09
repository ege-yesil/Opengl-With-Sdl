#ifndef STRING_H 
#define STRING_H 

#include <stdint.h>

typedef struct {
    char *str;
    size_t size;
    size_t capacity;
} String;

// Misc functions
char *readFile(const char *path);
String intToStr(int n);
String getParentDir(const char *path); // pass a directory get the parent directory
String mergeDir(const char *parent, const char *child); // merge 2 directories in a meaningful way

// String funcitons
String makeStr(const char *str);
String makeStrN(const char  *str, size_t n);
uint8_t reserveStr(String *s, size_t capacity);
void appendStr(String *s1, String s2, size_t size);
void appendStrC(String *s1, char *s2, size_t size);
void appendStrN(String *s1, int n);
void popStr(String *s, size_t count);

#endif
