#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef struct {
	bool leftb, middleb, rightb;
	int x, y;
} MouseState;
typedef struct {
	uint8_t flags;
	uint8_t x_mov;
	uint8_t y_mov;
} MousePacket;
void init_mouse();
bool _mouse_int();
MouseState get_mouse_state();
