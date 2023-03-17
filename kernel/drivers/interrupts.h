#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifndef INCLUDE_INTERRUPTS
#define INCLUDE_INTERRUPTS
bool panicked;
void install_idt();
void except(char str[]);
#define PIC_1 0x20		
#define PIC_2 0xA0		
#define PIC_1_DATA 0x21
#define PIC_2_DATA 0xA1

#define PIC_1_OFFSET 0x20
#define PIC_2_OFFSET 0x28
#define PIC_ICW1_ICW4 0x01	
#define PIC_ICW1_INIT 0x10	
#define PIC_ICW4_8086 0x01

void interrupt_handler_32(); // TIMER
void interrupt_handler_33(); // KEYBOARD
void interrupt_handler_44(); // MOUSE
void interrupt_handler_0(); // DIV ERROR
void interrupt_handler_4(); // OVERFLOW0
void interrupt_handler_6(); // UNDEFINED OPCODE
void interrupt_handler_8(); // DOUBLE FAULT
void interrupt_handler_14(); // PAGE FAULT
void interrupt_handler_128(); // SYSCALL HANDLER

struct IDT{
	uint16_t size;
	uint64_t address;
} __attribute__((packed));

struct IDTDescriptor{
	uint16_t offset_low;
	uint16_t segment_selector;
	uint8_t ist;
	uint8_t type;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved;

} __attribute__((packed));

void install_idt();

void interrupt_handler(uint32_t interrupt, uint32_t errorcode);
void except(char str[]);
#endif