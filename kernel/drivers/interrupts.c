#include "interrupts.h"
#include "vga.h"
#include "../kernel.h"
#include "io.h"
#include "keyboard.h"
#include "../tasks/tasks.h"
#include "../misc/utils.h"
#include "../mem/pmm_mem.h"
#include "../mem/vmm_mem.h"
#include "poweroff.h"
bool panicked = false;
void pic_ack(unsigned int interrupt){
	if(interrupt >= 40) write_port(PIC_2, 0x20);
	write_port(PIC_1, 0x20);
}

void pic_remap(int offset1, int offset2){
	write_port(PIC_1, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	write_port(PIC_2, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	write_port(PIC_1_DATA, offset1);
	write_port(PIC_2_DATA, offset2);
	write_port(PIC_1_DATA, 4);
	write_port(PIC_2_DATA, 2);
	write_port(PIC_1_DATA, PIC_ICW4_8086);
	write_port(PIC_2_DATA, PIC_ICW4_8086);

	write_port(PIC_1_DATA, 0b00000000);
	write_port(PIC_2_DATA, 0b00000000);
	asm("sti");		
}

#define INT_DESC_COUNT 256

struct IDTDescriptor idt_descriptors[INT_DESC_COUNT];
struct IDT idt;

void init_int_desc(int index, uint64_t address){
	idt_descriptors[index].offset_high = (address >> 32) & 0xFFFFFFFF;
	idt_descriptors[index].offset_mid = (address >> 16) & 0xFFFF;
	idt_descriptors[index].offset_low = (address & 0xFFFF);
	idt_descriptors[index].segment_selector = 0x28;
	idt_descriptors[index].reserved = 0x00;
	idt_descriptors[index].type = 0x8E;
	idt_descriptors[index].ist = 0;
}
void init_uint_desc(int index, uint64_t address){
	idt_descriptors[index].offset_high = (address >> 32) & 0xFFFFFFFF;
	idt_descriptors[index].offset_mid = (address >> 16) & 0xFFFF;
	idt_descriptors[index].offset_low = (address & 0xFFFF);
	idt_descriptors[index].segment_selector = 0x28;
	idt_descriptors[index].reserved = 0x00;
	idt_descriptors[index].type = 0xEE;
	idt_descriptors[index].ist = 0;
}

void install_idt(){
	init_int_desc(32, (uint64_t) interrupt_handler_32); // interrupt timer
	init_int_desc(33, (uint64_t) interrupt_handler_33); // keyboard key pressed
	init_int_desc(44, (uint64_t) interrupt_handler_44); // mouse
	init_int_desc(41, (uint64_t) interrupt_handler_44); // usb mouse
	init_int_desc(0, (uint64_t)interrupt_handler_0); // division error
	init_int_desc(4, (uint64_t) interrupt_handler_4); // overflow
	init_int_desc(6, (uint64_t) interrupt_handler_6); // undefined opcode
	init_int_desc(8, (uint64_t) interrupt_handler_8); // double fault
	init_int_desc(14, (uint64_t) interrupt_handler_14); // page fault
	init_uint_desc(128, (uint64_t) interrupt_handler_128); // system call
	idt.address = (uint64_t) &idt_descriptors;
	idt.size = sizeof(struct IDTDescriptor) * INT_DESC_COUNT;
	asm volatile("lidt %0" : : "m"(idt));
	pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);
}

void except(char str[]){
	prints(0, 0, "KERNEL PANIC - PRESS ANY KEY TO RESTART - DEBUG INFO:", 0xff0000, 0xff);
	prints(0, 1, str, 0xff0000, 0xff);
	panicked = true;
	processnum = 0;
	for(;;){asm("hlt");}
}

bool mouseDone = false;
MouseState mouse_state;
void interrupt_handler(uint32_t interrupt, uint32_t errorcode){
	char errorcodeSTR[32];
	uitoa(errorcode, errorcodeSTR, 16);
	char keyboardc;
	switch (interrupt){
		case 0:
			except("DIVIDE ERROR");
			break;
		case 4:
			prints(0, 2, errorcodeSTR, 0xff0000, 0xff);
			except("OVERFLOW");
			break;
		case 6:
			except("UNDEFINED OPCODE");
			break;
		case 8:
			except("DOUBLE FAULT");
			break;
		case 13:
			prints(0, 2, errorcodeSTR, 0xff0000, 0xff);
			except("GE.PR. FAULT");
			break;
		case 14:
			prints(0, 2, errorcodeSTR, 0xff0000, 0xff);
			prints(0, 3, uitoa(read_cr2(), hhdm, 16), 0xff0000, 0xff);
			except("PAGE FAULT");
			break;
		case 33: // keyboard
			if (panicked){
				reboot();
			}
			keyboardc = _keyboard_int();
			if(keyboardc && (keyboardc < 0x7f))
				keyboard_int(keyboardc);
			break;
		case 44: // mouse
			mouseDone = _mouse_int();
			if(mouseDone)
				mouse_int(get_mouse_state());
			break;
		default:
			except("UNKNOWN INTERRUPT");
			break;
	}
	pic_ack(interrupt);
}
void syscall_handler(uint64_t type, uint64_t v1, uint64_t v2){
	if(type == 0){
		printc(0, v1, (uint8_t)v2, 0xffffff, 0);
	}
}