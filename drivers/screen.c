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
    int i = 0;
    while(message[i] != 0){
        print_char(message[i], i, 0, 0x0f);
        i++;
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
