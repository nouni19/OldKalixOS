#include "keyboard.h"
#include "io.h"
bool isShift = false;
bool isCtrl = false;
bool isAlt = false;
bool isLock = false;
const char scancode_to_char[] = {
	'?', KEY_ESC, '1', '2', '3', '4', '5',
	'6', '7', '8', '9', '0', '-', '=',
	KEY_BCK, '?', 'q', 'w', 'e', 'r', 't',
	'y', 'u', 'i', 'o', 'p', '[', ']',
	KEY_ENT, '?', 'a', 's', 'd', 'f', 'g',
	'h', 'j', 'k', 'l', ';', '\'', '`',
	'?', '\\', 'z', 'x', 'c', 'v', 'b',
	'n', 'm', ',', '.', '/', '?', '?',
	'?', ' '
};
const char sscancode_to_char[] = {
	'?', KEY_ESC, '!', '@', '#', '$', '%',
	'^', '&', '*', '(', ')', '_', '+',
	KEY_BCK, '?', 'Q', 'W', 'E', 'R', 'T',
	'Y', 'U', 'I', 'O', 'P', '{', '}',
	KEY_ENT, '?', 'A', 'S', 'D', 'F', 'G',
	'H', 'J', 'K', 'L', ':', '"', '~',
	'?', '|', 'Z', 'X', 'C', 'V', 'B',
	'N', 'M', '<', '>', '?', '?', '?',
	'?', ' '
};
unsigned char toAscii(unsigned char sc){
	if (sc < 0x3A){
		return scancode_to_char[sc];
	} else{
		return NO_KEY;
	}
	
}

char keyboard_int(){
	uint8_t r =  read_port(KEYBOARD_DATA_PORT);
	switch (r){
		case 0x1D:
			isCtrl = true;
			break;
		case 0x9D:
			isCtrl = false;
			return 0;
		case 0x2A:
			isShift = true;
			break;
		case 0xAA:
			isShift = false;
			break;
		case 0x38:
			isAlt = true;
			break;
		case 0xB8:
			isAlt = false;
			break;
		case 0x3A:
			isLock = isLock?false:true;
			break;
		default:
			if (r > 0x3A)
				break;
			return (isShift != isLock)?sscancode_to_char[(uint8_t)r]:scancode_to_char[(uint8_t)r];
	}
	return 0;
}