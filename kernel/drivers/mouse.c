#include "io.h"
#include "interrupts.h"
#include "mouse.h"
#include "vga.h"
#include "../tasks/tasks.h"
int32_t x, y = 0;
bool left_pressed, right_pressed, middle_pressed = false;
uint8_t current_byte = 0;
uint8_t packet[4] = {0};
uint8_t bytes_per_packet = 3;

static inline void mouse_wait(int type){
	int timeout = 100000;
	if(type == 0){
		while(timeout--){
			if(read_port(0x64)&(1<<0)){
				return;
			}
		}
	} else{
		while(timeout--){
			if(!(read_port(0x64)&(1<<1)))
				return;
		}
	}
}

static inline void mouse_write(uint8_t val){
	mouse_wait(1);
	write_port(0x64, 0xd4);
	mouse_wait(1);
	write_port(0x60, val);
}
static inline uint8_t mouse_read(){
	mouse_wait(0);
	return read_port(0x60);
}

void init_mouse(){
	x = fb_width / 2;
	y = fb_height / 2;
	mouse_wait(1);
	write_port(0x64, 0xa8);
	mouse_wait(1);
	write_port(0x64, 0x20);
	uint8_t status = mouse_read();
	mouse_read();
	status |= (1<<1);
	status &= ~(1<<5);
	mouse_wait(1);
	write_port(0x64, 0x60);
	mouse_wait(1);
	write_port(0x60, status);
	mouse_read();
	mouse_write(0xff);
	mouse_read();
	mouse_write(0xf6);
	mouse_read();
	mouse_write(0xf4);
	mouse_read();
}

static int handler_cycle = 0;
static MousePacket current_packet;
static int discard_packet = 0;

bool mouse_int(){
	if(ticks < 250){
		read_port(0x60);
		return false;
	}
	switch (handler_cycle){
		case 0:
			current_packet.flags = read_port(0x60);
			handler_cycle++;
			if(current_packet.flags & (1<<6) || current_packet.flags & (1<<7))
				discard_packet = 1;
			if(!(current_packet.flags & (1<<3)))
				discard_packet = 1;
			return false;
		case 1:
			current_packet.x_mov = read_port(0x60);
			handler_cycle++;
			return false;
		case 2:
			current_packet.y_mov = read_port(0x60);
			handler_cycle = 0;
			if(discard_packet){
				discard_packet = 0;
				return false;
			}
			int64_t x_mov, y_mov;
			if(current_packet.flags & (1<<4)){
				x_mov = (int8_t)current_packet.x_mov;
			}else{
				x_mov = current_packet.x_mov;
			}
			if (current_packet.flags & (1<<5)){
				y_mov = (int8_t)current_packet.y_mov;
			} else{
				y_mov = current_packet.y_mov;
			}
			if(x_mov + x > fb_width){ 
				x = fb_width;
			} else if(x_mov + x < 0){
			 	x = 0;
			} else{
				x += x_mov;
			}
			if(y - y_mov > fb_height){
				y = fb_height;
			} else if(y - y_mov < 0){
				y = 0;
			} else{
				y -= y_mov;
			}
			left_pressed = current_packet.flags & 0b1;
			right_pressed = current_packet.flags & 0b10;
			middle_pressed = current_packet.flags & 0b100;
			return true;
	}
	return false;
}

MouseState getMouse(){
	MouseState state;
	state.leftb = left_pressed;
	state.middleb = middle_pressed;
	state.rightb = right_pressed;
	state.x = x;
	state.y = y;
	return state;
}