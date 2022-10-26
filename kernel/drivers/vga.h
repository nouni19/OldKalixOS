#include <stdint.h>
#pragma once
#define noback 0x123456fd
int cursorX;
int cursorY;
char defAtt;
uint32_t* fb_addr;
uint16_t fb_width;
uint16_t fb_height;
uint16_t fb_pitch;
uint16_t Xmax;
uint16_t Ymax;
void pixel(int x, int y, uint32_t color);
void rectangle(int x1, int y1, int x2, int y2, uint32_t color);
void line(int x1, int y1, int x2, int y2, uint32_t color);
void printc(int x, int y, char ch, uint32_t att, uint32_t back);
void prints(int x, int y, char str[], uint32_t att, uint32_t back);
void printsl(int x, int y, char str[], uint32_t att, uint32_t back, uint32_t len);
void clear(uint32_t att);
