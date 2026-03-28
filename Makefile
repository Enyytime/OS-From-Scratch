# 1. Setup variables
# This finds all .c and .h files in your kernel and drivers directories
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
# Converts a list like 'kernel/kernel.c' to 'kernel/kernel.o'
OBJ = ${C_SOURCES:.c=.o}

# 2. Toolchain
CC = i386-elf-gcc
LD = i386-elf-ld
GDB = i386-elf-gdb

# 3. Flags
CFLAGS = -g -ffreestanding -Wall -Wextra

# 4. Primary Build Targets
# This is the main file QEMU will run
os-image.bin: build/boot.bin build/kernel.bin
	cat $^ > $@

# Link the kernel binary
# kernel_entry.o MUST be the first dependency
build/kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^ --oformat binary

# This is the same as kernel.bin but keeps debug symbols for GDB
build/kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -o $@ -Ttext 0x1000 $^

# --- NEW RULE: How to make the boot.bin specifically ---
build/boot.bin: boot/boot.asm
	mkdir -p build
	nasm $< -f bin -o $@

# 5. Execution Rules
run: os-image.bin
	qemu-system-i386 -drive format=raw,file=os-image.bin

debug: os-image.bin build/kernel.elf
	qemu-system-i386 -s -S -drive format=raw,file=os-image.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file build/kernel.elf"

# 6. Generic Compilation Rules
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

# 7. Cleanup
clean:
	rm -rf *.bin *.o os-image.bin
	rm -rf build/*.bin build/*.elf build/*.o
	rm -rf kernel/*.o boot/*.o drivers/*.o
