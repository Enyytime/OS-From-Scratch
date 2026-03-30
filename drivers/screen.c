/**
 * @file screen.c
 * @brief VGA driver with a non-destructive scrollback buffer.
 * * This driver manages a virtual terminal history in RAM, allowing the user
 * to scroll through previous output while maintaining a 25-row viewport 
 * on the physical VGA hardware.
 */

 
#include "ports.h"
#include "screen.h"
#include "../kernel/util.h"

/* --- Configuration --- */
#define VIDEO_ADDRESS 0xb8000   /**< Start of VGA text mode video memory */
#define MAX_HISTORY 500         /**< Number of rows preserved in RAM history */
#define WHITE_ON_BLACK 0x0f    /**< Default color attribute (Light Grey on Black) */


/* --- Terminal State --- */
/** * @brief The virtual buffer storing all text history.
 * Each row is (80 columns * 2 bytes per char/attr).
 */
static char terminal_history[MAX_HISTORY][MAX_COLS * 2]; 

static int total_lines_written = 0; /**< Total count of rows currently in history */
static int current_view_row = 0;    /**< Index of the history row currently at the top of the screen */
static int cursor_col = 0;          /**< Current horizontal position of the cursor (0 to 79) */

/* --- Private Function Prototypes --- */
static void refresh_screen();
static void print_char(char character, int col, int history_row, char attribute);
static int get_cursor_offset();
static void set_cursor_offset(int offset);
static int get_offset(int col, int row);


/**
 * @brief Synchronizes physical VGA memory with the virtual viewport.
 * 
 * @details Maps a 25-row slice of the history buffer (starting at 
 * current_view_row) to the physical address 0xb8000 using memory_copy.
 * If the history index exceeds total_lines_written, the row is cleared.
 */
static void refresh_screen(){
    for(int i = 0; i < MAX_ROWS; i++){
        // for keeping track of every row below current_view_row
        // if current view row is row 3, and the total row is 5
        // then i starts with 0
        // then history_index[0] = 3 + 0 = row 3 | history_index[1] = 3 + 1 = row 4 | history_index[2] = 3 + 2 = row 5
        int history_index = i + current_view_row;
        
        char* vga_row = (char*)(VIDEO_ADDRESS + (i * MAX_COLS * 2));
        
        // print all the history until it meets the end, after that we print blank
        if(history_index < total_lines_written){
            
            // Copy the stored line to the screen
            memory_copy((char*)(terminal_history[history_index]), vga_row, MAX_COLS * 2);

        } else {
            // Fill unused screen rows with blanks
            for (int j = 0; j < MAX_COLS * 2; j += 2) {
                vga_row[j] = ' ';
                vga_row[j + 1] = WHITE_ON_BLACK;
            }
        }
    }
}

/**
 * @brief Scrolls the viewport one row up.
 * @details Decrements current_view_row and triggers refresh_screen().
 */
void scroll_up(){
    if(current_view_row > 0){
        current_view_row--;
        refresh_screen();
    }
}

/**
 * @brief Scrolls the viewport one row down.
 * @details Increments current_view_row and triggers refresh_screen().
 */
void scroll_down(){
    if(current_view_row + MAX_ROWS < total_lines_written){
        current_view_row++;
        refresh_screen();
    }
}

/**
 * @brief Main printing function for the kernel.
 * @param message Null-terminated string to be printed.
 * @details Processes the string character by character. Handles '\n', 
 * automatic line wrapping, and ensures the viewport follows new text 
 * by updating current_view_row.
 */
void kprint(char* message){
    // initialize if this is the first line ever written
    if (total_lines_written == 0){
        total_lines_written = 1;
    }

    for(int i = 0; message[i] != 0; i++){
        // enter a newline if we see a new line character, or if the character we typed
        // exceed the max column
        if(message[i] == '\n' || cursor_col >= MAX_COLS){
            total_lines_written++;
            cursor_col = 0;

            // Prevent buffer overflow (wrap back to 0 or implement circular logic)
            if (total_lines_written >= MAX_HISTORY) {
                total_lines_written = MAX_HISTORY - 1; 
                // Simple fix: stop at the end. Elite fix: memmove the whole buffer up.
            }
            
            // if we exceed the total number of rows
            if(total_lines_written >= MAX_ROWS){
                // move the line down by 1 line
                current_view_row = total_lines_written - MAX_ROWS;
            }
        } else {
            print_char(message[i], cursor_col, total_lines_written - 1, WHITE_ON_BLACK);
            cursor_col++;
        }
    }
    
    // update the screen every clock
    refresh_screen();

    // don't forget to update the cursor location
    // let's say the total_lines_written is 29 and current_view_row is at row 5
    // total row shown in the screen is 25
    // then the row of our cursor at the bottom will be 29 - 4
    // location of the cursor will be at row 25 of the screen (bottom)
    int cursor_row = (total_lines_written - 1) - current_view_row;
    set_cursor_offset(get_offset(cursor_col, cursor_row));

}

void kprint_backspace() {
    // 1. Don't delete if we are at the very start of a line
    if (cursor_col > 0) {
        cursor_col--;
        
        // 2. Wipe the character in our RAM history
        int current_line_idx = total_lines_written - 1;
        terminal_history[current_line_idx][cursor_col * 2] = ' ';
        terminal_history[current_line_idx][cursor_col * 2 + 1] = WHITE_ON_BLACK;

        // 3. Redraw the screen so the user sees the change
        refresh_screen();

        // 4. Move the hardware blinking cursor back
        int cursor_row = (total_lines_written - 1) - current_view_row;
        set_cursor_offset(get_offset(cursor_col, cursor_row));
    }
}

/**
 * @brief Internal helper to write a character to the RAM buffer.
 * @param character ASCII character to store.
 * @param col Column position (0-79).
 * @param history_row The absolute line index in terminal_history.
 * @param attribute Color and font attributes for VGA.
 */
static void print_char(char character, int col, int history_row, char attribute) {
    terminal_history[history_row][col * 2] = character;
    terminal_history[history_row][col * 2 + 1] = attribute;
}


/**
 * @brief Wipes the history buffer and resets the display.
 */
void clear_screen() {
    for (int i = 0; i < MAX_HISTORY; i++) {
        for (int j = 0; j < MAX_COLS * 2; j += 2) {
            terminal_history[i][j] = ' ';
            terminal_history[i][j + 1] = WHITE_ON_BLACK;
        }
    }
    total_lines_written = 1;
    current_view_row = 0;
    cursor_col = 0;
    refresh_screen();
    set_cursor_offset(get_offset(0, 0));
}


/**
 * @brief Reads the hardware cursor position.
 * @return The 16-bit cursor offset from VGA ports.
 */
static int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

/**
 * @brief Updates the VGA hardware cursor position.
 * @param offset The byte offset from 0xb8000.
 */
static void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));

}

/**
 * @brief Calculates memory offset from coordinates.
 * * @param col Column index.
 * @param row Row index.
 * @return int The byte offset calculated as (row * 80 + col) * 2.
 */
static int get_offset(int col, int row) { 
    return 2 * (row * MAX_COLS + col); 
}

/**
 * @brief Calculates the row number from a memory offset.
 * * @param offset The byte offset.
 * @return int The row index (offset / 160).
 */
static int get_offset_row(int offset) { 
    return offset / (2 * MAX_COLS); 
}

