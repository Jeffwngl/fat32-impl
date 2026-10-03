#pragma once

#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#include "BootSector.h"
#include "Disk.h"

/*
contains the functions of the File Partition Table,
the FAT is a giant array of 4 byte entries where there
is one entry for every cluster in the data area.
*/

typedef struct {
    uint32_t fat_start_sector;
    uint32_t data_start_sector;
    
    uint32_t sectors_per_cluster;
    uint32_t bytes_per_sector;

    uint32_t root;
} FAT;

enum FAT_Type {
    FAT12,
    FAT16,
    FAT32
};

int init_FAT(
    FAT* fat,
    BootSector* bootSector
);

int check_FAT(
    FAT* fat,
    BootSector* bootSector,
    Disk* disk
);

enum FAT_Type get_FAT_type(
    BootSector* bootSector
);

int fat32_initialize_fat_table(
    Disk* disk,
    FAT* fat,
    BootSector* bootSector
);

uint32_t fat32_get_entry_sector(
    BootSector* bootSector,
    uint32_t N
);

uint32_t fat32_get_entry_offset(
    BootSector* bootSector,
    uint32_t N
);

int fat32_load_value(
    Disk* disk,
    BootSector* bootSector,
    uint32_t N
);

int fat32_store_value(
    Disk* disk,
    BootSector* bootSector,
    uint32_t N,
    uint32_t newVal
);

#endif
