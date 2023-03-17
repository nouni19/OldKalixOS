#include "drivers/interrupts.h"
#include "drivers/keyboard.h"
#include "drivers/io.h"
#include "drivers/vga.h"
#include "drivers/mouse.h"
#include "drivers/poweroff.h"
#include "drivers/gdt.h"
#include "tasks/tasks.h"
#include "mem/pmm_mem.h"
#include "mem/vmm_mem.h"
#include "misc/utils.h"
#include "disk/ramdisk.h"
#include "disk/fat32.h"
#include "stivale2.h"
#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
static unsigned char stack[8192];
int xtemp = 0;
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
	.entry_point = 0,
	.stack = (uintptr_t)stack + sizeof(stack),
	.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
	.tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id){
	struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
	for (;;){
		if (current_tag == NULL){
			return NULL;
		}
		if (current_tag -> identifier == id){
			return current_tag;
		}
		current_tag = (void *)current_tag->next;
	}
}


// MAIN KERNEL SECTION


uint32_t xytest = 0;
void keyboard_int(unsigned char c){ //gets called when valid key pressed (c)
	if(c != KEY_BCK){
		printc(xytest, 0, c, 0xff00ff, 0);
		xytest++;
	} else if(xytest > 0){
		xytest--;
		printc(xytest, 0, ' ', 0xff00ff, 0);
	}
}
void mouse_int(MouseState state){ //gets called when mouse updated (state)
	if (state.leftb)
		pixel(state.x, state.y, 0xffffff);
}
void _start(struct stivale2_struct *stivale2_struct){
	settimer(10000); //init timer
	//init memory
	struct stivale2_struct_tag_memmap *memmap_ptr = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	struct stivale2_struct_tag_hhdm *hhdm_stptr = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_HHDM_ID);
	hhdm = hhdm_stptr->addr;
	pmm_init(memmap_ptr);
	vmm_init();
	//init modules
	struct stivale2_struct_tag_modules *modules = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);
	//init graphics
	struct stivale2_struct_tag_framebuffer *fb = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	if (fb == NULL){for (;;){asm("hlt");}}
	fb_addr = (uint32_t*)fb->framebuffer_addr;fb_width = fb->framebuffer_width;fb_height = fb->framebuffer_height;fb_pitch = fb->framebuffer_pitch;
	Xmax = fb_width / 8;
	Ymax = fb_height / 16;
	install_idt(); //init IDT
	init_mouse(); //init mouse
	prints(0, 0, "Booted kernel successfully!", 0x00ff00, 0);
	//uint8_t elfresult = run_elf(modules->modules[1].begin, modules->modules[1].end-modules->modules[1].begin, 4096);
	//char buf[32];
	//uitoa(elfresult, buf, 16);
	//prints(0, 2, buf, 0xff00ff, 0);
	//printsl(0, 3, modules->modules[1].begin+1, 0xff00ff, 0, 3);
	for(;;){asm("hlt");}
}
