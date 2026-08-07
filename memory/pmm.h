#ifndef PMM_H
#define PMM_H

#include "cpu/types.h"

#define PAGE_SIZE     4096          /* 4 KiB */
#define TOTAL_MEMORY  0x1000000     /* 16 MiB */
#define TOTAL_FRAMES  (TOTAL_MEMORY / PAGE_SIZE)

#define STACK_TOP     0x90000
#define STACK_SIZE    0x10000       /* 64 KiB */
#define STACK_BOTTOM  (STACK_TOP - STACK_SIZE)

#define FRAME_ALLOC_FAIL 0xFFFFFFFF

void init_pmm(void);
u32  alloc_frame(void);
void free_frame(u32 addr);

u8   test_frame(u32 index);
void clear_frame(u32 index);

u32  pmm_used_frames(void);
u32  pmm_total_frames(void);

#endif