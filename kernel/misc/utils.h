#include "stdint.h"
#include "stddef.h"
int abs (int i);
void swap(char *x, char *y);
char* reverse(char *buffer, int i, int j);
char* itoa(int64_t value, char* buffer, int base);
char* uitoa(uint64_t value, char* buffer, int base);
void* memcpy(void* dest, const void *src, size_t len);
size_t strlen(const char *str);
char* strcpy(char* dest, const char *src);
char* strcat(char *dest, const char *src);
int strcmp(const char* s1, const char* s2);