#ifndef SCREEN_H
#define SCREEN_H

/* Standard VGA Screen dimensions */
#define MAX_ROWS 25
#define MAX_COLS 80

/* VGA Video Memory address */
#define VIDEO_ADDRESS 0xb8000

/* Attribute bytes for colors */
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen I/O Ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public Kernel API */
void clear_screen();
void kprint(char *message);
void kprint_at(char *message, int col, int row);

#endif
