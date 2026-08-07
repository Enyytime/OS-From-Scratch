#include "pmm.h"
#include "../kernel/util.h"

static u8 frame_bitmap[TOTAL_FRAMES / 8];
extern u32 kernel_end;
static u32 used_frames = 0;


/** 
 * @brief to mark the frame that this frame is used and do NOT write memory
 * in those frames
 * 
 * @param index the frame index on which the memory will be mark as do NOT write there
 *
 */
static void set_frame(u32 index) {
    // just like in CSSE2310 if let's say frame 11 is being used
    // that means that it's in byte 1, byte 0 is from 0-8 so 11 is in byte 1
    // and then it's on the 3rd bit, since the offset is 3, 11-8 = 3.
    if (index >= TOTAL_FRAMES) return;
    if (!test_frame(index)) used_frames++;
    frame_bitmap[index / 8] |= (1 << (index % 8));
}

u8 test_frame(u32 index) {
    /*
      1 << 3        = 00001000    (a "mask" with only bit 3 set)

        00001010    (the byte)
      & 00001000    (the mask)
        ----------
        00001000    -> non-zero, so frame 11 is USED

        If bit 3 was 0 instead:
        00000010
      & 00001000
        ----------
        00000000    -> zero, so frame 11 is FREE
    */
    if (index >= TOTAL_FRAMES) return 1;   /* out of range == not allocatable */
    return frame_bitmap[index / 8] & (1 << (index % 8));
}

void clear_frame(u32 index) {
    if (index >= TOTAL_FRAMES) return;
    if (test_frame(index)) used_frames--;
    frame_bitmap[index / 8] &= ~(1 << (index % 8));
}

void init_pmm(void) {
    memory_set(frame_bitmap, 0, sizeof(frame_bitmap)); // set all memories into 0
    used_frames = 0;
    u32 kernel_frames = (u32)&kernel_end / PAGE_SIZE;

    // mark the kernel frames, so it won't get overwritten
    for(u32 i = 0; i < kernel_frames; i++) {
        set_frame(i);
    }

    /* Stack: grows DOWN from STACK_TOP, so reserve [STACK_BOTTOM, STACK_TOP) */
    for (u32 addr = STACK_BOTTOM; addr < STACK_TOP; addr += PAGE_SIZE) {
        set_frame(addr / PAGE_SIZE);
    }

    /* Video memory, BIOS ROM and MMIO: 0xA0000 - 0xFFFFF (fixes point 4) */
    for (u32 addr = 0xA0000; addr < 0x100000; addr += PAGE_SIZE) {
        set_frame(addr / PAGE_SIZE);
    }
}

u32 alloc_frame(void) {
    for(u32 i = 0; i < TOTAL_FRAMES; i++) {
        u8 isEmpty = test_frame(i);
        if(isEmpty == 0) {
            set_frame(i);
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

void free_frame(u32 addr) {
    clear_frame(addr / PAGE_SIZE);
}

u32 pmm_used_frames(void)  { return used_frames; }
u32 pmm_total_frames(void) { return TOTAL_FRAMES; }