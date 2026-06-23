#ifndef PMM_H
#define PMM_H

#include "cpu/types.h"

#define PAGE_SIZE 4096 // 4 KiB
#define TOTAL_MEMORY 0x1000000 // 16 MiB
#define TOTAL_FRAMES (0x1000000 / 4096)

void init_pmm(void);
u32 alloc_frame(void);
void free_frame(u32 addr);

#endif