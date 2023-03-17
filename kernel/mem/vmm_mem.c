#include "vmm_mem.h"
#include "pmm_mem.h"
#include "../misc/utils.h"
#include "../drivers/interrupts.h"
#define IS_PR(val) (val&1)
#define PHYS_TO_VIRT(addr) ((uint64_t*)(((uint64_t)addr)+hhdm))
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
	printc(1, 4, 'A', 0xff, 0);
	memset(((void*) pml4)+hhdm, 0, 2048);
	printc(1, 4, 'B', 0xff, 0);
	memcpy(((void*)pml4)+2048+hhdm, ((void*)start_pml4)+hhdm, 2048);
	printc(1, 4, 'C', 0xff, 0);
	return pml4;
}/*
void map_block(uint64_t* pml4, void* paddr,  void* vaddr, uint8_t flags){
	uint64_t paddrint = ((uint64_t)paddr & ~((uint64_t)0x3FF));
	uint64_t vaddrint = ((uint64_t)vaddr & ~((uint64_t)0x3FF));
	uint16_t pml4i = (uint16_t)((vaddrint >> 48) & 0x3FF);
	uint16_t pdpti = (uint16_t)((vaddrint >> 36) & 0x3FF);
	uint16_t pdi = (uint16_t)((vaddrint >> 24) & 0x3FF);
	uint16_t pti = (uint16_t)((vaddrint >> 12) & 0x3FF);
	uint64_t* pdpt;
	uint64_t* pd;
	uint64_t* pt;
	if(!IS_PR(PHYS_TO_VIRT(pml4)[pml4i])){
		pdpt = pmalloc(1);
		PHYS_TO_VIRT(pml4)[pml4i] = flags | ((uint64_t)pdpt);
	} else{
		pdpt = (uint64_t*)(PHYS_TO_VIRT(pml4)[pml4i] & ~((uint64_t)0x3FF));
	}
	char buf[16];
	uitoa(pdpt,buf,16);
	prints(0,9,buf,0xff00ff,0);
	printc(6,5,'N',0xff,0);
	if(!IS_PR(PHYS_TO_VIRT(pdpt)[pdpti])){
		pd = pmalloc(1);
		PHYS_TO_VIRT(pdpt)[pdpti] = flags | ((uint64_t)pd);
	} else{
		pd = (uint64_t*)(PHYS_TO_VIRT(pdpt)[pdpti] & ~((uint64_t)0x3FF));
	}
	printc(6,5,'M',0xff,0);
	if(!IS_PR(PHYS_TO_VIRT(pd)[pdi])){
		pt = pmalloc(1);
		PHYS_TO_VIRT(pd)[pdi] = flags | ((uint64_t)pt);
	} else{
		pt = (uint64_t*)(PHYS_TO_VIRT(pd)[pdi] & ~((uint64_t)0x3FF));
	}
	printc(6,5,'O',0xff,0);
	PHYS_TO_VIRT(pt)[pti] = flags | paddrint;
}*/
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
