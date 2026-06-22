#ifndef UTIL_H
#define UTIL_H

#include "../cpu/types.h"

void memory_copy(char *source, char *dest, int nbytes);
void memory_set(u8 *dest, u8 val, u32 len);
void itoa(int n, char str[]);
int strcmp(const char *s1, const char *s2);
int strlen(const char *s);

#endif