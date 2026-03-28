#include "ports.h"
#include "screen.h"

void print_char(char character, int col, int row, char attribute){
    if(!attribute){
        attribute = 0x0f; // white on black
    }

    unsigned char* vidmem = (unsigned char*) 0xb8000;

    int offset = (row * 80 + col) * 2;

    vidmem[offset] = character;
    vidmem[offset + 1] = attribute;
}

void kprint(char* message){
    int offset = get_cursor_offset();
    int i = 0;
    while(message[i] != 0){
        if(message[i] == 'n'){  // go to the next row if we meet a new line
            int row = get_offset_row(offset);
            offset = get_offset(0, row + 1);
        } else {
            int row = get_offset_row(offset);
            int col = (offset - get_offset(0, row)) / 2;

            print_char(message[i], col, row, 0x0f);
            offset += 2; 
        }
    }
}

void clear_screen(){
    int screen_size = MAX_COLS * MAX_ROWS;
    char* screen = (char*) 0xb8000;
    
    for(int i = 0; i < screen_size; i++){
        screen[i*2] = ' ';
        screen[i * 2 + 1] = 0x0f; 
    }
}


static int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}
 
static void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));

}

static int get_offset(int col, int row) { 
    return 2 * (row * MAX_COLS + col); 
}

static int get_offset_row(int offset) { 
    return offset / (2 * MAX_COLS); 
}

