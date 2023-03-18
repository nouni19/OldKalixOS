#include "vmm_mem.h"
#include "pmm_mem.h"
#include "../misc/utils.h"
#include "../drivers/interrupts.h"
#define IS_PR(val) (val&1)
uint64_t read_cr3() {
  uint64_t cr3;
  asm ("movq %%cr3, %0" :"=r"(cr3));
  return cr3;
}
uint64_t read_cr2() {
	uint64_t cr2;
	asm ("movq %%cr2, %0" :"=r"(cr2));
	return cr2;
}
void write_cr3(uint64_t cr3) {
	if(cr3 == ALIGN(cr3))
		asm ("movq %0, %%cr3" ::"r"(cr3));
	else{
		except("Set cr3 to unaligned value");
	}
}

uint64_t* newpml4(){
	uint64_t* pml4 = pmalloc(1);
	memset(((void*) pml4)+hhdm, 0, 2048);
	memcpy(((void*)pml4)+2048+hhdm, ((void*)start_pml4)+hhdm, 2048);
	return pml4;
}
#define PAG_4K_FLAGS 0x3FFULL
#define PAG_PRESENT 1
#define PAG_WRITABLE 2
#define PAG_USER 4
static uint64_t getidx(uint64_t virt, uint64_t level) { return (virt >> (12 + (level - 1) * 9)) & 0x1ff; }
static uint64_t *get_alloc_next(uint64_t *cur, uint64_t idx) {
    if (!(cur[idx] & PAG_PRESENT)) cur[idx] = ((uint64_t)pmalloc(1)) | PAG_PRESENT | PAG_WRITABLE | PAG_USER;
    return PHYS_TO_VIRT(cur[idx] & ~PAG_4K_FLAGS);
}
void map_block(uint64_t pt, uint64_t virt, uint64_t phys, uint64_t flags) {
	virt &= ~PAG_4K_FLAGS;
	phys &= ~PAG_4K_FLAGS;
    uint64_t *p3 = get_alloc_next(PHYS_TO_VIRT(pt), getidx(virt, 4)), *p2 = get_alloc_next(p3, getidx(virt, 3)), *p1 = get_alloc_next(p2, getidx(virt, 2)); 
    p1[getidx(virt, 1)] = phys | flags | PAG_PRESENT;
}

void vmm_init(){
	base_pml4 = (uint64_t*)read_cr3();
	start_pml4 = base_pml4 + 256;
}
