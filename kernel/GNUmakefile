# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := Kalix.elf

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
override ASM := nasm

ifeq ($(origin CC), default)
CC := gcc
endif
 
# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
ifeq ($(origin LD), default)
LD := ld
endif
 
# User controllable CFLAGS.
CFLAGS ?= -O2 -g -Wall -Wextra -pipe
 
# User controllable linker flags. We set none by default.
LDFLAGS ?=
 
# Internal C flags that should not be changed by the user.
override INTERNALCFLAGS :=   \
	-I.                  \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD
 
# Internal linker flags that should not be changed by the user.
override INTERNALLDFLAGS :=    \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static
 
# Use find to glob all *.c files in the directory and extract the object names.
override CFILES := $(shell find ./ -type f -name '*.c')
override OBJ := $(CFILES:.c=.o)
override HEADER_DEPS := $(CFILES:.c=.d)
 
# Default target.
.PHONY: all
all: $(KERNEL)
 
# Link rules for the final kernel executable.
$(KERNEL): $(OBJ) kernelasm.o
	$(LD) $(OBJ) kernelasm.o $(LDFLAGS) $(INTERNALLDFLAGS) -o $@
	make clean
	make iso
# Compilation rules for *.c files.
-include $(HEADER_DEPS)
%.o: %.c kernelasm.asm
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@
	$(ASM) kernelasm.asm -f elf64 -o kernelasm.o
# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf *.o
	rm -rf drivers/*.o
	rm -rf mem/*.o
	rm -rf misc/*.o
	rm -rf disk/*.o
	rm -rf tasks/*.o
	rm -rf *.d
	rm -rf drivers/*.d
	rm -rf mem/*.d
	rm -rf misc/*.d
	rm -rf disk/*.d
	rm -rf tasks/*.d
qemu: image.iso
	qemu-system-x86_64 image.iso
qemudebug: image.iso
	qemu-system-x86_64 -d int -no-reboot -no-shutdown image.iso
iso: iso.sh
	bash iso.sh
