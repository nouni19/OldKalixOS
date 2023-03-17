#include "pmm_mem.h"
#include "../drivers/vga.h"
#include "stdbool.h"
uint64_t _kernel_start;
uint64_t _kernel_end;
void* slabH = 0;
void *memset (void *dest, uint8_t val, size_t len){
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

void pmm_ureg(uint32_t block, uint32_t blocks){ //use region, block 0indexed
	for(uint32_t i=0;i<blocks;i++) pmm_map[(block+i)/8] |= 1<<(block % 8);
}
void pmm_unreg(uint32_t block, uint32_t blocks){ //unuse region, block 0indexed
    for(uint32_t i=0;i<blocks;i++) pmm_map[(block+i)/8] &= ~(1<<(block%8));
}
void* pmalloc(uint32_t blocks){
	uint32_t freebl = 0;
	uint64_t p = 0;
	for(uint32_t i = 0; i<pmm_map_size*8; i++){
		if(PMM_REGU(i)){
			freebl = 0;
			p = 0;
		} else{
			if(freebl == 0){
				p = i;
			}
			freebl++;
			if(freebl == blocks){
				pmm_ureg(p,freebl);
				return (void*)(p<<12);
			}
		}
	}
	return 0;
}
void pfree(uint64_t p, uint32_t blocks){
	pmm_unreg(TOBLOCKS(p), blocks);
}
void pmm_init(struct stivale2_struct_tag_memmap *stmap){
	uint64_t highestb = stmap->memmap[stmap->entries].base + stmap->memmap[stmap->entries].length;
	for(uint64_t i=0; i<stmap->entries; i++){
		uint64_t currentb = stmap->memmap[i].base + stmap->memmap[i].length;
		if(highestb < currentb){
			highestb = currentb;
		}
	}
	ramb = highestb;
	uint32_t bneeded = (TOBLOCKS(highestb))/8;
	for(uint64_t i=0; i<stmap->entries; i++){
		if((stmap->memmap[i].type == 1) && (stmap->memmap[i].length >= bneeded)){ 
			pmm_map = (uint8_t*)(stmap->memmap[i].base);
			pmm_map += hhdm;
			break;
		}
	}
	for(uint32_t i=0; i<bneeded; i++){
		pmm_map[i] = 0xFF;
	}
	for(uint64_t i=0; i<stmap->entries; i++){
		if((stmap->memmap[i].type == 1) && ((stmap->memmap[i].base > ((uint64_t)pmm_map)+bneeded) || ((stmap->memmap[i].base+stmap->memmap[i].length) < ((uint64_t)pmm_map)))){
			pmm_unreg(TOBLOCKS(stmap->memmap[i].base), TOBLOCKS(stmap->memmap[i].length));
		}
	}
	pmm_map_size = bneeded;
}
void* spmalloc(){
	void* returned = 0;
	if(!slabH){
		slabH = pmalloc(1);
		((uint64_t*)slabH)[0] = 62;
		slabH += 8;
		returned = slabH;
		slabH += 64;
		for(uint32_t i = 0; i<61; i++){
			((uint64_t*)(slabH+i*64))[0]=(((uint64_t)slabH)+(i+1)*64);
		}
		((uint64_t*)(slabH+61*64))[0] = 0;
		return returned;
	}
	((uint64_t*)ALIGN((uint64_t)slabH))[0]--;
	returned = slabH;
	slabH = (void*)(((uint64_t*)slabH)[0]);
	return returned;
}
void spfree(void* addr){
	((uint64_t*)ALIGN((uint64_t)addr))[0]++;
	if(((uint64_t*)ALIGN((uint64_t)addr))[0] == 63){
		pfree(ALIGN((uint64_t)addr), 1);
		slabH = 0;
		return;
	}
	((uint64_t*)addr)[0] = (uint64_t)slabH;
	slabH = addr;
}
//memdebug section
uint8_t binToAscii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
char* pagecontent(uint8_t* addr, char* buffer){
	char* ori_buffer = buffer;
	//addr = (uint8_t*)((uint64_t)addr & ~(1 << 12));
	for(uint32_t i = 0; i<4096; i++){
		if(!(i % 8) && (i %16)){
			*buffer = '|';
			buffer++;
			*buffer = ' ';
			buffer++;
		}
		if(!(i % 16)){
			*buffer = '\n';
			buffer++;
			for(uint32_t j = 15; j>0; j--){
				*buffer = binToAscii[(((uint64_t)addr + i) >> (j*4)) & 0xF];
				buffer++;
			}
			*buffer = '0';
			buffer++;
			*buffer = ' ';
			buffer++;
		}
		*buffer = binToAscii[addr[i] >> 4];
		buffer++;
		*buffer = binToAscii[addr[i] & 0xF];
		buffer++;
		*buffer = ' ';
		buffer++;
	}
	*buffer = '\0';
	return ori_buffer;
}

void showpage(void* addr){
	char* buffer = pmalloc(100);
	uint32_t block = TOBLOCKS((uint64_t)addr);//				|
	if(ALIGN((uint64_t)addr) == addr){ //this double faults???? v
		prints(30, 1, PMM_REGU((uint64_t)addr)?"MEMDEBUG:BLOCK USED":"MEMDEBUG:BLOCK FREE", PMM_REGU((uint64_t)addr)?0xFF0000:0x00FF00, 0);
	} else{
		prints(30, 1, "MEMDEBUG:ADDR UNALIGNED", 0x40DCFF, 0);
	}
	pagecontent((uint8_t*)addr, buffer);
	prints(0, 1, buffer, 0x40DCFF, 0x0);
	pfree(buffer, 4);
}
