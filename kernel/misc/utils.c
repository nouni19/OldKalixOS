#include "utils.h"
// all of these are copied
void *memset (void *dest, uint8_t val, size_t len){
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}
int abs (int i){
  return i < 0 ? -i : i;
}
void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
char* reverse(char *buffer, int i, int j){
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
    return buffer;
}
char* itoa(int64_t value, char* buffer, int base){
    if (base < 2 || base > 32) {
        return buffer;
    }
    long n = abs(value);
    int i = 0;
    while (n){
        int r = n % base;
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
        n = n / base;
    }
    if (i == 0) {
        buffer[i++] = '0';
    }
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
    buffer[i] = '\0';
    return reverse(buffer, 0, i - 1);
}
char* uitoa(uint64_t value, char* buffer, int base){
    if (base < 2 || base > 32) {
        return buffer;
    }
    int i = 0;
    while (value){
        int r = value % base;
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
        value = value / base;
    }
    if (i == 0) {
        buffer[i++] = '0';
    }
    buffer[i] = '\0';
    return reverse(buffer, 0, i - 1);
}
void* memcpy(void* dest, const void *src, size_t len){
    char* d = dest;
    const char *s = src;
    while(len--)
        *d++ = *s++;
    return dest;
}
size_t strlen(const char *str){
    size_t i = 0;
    while(str[i++]);
    return i-1;
}
char* strcpy(char* dest, const char *src){
    return memcpy(dest, src, strlen(src)+1);
}
char* strcat(char *dest, const char *src){
    strcpy(dest + strlen(dest), src);
    return dest;
}
int strcmp(const char* s1, const char* s2){
    while(*s1 && (*s1 == *s2)){
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
uint64_t max(uint64_t a, uint64_t b){
    return a>b?a:b;
}
uint64_t min(uint64_t a, uint64_t b){
    return a<b?a:b;
}