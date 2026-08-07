/**
 * @file string.c
 * @brief Utility functions for string manipulation and conversion.
 */
#include "util.h"

/**
 * @brief Copies a block of memory from one location to another.
 *
 * This is the kernel's version of 'memcpy'. It copies @p nbytes 
 * from the @p source address to the @p dest address.
 *
 * @note This implementation does not handle overlapping memory regions. 
 * If source and destination overlap, the result is undefined.
 *
 * @param source Pointer to the memory area to copy from.
 * @param dest   Pointer to the memory area to copy to.
 * @param nbytes Number of bytes to copy.
 */
void memory_copy(char* source, char* dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        /* *(dest + i) is the same as dest[i] */
        *(dest + i) = *(source + i);
    }
}

/**
 * @brief Fills a block of memory with a specific byte value.
 *
 * This is the kernel's version of 'memset'. It is frequently used 
 * to "zero out" structures or clear a buffer before use.
 *
 * @param dest Pointer to the memory block to fill.
 * @param val  The byte value to be set (e.g., 0 for clearing).
 * @param len  The number of bytes to set.
 */
void memory_set(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    /* Move through memory, setting the value and advancing the pointer */
    for ( ; len != 0; len--) {
        *temp++ = val;
    }
}

/**
 * @brief Reverses a string in-place.
 *
 * Takes a null-terminated string and swaps characters from 
 * both ends until it reaches the center.
 *
 * @param s The string to be reversed.
 */
void reverse(char s[]) {
    int i, j;
    char temp;

    // 1. Find the length of the string (excluding the null terminator)
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    
    // 2. Use two pointers: 'i' at the start, 'j' at the end
    for (i = 0, j = len - 1; i < j; i++, j--) {
        // Swap characters using a temporary variable
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

/**
 * @brief Converts an integer to a null-terminated string (ASCII).
 *
 * This function takes a signed integer and converts it into a string representation.
 * It handles negative numbers by storing the sign and appending a '-' character.
 *
 * @param n   The integer to be converted.
 * @param str The character buffer where the result will be stored. 
 * Ensure the buffer is large enough (at least 12 bytes for a 32-bit int).
 */
void itoa(int n, char str[]) {
    int i, sign;

    // If n is less than zero, change to positive first since there's no negative ascii
    if ((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    do {
        str[i++] = n % 10 + '0'; // makasih bu diana
    } while ((n /= 10) > 0);

    // Put the negative sign back if it was originally negative
    if (sign < 0) {
        str[i++] = '-';
    } 
    
    str[i] = '\0';

    // Flip the string so it reads correctly (e.g., "-31" becomes "-13")
    reverse(str);
}

/**
 * @brief Compares two null-terminated strings lexicographically.
 *
 * @param s1 Pointer to the first string.
 * @param s2 Pointer to the second string.
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2.
 */
int strcmp(const char* s1, const char* s2){
    while(*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * @brief Returns the length of a null-terminated string.
 *
 * @param s Pointer to the string.
 * @return The number of characters before the null terminator.
 */
int strlen(const char* s) {
    int n = 0;
    while(*s++) {
        n++;
    }

    return n;
}

/**
 * @brief Copies src into dest, always null-terminating, never overflowing.
 * @param dest_size Total size of the dest buffer, including the null byte.
 */
void sstrncpy(char *dest, const char *src, int dest_size) {
    if (dest_size <= 0) return;

    int i = 0;
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

char *strchr(const char *s, char c) {
    while (*s) {
        if (*s == c) return (char *)s;
        s++;
    }
    return 0;
}