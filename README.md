# OS-From-Scratch
This is my attempt on making an OS. Practicing how to document correctly too. I have little experience in making an OS, i will learn it as i go. this repository will heavily reference OSdev wiki, The Little Book About OS Development, and this Repository https://github.com/cfenollosa/os-tutorial/blob/master/00-environment/README.md
# OS-From-Scratch

A 32-bit x86 operating system written from nothing — no libc, no kernel framework, no runtime. Just a bootloader, a cross-compiler, and freestanding C.

**Stack:** C (freestanding) · x86 Assembly (NASM) · i686-elf cross-toolchain · QEMU

---

## Why this project

Most software runs on top of an operating system. This one *is* the operating system — there is no layer beneath it except the CPU.

That constraint removes every convenience you normally take for granted. There is no `printf`, because nothing has taught the machine what a screen is. There is no `malloc`, because nothing has decided which bytes of RAM are free. There is no `strlen`, no stack setup, no crash handler. Every one of those had to be written before it could be used.

The goal is to reach a usable shell with filesystem commands (`ls`, `cat`, `cd`, `pwd`) — building each layer required to make that possible, in order.

---

## What works

### Boot and CPU setup
- **Bootloader** — loads the kernel off disk in 16-bit real mode and hands control over
- **Protected mode switch** — 16-bit real mode → 32-bit protected mode, the transition that unlocks the full address space and memory protection
- **GDT (Global Descriptor Table)** — defines the flat memory segmentation model the kernel runs under
- **IDT / ISR / IRQ** — full interrupt handling: CPU exception vectors, hardware interrupt routing, and PIC remapping so hardware IRQs don't collide with CPU exception numbers

Interrupt setup is where a lot of hobby kernels quietly break. Exceptions 8, 10–14, and 17 push an error code onto the stack; the rest don't. Getting the stack frame wrong by four bytes corrupts every register the handler restores, and the failure shows up somewhere unrelated later. Each vector's stub had to account for that individually.

### Drivers
- **VGA text driver** — direct writes to video memory at `0xB8000`, cursor control via port I/O, and scrollback
- **PS/2 keyboard driver** — scancode-to-ASCII translation, interrupt-driven input, bounded input buffer

### Memory
- **Physical memory manager (PMM)** — bitmap allocator tracking free and used physical frames, with bounds checking, double-free detection, and allocation counters
- Verified live via a `memtest` shell command that allocates, frees, and re-allocates frames while checking for aliasing and leaks

### Shell
- Command dispatch with `argv`-style tokenization — a small parser that splits an input line into an argument vector, the same model a real shell uses, rather than string-comparing the entire line
- Commands: `help`, `clear`, `reboot`, `meminfo`, `memtest`

### Kernel utility library
A minimal libc replacement written from scratch, because none exists in a freestanding target: `strcmp`, `strchr`, `strncpy_safe`, integer-to-string conversion, and the memory routines the kernel needs.

---

## Build and run

**Requirements:** `i686-elf-gcc`, `i686-elf-binutils`, `nasm`, `qemu-system-i386`, `make`

The cross-compiler matters. The host `gcc` targets Linux and assumes a userspace, a libc, and an ELF loader — none of which exist here. `i686-elf-gcc` is a compiler that targets bare metal and nothing else.

```bash
make            # build kernel + disk image
make run        # boot in QEMU
make clean      # remove build artifacts
```

Debugging with GDB attached to QEMU:

```bash
make debug      # <!-- confirm this target exists in your Makefile -->
```

---

## Roadmap

**Next: filesystem layer.** Reaching `ls` and `cat` requires more than the commands themselves:

1. **Tar ramdisk** — a filesystem image loaded into memory at boot. Tar is chosen because the format is simple enough to parse without a disk driver: fixed 512-byte headers, octal-encoded fields, no allocation tables.
2. **VFS layer** — an abstraction between shell commands and the underlying filesystem, so `cat` doesn't need to know whether it's reading from a ramdisk or a real disk.
3. **`ls`, `cat`, `cd`, `pwd`** — implemented against the VFS.

**Later:**
- E820 memory map parsing (currently the PMM assumes a fixed memory size rather than querying the BIOS for the real layout)
- Paging and virtual memory
- Kernel heap (`kmalloc`) on top of the PMM
- Timer-driven preemptive multitasking

---

## Known limitations

Stated plainly, because they're real:

- Memory size is hardcoded rather than read from the BIOS E820 map
- No paging — the kernel runs on flat physical addresses with no memory protection between it and anything else
- No userspace, no privilege separation; everything runs in ring 0
- No filesystem yet
- Single-tasking

---

## Notes

Written on Arch Linux with the `i686-elf` cross-toolchain, tested in QEMU. Targets i386/i686 — 32-bit deliberately, since the protected-mode transition and segmentation model are the parts worth learning, and long mode hides both.

<!-- Add a screenshot of the shell running in QEMU here — it's the single highest-impact thing you can add to this README. -->
