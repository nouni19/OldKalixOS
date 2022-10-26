#include <stdint.h>
#pragma once
#define ELF_WRONG_MAGIC 0x01
#define ELF_WRONG_SIZE 0x02
#define ELF_WRONG_ARCH 0x03
#define ELF_WRONG_BITTAGE 0x04
#define ELF_WRONG_ENDIANNESS 0x05
#define ELF_NOT_EXECUTABLE 0x06
uint32_t processnum;
typedef struct {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rdi, rsi, rdx, rcx, rbx, cr3, rax, intno, ec, rip, cs, rflags, rsp, ss; 
} ProcessState;
typedef struct {
	uint8_t magic[4];
	uint8_t bits;
	uint8_t endianness;
	uint8_t elfhvers;
	uint8_t abi;
	uint64_t pad;
	uint16_t type;
	uint16_t inset;
	uint32_t elfvers;
	uint64_t entrypos;
	uint64_t proghpos;
	uint64_t secthpos;
	uint32_t flags;
	uint16_t headersize;
	uint16_t proghentrysize;
	uint16_t proghentrynum;
	uint16_t secthentrysize;
	uint16_t secthentrynum;
	uint16_t sectnameind;
} ELFHeader;
typedef struct {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t filesz;
	uint64_t memsz;
	uint64_t align;
} ELFPEntry;
typedef struct {
	uint32_t name;
	uint32_t type;
	uint64_t flags;
	uint64_t addr;
	uint64_t offset;
	uint64_t size;
	uint32_t link;
	uint32_t info;
	uint64_t addralign;
	uint64_t entsize;
} ELFSEntry;
uint64_t ticks;
uint32_t process;
uint32_t processn;
ProcessState *processes[];
void set_timer(int hz);
uint8_t run_elf(void* data, uint32_t bytes, uint32_t stackbytes);