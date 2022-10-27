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

void settimer(int hz){ // change timer frequency
	int divisor = 1193180 / hz;
	write_port(0x43, 0x36);
	write_port(0x40, divisor & 0xFF);
	write_port(0x40, divisor >> 8);
}

ProcessState* clock_handler(ProcessState *stack){ // clock interrupt handler
	tticks++;
	if(!(tticks % 10))
		ticks++;
	if(processnum == 0) return stack;
	if(processnum == process)
		process = 1;
	else
		process++;
	return processes[process];
}

uint8_t run_elf(void* data, uint32_t bytes, uint32_t stackbytes){ // runs elf: stackbytes 4096 aligned, argv max 4096 bytes
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
	state.cr3 = newpml4();
	state.rip = ((ELFHeader*)data)->entrypos;
	state.rbp = 0x7FFFFFF;
	state.rsp = 0x7FFFFFF - sizeof(ProcessState);
	void *stackregion = pmalloc(stackbytes / 4096);
	state.rbp = stackregion + stackbytes;
	state.rsp = stackregion + stackbytes - sizeof(ProcessState);
	
	for(uint32_t i=0; i<(stackbytes/4096); i++){
		map_block(state.cr3, stackregion+(i*4096), 0x7FFF000-(4096*i), 7);
	}
	for(uint32_t i=0; i<((ELFHeader*)data)->proghentrynum; i++){
		if((pt[i].type == 1) || (pt[i].type == 3)){
			uint8_t flags = 5+(((pt[i].flags>>1)%2)*2);
			if(pt[i].memsz && pt[i].filesz) {
				for(uint32_t j=0; j<((pt[i].memsz/4096)+((pt[i].memsz%4096)?1:0)); j++)
					map_block(state.cr3+hhdm, data+pt[i].paddr+(4096*j), pt[i].vaddr+(4096*j), flags);
				if(pt[i].memsz > pt[i].filesz){
					map_block(state.cr3+hhdm, pmalloc((pt[i].memsz - pt[i].filesz)/4096+(((pt[i].memsz-pt[i].filesz)%4096)?1:0)), pt[i].vaddr+pt[i].filesz, flags);
				}
			}
		}
	}
	memcpy(stackregion+stackbytes-sizeof(ProcessState), &state, sizeof(ProcessState));
	//processnum++;
	processes[processnum] = stackregion+stackbytes-sizeof(ProcessState);
	return 0;
}