#include "ramdisk.h"
#include "../drivers/interrupts.h"
#include "../mem/pmm_mem.h"
#include "../misc/utils.h"
void rdiskinit(uint32_t blocknum){
	ramdisk = pmalloc(blocknum/8);
	ramdiskblocks = blocknum;
}
void rdiskdeinit(){
	pfree((uint64_t)ramdisk, ramdiskblocks/8);
	ramdisk = 0;
	ramdiskblocks = 0;
}
uint32_t rdiskread(void* buf, uint32_t start, uint32_t count){
	if((start+count) > ramdiskblocks) except("RAMDISK READ OVER LIMIT");
	memcpy(buf, ramdisk+start*512, 512*count);
	return count;
}
uint32_t rdiskwrite(void* data, uint32_t start, uint32_t count){
	if((start+count) > ramdiskblocks) except("RAMDISK WRITE OVER LIMIT");
	memcpy(ramdisk+start*512, data, 512*count);
	return count;
}