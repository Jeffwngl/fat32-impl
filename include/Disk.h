#pragma once

#ifndef DISK_H
#define DISK_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	FILE *file;
	uint32_t sector_size;
} Disk;

// reads from disk sector to buffer
int disk_read_sector(
	FILE *disk,
	uint32_t sector,
    uint32_t bytesPerSector,
    void* buffer
);

// writes from buffer to disk sector
int disk_write_sector(
    FILE* disk,
    uint32_t sector,
    uint32_t bytesPerSector,
    const void* buffer
);

#endif
