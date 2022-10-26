#include "stdint.h"
void* ramdisk;
uint32_t ramdiskblocks;
void rdiskinit(uint32_t blocknum);
void rdiskdeinit();
uint32_t rdiskread(void* buf, uint32_t start, uint32_t count);
uint32_t rdiskwrite(void* data, uint32_t start, uint32_t count);