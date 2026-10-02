#include "../include/BootSector.h"
#include <stdint.h>

// little endian helpers - reconstruct int from little endian bytes
// little endian only matters when value uses more than one byte and only for multibyte integers
/*
e.g. FAT32 stores the 16 bit value 0x1234
the bytes are in little endian i.e. 34 12
p[0] = 34, p[1] = 12
p[1] << 8 shifts the little endian byte to the front
0000 0000 0001 0010 -> 0001 0010 0000 0000
then we combine with bitwise XOR with both bytes
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
    // fields compared to FAT316/12.

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
