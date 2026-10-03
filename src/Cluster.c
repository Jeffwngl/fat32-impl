#include "Cluster.h"
#include "BootSector.h"
#include <stdint.h>

// clusters number 0 and 1 are reserved and a valid cluster number starts at 2
int fat32_read_cluster(
    Disk* disk,
    BootSector* bootSector,
    uint32_t N,
    void* buffer
) {
    if (N < 2) {
        return -1;
    }

	uint32_t firstSecOfClus = data_get_start_sector(bootSector) + (N - 2) * bootSector->BPB_SecPerClus;
    uint32_t bytesPerSec = bootSector->BPB_BytesPerSec;
    uint32_t secsPerClus = bootSector->BPB_SecPerClus;

    uint8_t* bytes = buffer;

    for (uint32_t i = 0; i < secsPerClus; ++i) {
        if (disk_read_sector(
                disk->file,
                firstSecOfClus + i,
                bytesPerSec,
                &bytes[i * bytesPerSec]
            )
        ) {
            return -1;
        }
    }

    return 0;
}

// FAT12 and 16 will have different EOC marks
int fat32_is_eoc(
    uint32_t val
) {
    return val >= 0x0FFFFFF8 && val <= 0x0FFFFFFF;
}

int fat32_is_bad_cluster(
    uint32_t val 
) {
    return val == 0x0FFFFFF7;
}

int fat32_is_free_cluster(
    uint32_t val
) {
    return val == 0x00000000;
}
