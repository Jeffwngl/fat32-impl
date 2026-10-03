#include "FAT.h"

#include <stdint.h>
#include <stdlib.h>

#include "BootSector.h"
#include "Helper.h"

int init_FAT(
    FAT* fat, 
    BootSector *bootSector
) {
    fat->fat_start_sector = fat32_get_start_sector(bootSector);
    fat->data_start_sector = data_get_start_sector(bootSector);
    fat->bytes_per_sector = bootSector->BPB_BytesPerSec;
    fat->sectors_per_cluster = bootSector->BPB_SecPerClus;
    fat->root = bootSector->BPB_RootClus;

    return 0;
}

// the FAT type is determined by the number of clusters on the volume and nothing else.
enum FAT_Type get_FAT_type(
    BootSector* bootSector
) {
    uint32_t numClusters = data_get_size(bootSector) / bootSector->BPB_SecPerClus;

    if (numClusters <= 4085) {
        return FAT12;
    }
    else if (numClusters <= 65525) {
        return FAT16;
    }
    else {
        return FAT32;
    }
}

// returns specific byte position inside the FAT where entry for a cluster is stored
uint32_t fat32_get_entry_sector(
    BootSector* bootSector,
    uint32_t N
) {
    return bootSector->BPB_ReservedSecCnt + (N * 4 / bootSector->BPB_BytesPerSec);
}

uint32_t fat32_get_entry_offset(
    BootSector* bootSector, 
    uint32_t N
) {
    return (N * 4) % bootSector->BPB_BytesPerSec;
}

/*
the FAT entry of a FAT32 volume occupies 32 bits, but it's upper 4 bits are reserved, only the lower 28 bits are valid, these upper bits are initialized to 0.
*/
int fat32_load_value(
    Disk* disk,
    BootSector* bootSector,
    uint32_t N
) {
    uint32_t fatSector = fat32_get_entry_sector(bootSector, N); 
    uint32_t fatOffset = fat32_get_entry_offset(bootSector, N);
    uint32_t bytesPerSec = bootSector->BPB_BytesPerSec;
    uint8_t* buffer = malloc(bytesPerSec);

    disk_read_sector(disk->file, fatSector + fatOffset, bytesPerSec, buffer);

    uint32_t val = read_le32(buffer);

    free(buffer);

    // bit mask to clear upper 4 reserved bits
    return val & 0x0FFFFFFF;
}

int fat32_store_value(
    Disk* disk,
    BootSector* bootSector,
    uint32_t N,
    uint32_t newVal
) {
    uint32_t fatSector = fat32_get_entry_sector(bootSector, N); 
    uint32_t fatOffset = fat32_get_entry_offset(bootSector, N);
    uint32_t bytesPerSector = bootSector->BPB_BytesPerSec;
    uint8_t* buffer = malloc(bytesPerSector);

    if (buffer == NULL) {
        return -1;
    }

    if (disk_read_sector(
            disk->file,
            fatSector,
            bytesPerSector,
            buffer
        ) != 0
    ) {
        free(buffer);
        return -1;
    }

    uint32_t currVal = read_le32(&buffer[fatOffset]);

    uint32_t finalVal = (currVal & 0xF0000000) | (newVal & 0x0FFFFFFF);

    write_le32(&buffer[fatOffset], finalVal);

    if (disk_write_sector(
            disk->file,
            fatSector,
            bytesPerSector,
            buffer) != 0
    ) {
        free(buffer);
        return -1;
    }

    free(buffer);

    return 0;
}

