#include "../drivers/io.h"
#include "../mem/vmm_mem.h"
#include "../mem/pmm_mem.h"
#include "../misc/utils.h"
#include "tasks.h"
uint64_t tticks = 0;
uint64_t ticks = 0;
uint32_t process = 0;
uint32_t processnum = 0;
void settimer(int hz){ // change timer frequency
	int divisor = 1193180 / hz;
	write_port(0x43, 0x36);
	write_port(0x40, divisor & 0xFF);
	write_port(0x40, divisor >> 8);
}
uint64_t tempdebug = 0;
uint64_t clock_handler(ProcessState *stack){ // clock interrupt handler
	if(processnum == 1) tempdebug++;
	char buf[32];
	uitoa(tempdebug,buf,16);
	prints(0,15,buf,0xff0000,0);
	tticks++;
	if(!(tticks % 10))
		ticks++;
	if(processnum == 0) return stack;
	if(processnum == process)
		process = 1;
	else
		process++;
	write_cr3(processesinfo[process].cr3);
	return ((uint64_t)processes[process]);
}
LOADELF_RESULT run_elf(void* data, uint32_t bytes, uint32_t stackbytes){ // runs elf: stackbytes 4096 aligned
	if(bytes <= 120)
		return 1;
	if(((ELFHeader*)data)->magic[0] != 0x7F)
		return 2;
	if(((ELFHeader*)data)->inset != 0x3E)
		return 3;
	if(((ELFHeader*)data)->bits != 2)
		return 4;
	if(((ELFHeader*)data)->endianness != 1)
		return 5;
	if(((ELFHeader*)data)->type != 2)
		return 6;
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
	uitoa(pt[0].vaddr,buf,16);
	prints(0,4,buf,0xff0fff,0);
	for(uint32_t i=0; i<((ELFHeader*)data)->proghentrynum; i++){
		if(pt[i].type == 1){
			uint8_t flags = 5+(((pt[i].flags>>1)%2)*2);
			flags = 3;
			if(pt[i].memsz) {
				//uitoa(((pt[i].memsz/4096)+((pt[i].memsz%4096)?1:0)), buf, 16);
				//prints(0,7,buf,0xffff,0);
				uint64_t blocksneeded = ALIGNUP(pt[i].memsz);
				void* physicalarea = pmalloc(blocksneeded);
				for(int j = 0; j<blocksneeded; j++)
					map_block(cr3, pt[i].vaddr+(j<<12), physicalarea + (j<<12), flags);
				memcpy(PHYS_TO_VIRT(physicalarea),data+pt[i].offset,pt[i].filesz);
				memset(physicalarea+pt[i].filesz, 0, pt[i].memsz-pt[i].filesz);
			}
		}
	}
	memcpy(stackregion+stackbytes-sizeof(ProcessState)+hhdm, ((uint64_t)&state), sizeof(ProcessState));
	ProcessInfo info;
	info.cr3 = cr3;
	processes[processnum+1] = state.rsp;
	processesinfo[processnum+1] = info;
	processnum++;
	return 0;
}