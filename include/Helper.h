#pragma once

#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>

// little endian helpers - reconstruct int from little endian bytes
// little endian only matters when value uses more than one byte and only for multibyte integers
/*
e.g. FAT32 stores the 16 bit value 0x1234
the bytes are in little endian i.e. 34 12
p[0] = 34, p[1] = 12
p[1] << 8 shifts the little endian byte to the front
0000 0000 0001 0010 -> 0001 0010 0000 0000
then we combine with bitwise OR with both bytes
*/
static uint16_t read_le16(
    const uint8_t* p
) {
    return (uint16_t)p[0] | (uint16_t)p[1] << 8;
}

static uint32_t read_le32(
    const uint8_t* p
) {
    return (uint32_t)p[0]
        | (uint32_t)p[1] << 8
        | (uint32_t)p[2] << 16
        | (uint32_t)p[3] << 24;
}

// reading little endian 32 also follows a similar process
// in this case we read from the buffer and turn to le32
static void write_le32(
    uint8_t* p,
    uint32_t newVal
) {
    p[0] = newVal & 0xFF;
    p[1] = (newVal >> 8) & 0xFF;
    p[2] = (newVal >> 16) & 0xFF;
    p[3] = (newVal >> 24) & 0xFF;
}

#endif
