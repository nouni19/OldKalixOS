#include "stdint.h"
#include "stdbool.h"
typedef uint64_t u64;
#define BASE_FLAGS 0b00001011
uint64_t* start_pml4;
uint64_t* base_pml4;
uint64_t read_cr3();
void write_cr3(uint64_t cr3);
uint64_t read_cr2();
uint64_t* newpml4();
//void map_block(uint64_t* pml4, void* paddr,  void* vaddr, uint8_t flags);
void map_block(u64 pt, u64 virt, u64 phys, u64 flags);
void vmm_init();