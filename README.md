# KalixOS

x86_64 kernel made using C and assembly, and Limine bootloader

## Features

* [x] Booting using Limine
* [x] Draw and print text on screen
* [x] Interrupts
* [x] PS/2 keyboard and mouse
* [x] Physical memory manager
* [x] Virtual memory manager
* [x] ELF loader
* [ ] Preemptive multitasker
* [ ] FAT32 filesystem

## Requirements

* makefile
* gcc
* ld
* nasm

## Building

`cd kernel` then `make` to build, and `cd kernel` then `make qemu` or `make qemudebug` to run

## Note

This contains a lot of testing code (random prints), copied code, and unfunctional code
