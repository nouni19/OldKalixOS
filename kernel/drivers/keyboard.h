#pragma once
#include <stdbool.h>
#include <stdint.h>
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_MAX_ASCII 83
#define KEY_ESC 0xFF
#define KEY_BCK 0xFE
#define KEY_ENT 0xFD
#define NO_KEY 0xFB
bool isShift;
bool isCtrl;
bool isAlt;
bool isLock;
unsigned char toAscii(unsigned char sc);
char keyboard_int();