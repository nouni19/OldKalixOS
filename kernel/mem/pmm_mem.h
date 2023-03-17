#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../stivale2.h"
#define PMM_REGU(block) (pmm_map[((uint64_t)block)/8] & (1<<(((uint64_t)block)%8))?true:false)
#define ALIGN(addr)((uint64_t)addr & ~((uint64_t)0x3FF))
#define TOBLOCKS(addr)(ALIGN(addr) / 4096)
uint8_t* pmm_map;
uint64_t hhdm;
uint64_t ramb;
uint32_t pmm_map_size;
void *memset (void *dest, uint8_t val, size_t len);
void pmm_init(struct stivale2_struct_tag_memmap *stmap);
void pmm_ureg(uint32_t block, uint32_t blocks);
void pmm_unreg(uint32_t block, uint32_t blocks);
void* pmalloc(uint32_t blocks);
void pfree(uint64_t p, uint32_t blocks);
void* spmalloc();
void spfree(void* addr);
//memdebug section
char* pagecontent(uint8_t* addr, char* buffer);
void showpage(void* addr);