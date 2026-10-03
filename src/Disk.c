#include "Disk.h"
#include <stdint.h>
#include <stdio.h>

int disk_read_sector(
	FILE *disk,
	uint32_t sector,
    uint32_t bytesPerSector,
    void* buffer
) {
    uint64_t offset = (uint64_t)bytesPerSector * (uint64_t)sector;
    
    if (fseek(disk, offset, SEEK_SET) != 0) {
        return -1;
    }

    if (fread(buffer, 1, bytesPerSector, disk) != bytesPerSector) {
        return -1;
    }

    return 0;
}

int disk_write_sector(
    FILE* disk,
    uint32_t sector,
    uint32_t bytesPerSector,
    const void* buffer
) {
    uint64_t offset = (uint64_t)bytesPerSector * (uint64_t)sector;

    if (fseek(disk, offset, SEEK_SET) != 0) {
        return -1;
    }

    if (fwrite(buffer, 1, bytesPerSector, disk) != bytesPerSector) {
        return -1;
    }

    return 0;
}

// close an open file
int f_close(
	FILE *file
)

// read a file
int f_read(
	FILE *file
)

// write to a file
int f_write(
	FILE *file
)


