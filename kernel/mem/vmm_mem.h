#include "stdint.h"
#include "stdbool.h"
#define BASE_FLAGS 0b00001011
#define PHYS_TO_VIRT(addr) ((uint64_t*)(((uint64_t)addr)+hhdm))
uint64_t* start_pml4;
uint64_t* base_pml4;
uint64_t read_cr3();
void write_cr3(uint64_t cr3);
uint64_t read_cr2();
uint64_t* newpml4();
//void map_block(uint64_t* pml4, void* paddr,  void* vaddr, uint8_t flags);
void map_block(uint64_t pt, uint64_t virt, uint64_t phys, uint64_t flags);
void vmm_init();