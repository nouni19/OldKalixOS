#include "../mem/pmm_mem.h"
void reboot(){
	write_cr3(0);
	printc(0, 0, *((char*)hhdm), 0xffffff, 0);
}