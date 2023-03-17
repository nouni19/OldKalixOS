#include "../drivers/io.h"
#include "../mem/vmm_mem.h"
#include "../mem/pmm_mem.h"
#include "../misc/utils.h"
#include "tasks.h"
uint64_t tticks = 0;
uint64_t ticks = 0;
uint32_t process = 0;
uint32_t processnum = 0;
ProcessState *processes[100]; // array of running processes, storing all registers
uint64_t processescr3[100];
void settimer(int hz){ // change timer frequency
	int divisor = 1193180 / hz;
	write_port(0x43, 0x36);
	write_port(0x40, divisor & 0xFF);
	write_port(0x40, divisor >> 8);
}

uint64_t clock_handler(ProcessState *stack){ // clock interrupt handler
	tticks++;
	if(!(tticks % 10))
		ticks++;
	if(processnum == 0) return stack;
	printc(0,8,'J',0xffffff,0);
	if(processnum == process)
		process = 1;
	else
		process++;
	write_cr3(processescr3[process]);
	return ((uint64_t)processes[process])+hhdm;
}

uint8_t run_elf(void* data, uint32_t bytes, uint32_t stackbytes){ // runs elf: stackbytes 4096 aligned
	if(bytes <= 120)
		return ELF_WRONG_SIZE;
	if(((ELFHeader*)data)->magic[0] != 0x7F)
		return ELF_WRONG_MAGIC;
	if(((ELFHeader*)data)->inset != 0x3E)
		return ELF_WRONG_ARCH;
	if(((ELFHeader*)data)->bits != 2)
		return ELF_WRONG_BITTAGE;
	if(((ELFHeader*)data)->endianness != 1)
		return ELF_WRONG_ENDIANNESS;
	if(((ELFHeader*)data)->type != 2)
		return ELF_NOT_EXECUTABLE;
	ELFPEntry* pt = (ELFPEntry*)(((uint64_t)data) + ((ELFHeader*)data)->proghpos);
	ProcessState state;
	uint64_t cr3 = newpml4();
	char buf2[32];
	uitoa(cr3,buf2,16);
	prints(0,15,buf2,0xff00ff,0);
	state.rip = ((ELFHeader*)data)->entrypos;
	void *stackregion = pmalloc(stackbytes / 4096);
	state.rbp = 0x8000000;
	state.rsp = 0x8000000 - sizeof(ProcessState);
	char buf[32];
	uitoa(state.rsp,buf,16);
	prints(0,7,buf,0xffff,0);
	for(uint32_t i=0; i<(stackbytes/4096); i++){
		uitoa(0x7FFF000-(4096*i), buf,16);
		prints(0,8,buf,0xffff,0);
		uitoa(stackregion+(i*4096),buf, 16);
		prints(0,9,buf,0xffff,0);
		map_block(cr3, 0x7FFF000-(4096*i), stackregion+(i*4096), 7);
	}
	for(uint32_t i=0; i<((ELFHeader*)data)->proghentrynum; i++){
		if((pt[i].type == 1) || (pt[i].type == 3)){
			uint8_t flags = 5+(((pt[i].flags>>1)%2)*2);
			if(pt[i].memsz && pt[i].filesz) {
				printc(0,6,'D',0xffff,0);
				//uitoa(((pt[i].memsz/4096)+((pt[i].memsz%4096)?1:0)), buf, 16);
				//prints(0,7,buf,0xffff,0);
				for(uint32_t j=0; j<((pt[i].memsz/4096)+((pt[i].memsz%4096)?1:0)); j++)
					map_block(cr3, pt[i].vaddr+(4096*j)-(4096-(pt[i].vaddr%4096)), data+pt[i].paddr+(4096*j), flags);
				printc(0,6,'E',0xffff,0);
				if(pt[i].memsz > pt[i].filesz){
					map_block(cr3, pt[i].vaddr+pt[i].filesz, pmalloc((pt[i].memsz - pt[i].filesz)/4096+(((pt[i].memsz-pt[i].filesz)%4096)?1:0)), flags);
				}
			}
		}
	}
	memcpy(stackregion+stackbytes-sizeof(ProcessState)+hhdm, ((uint64_t)&state), sizeof(ProcessState));
	write_cr3(cr3);
	void (*foo)(void) = (void (*)())((ELFHeader*)data)->entrypos;
	printc(0,0,'Y',0x00ff00,0);
	foo();
	//processnum++;
	//processes[processnum] = 0x8000000 - sizeof(ProcessState);
	//processescr3[processnum] = cr3;
	return 0;
}