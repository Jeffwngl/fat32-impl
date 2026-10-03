#include "BootSector.h"

#include <stdint.h>
#include <string.h>

#include "Helper.h"

/*
sector offsets are based on: 
https://elm-chan.org/docs/fat_e.html
*/
int fat32_parse_boot_sector(
    BootSector* bootSector,
    const uint8_t* sector
) {
    memcpy(bootSector->BS_JmpBoot, &sector[0], 3);
    
    memcpy(bootSector->BS_OEMName, &sector[3], 8);
    bootSector->BS_OEMName[8] = '\0';
    
    bootSector->BPB_BytesPerSec = read_le16(&sector[11]);

    bootSector->BPB_SecPerClus = sector[13];

    bootSector->BPB_ReservedSecCnt = read_le16(&sector[14]);

    bootSector->BPB_NumFATs = sector[16];

    bootSector->BPB_RootEntCnt = read_le16(&sector[17]);

    bootSector->BPB_TotalSec16 = read_le16(&sector[19]);

    bootSector->BPB_Media = sector[21];

    bootSector->BPB_FATSz16 = read_le16(&sector[22]);

    bootSector->BPB_SecPerTrack = read_le16(&sector[24]);

    bootSector->BPB_NumHeads = read_le16(&sector[26]);

    bootSector->BPB_HiddenSec = read_le32(&sector[28]);

    bootSector->BPB_TotalSec32 = read_le32(&sector[32]);
    
    // starting at offset 36, FAT32 will have different
    // fields compared to FAT16/12.

    bootSector->BPB_FATSz32 = read_le32(&sector[36]); 

    bootSector->BPB_ExtFlags = read_le16(&sector[40]);

    bootSector->BPB_FSVer = read_le16(&sector[42]);
    
    bootSector->BPB_RootClus = read_le32(&sector[44]);

    bootSector->BPB_FSInfo = read_le16(&sector[48]);

    bootSector->BPB_BkupBootSec = read_le16(&sector[50]);
    
    memcpy(bootSector->BPB_Reserved, &sector[52], 12);

    bootSector->BS_DrvNum = sector[64];

    bootSector->BS_Reserved = sector[65];

    bootSector->BS_BootSig = sector[66];

    bootSector->BS_VolID = read_le32(&sector[67]);

    memcpy(bootSector->BS_VolLab, &sector[71], 11);
    bootSector->BS_VolLab[11] = '\0';

    memcpy(bootSector->BS_FileSysType, &sector[82], 8);
    bootSector->BS_FileSysType[8] = '\0';

    memcpy(bootSector->BS_BootCode32, &sector[90], 420);

    bootSector->BS_Sign = read_le16(&sector[510]);

    return 0;
}

int fat32_validate_boot_sector(
    const BootSector* bootSector
) {
    if (bootSector == NULL) {
        return -1;
    }

    if (bootSector->BS_Sign != 0xAA55) {
        return -1;
    }
    
    /*
    Microsoft supports 512, 1024, 2048 or 4096 bytes, however,
    some FAT drivers assume it to be 512, 512 is used here to
    maximize compatibility.
    */
    if (bootSector->BPB_BytesPerSec != 512) {
        return -1;
    }
    
    /*
    sectors per cluster must be non zero and in factors of 2
    x & (x - 1) becomes zero for powers of 2.
    credit to:
    https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
    */
    if (bootSector->BPB_SecPerClus == 0 
            || ((bootSector->BPB_SecPerClus 
                    & (bootSector->BPB_SecPerClus - 1)) != 0)) {
        return -1;
    }

    if (bootSector->BPB_ReservedSecCnt == 0) {
        return -1;
    }

    if (bootSector->BPB_NumFATs == 0) {
        return -1;
    }
    // FAT32 specific requirements
    if (bootSector->BPB_RootEntCnt != 0) {
        return -1;
    }

    if (bootSector->BPB_TotalSec16 != 0) {
        return -1;
    }

    if (bootSector->BPB_FATSz16 != 0) {
        return -1;
    }

    if (bootSector->BPB_FATSz32 == 0) {
        return -1;
    }

    if (bootSector->BPB_TotalSec32 == 0) {
        return -1;
    }

    if (bootSector->BPB_RootClus < 2) {
        return -1;
    }

    return 0;
}

// returns where the whole FAT region begins
uint32_t fat32_get_start_sector(
    const BootSector* bootSector
) {
    return bootSector->BPB_ReservedSecCnt;
}

uint32_t fat32_region_get_size(
    const BootSector* bootSector
) {
    return bootSector->BPB_FATSz32 * bootSector->BPB_NumFATs;
}

uint32_t root_dir_get_start_sector(
    const BootSector* bootSector
) {
    return (uint32_t)fat32_get_start_sector(bootSector) + fat32_region_get_size(bootSector);
}

uint32_t root_dir_get_size(
    const BootSector* bootSector
) {
    return (
        32 * bootSector->BPB_RootEntCnt
        + bootSector->BPB_BytesPerSec - 1
        )
        / bootSector->BPB_BytesPerSec;
}

uint32_t data_get_start_sector(
    const BootSector* bootSector
) {
    return root_dir_get_start_sector(bootSector) + root_dir_get_size(bootSector);
}

uint32_t data_get_size(
    const BootSector* bootSector
) {
    return bootSector->BPB_TotalSec32 - data_get_start_sector(bootSector);
}
