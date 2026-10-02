#pragma once

#ifndef BOOTSECTOR_H
#define BOOTSECTOR_H

#include <stdint.h>
#include <string.h>

/*
the boot sector in the first sector of the disk, with approximately 512 bytes of code.
the boot code is designed to be bootable, the BIOS/UEFI jumps here to start loading the operation system.

a uint8_t is 1 byte
a uint16_t is 2 bytes
a uint32_t is 4 bytes
*/

typedef struct {

    // general fields
	uint8_t BS_JmpBoot[3];
	char BS_OEMName[9]; // allow one extra byte for null terminator
	uint16_t BPB_BytesPerSec;
    uint8_t BPB_SecPerClus;
    uint16_t BPB_ReservedSecCnt;
    uint8_t BPB_NumFATs;
    uint16_t BPB_RootEntCnt; // for FAT12/16, it defines the number of directory entries in the root directory, for FAT32, this is field is 0
    uint16_t BPB_TotalSec16; // volume size or total sectors in volume in old 16 bit field, for FAT32, this is 0
    uint8_t BPB_Media; // describes the type of media, e.g. floppy disk, partitioned disks etc.
    uint16_t BPB_FATSz16; // size of FAT, number of sectors occupied by FAT
    uint16_t BPB_SecPerTrack; // number of sectors per track, only used for media that have geometry
    uint16_t BPB_NumHeads; // only used for media that have geometry and used for BIOS and IBM pc
    uint32_t BPB_HiddenSec; // number of hidden physical sectors after the FAT volume, usually used for IBM PC
    uint32_t BPB_TotalSec32; // volume soze of the FAT volume in 32 bit field

    // FAT32 specific fields
    uint32_t BPB_FATSz32;
    uint16_t BPB_ExtFlags;
    uint16_t BPB_FSVer;
    uint32_t BPB_RootClus;
    uint16_t BPB_FSInfo;
    uint16_t BPB_BkupBootSec;
    uint32_t BPB_Reserved[3];
    uint8_t BS_DrvNum;
    uint8_t BS_Reserved;
    uint8_t BS_BootSig;
    uint32_t BS_VolID;
    uint8_t BS_VolLab[11];
    char BS_FileSysType[8];
    uint8_t BS_BootCode32[420];
    uint16_t BS_Sign; // boot signature indicating that this is a valid boot sector, has to be 0xAA55, always allocated at offset 510

} BootSector;

int fat32_parse_boot_sector(
    BootSector* bootSector,
    const uint8_t* sector
);

int fat32_validate_boot_sector(
    BootSector* bootSector
);

uint16_t fat32_get_offset(
    BootSector* bootSector
);

uint32_t fat32_get_size(
    BootSector* bootSector
);

uint16_t root_dir_get_offset(
    BootSector* bootSector
);

uint32_t root_dir_get_size(
    BootSector* bootSector
);

uint32_t data_get_offset(
    BootSector* bootSector
);

uint32_t data_get_size(
    BootSector* bootSector
);

#endif
