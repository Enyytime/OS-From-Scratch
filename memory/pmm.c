#include "pmm.h"
#include "../kernel/util.h"

static u8 frame_bitmap[TOTAL_FRAMES / 8];
extern u32 kernel_end;

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
    return frame_bitmap[index / 8] & (1 << (index % 8));
}

void clear_frame(u32 index) {
    frame_bitmap[index / 8] &= ~(1 << (index % 8));
}

void init_pmm(void) {
    memory_set(frame_bitmap, 0, sizeof(frame_bitmap)); // set all memories into 0

    u32 kernel_frames = (u32)&kernel_end / PAGE_SIZE;

    // mark the kernel frames, so it won't get overwritten
    for(u32 i = 0; i < kernel_frames; i++) {
        set_frame(i);
    }

    set_frame(0x90000 / PAGE_SIZE); // stack
    set_frame(0xB8000 / PAGE_SIZE); // VGA
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
